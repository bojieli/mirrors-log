<?php
if (empty($argv[1]) || empty($argv[2]))
	die("Usage: findsession.php SQLhostname OutputFile");
$dbhost = $argv[1];
$dbname = 'mirrors';
$dbuser = 'root';
$dbpass = '';
if (!mysql_connect($dbhost, $dbuser, $dbpass))
	die("Cannot connect to MySQL host!");
if (!mysql_select_db($dbname))
	die("Cannot find database $dbname");

if (!($fp_gap = fopen($argv[2].".gap", "w")))
	die("Cannot open output file (.gap)");
if (!($fp_int = fopen($argv[2].".int", "w")))
	die("Cannot open output file (.int)");

define('BUFFER_SIZE', 10000);
function fetch_row() {
	static $start = 0;
	static $buffered = 0;
	static $rs_handle;
	if ($buffered == 0) {
		$rs_handle = mysql_query("SELECT ipv4, ipv4_0, ipv4_1, ipv4_2, ipv4_3, ipv6_0, ipv6_1, ipv6_2, ipv6_3, time, length, url_0, ua_0 FROM log LIMIT $start, ".BUFFER_SIZE);
		if (!($buffered = mysql_num_rows($rs_handle)))
			return null;
		$start += $buffered;
		if ($start % 10000 == 0)
			echo "$start\t";
	}
	$buffered--;
	return mysql_fetch_array($rs_handle, MYSQL_ASSOC);
}

function new_session(&$row) {
	return array(
		'ipv4' => $row['ipv4'],
		'ipv4_0' => $row['ipv4_0'],
		'ipv4_1' => $row['ipv4_1'],
		'ipv4_2' => $row['ipv4_2'],
		'ipv4_3' => $row['ipv4_3'],
		'ipv6_0' => $row['ipv6_0'],
		'ipv6_1' => $row['ipv6_1'],
		'ipv6_2' => $row['ipv6_2'],
		'ipv6_3' => $row['ipv6_3'],
		'duration' => 0,
		'start_time' => $row['time'],
		'requests' => 1,
		'traffic' => $row['length'],
		'dist' => strlen($row['url_0']) <= 31 ? $row['url_0'] : '',
		'ua_0' => substr($row['ua_0'], 0, 31)
	);
}

define('MAX_GAP', 1800); // 30 minutes
function _within_gap($start_time, $interval, $time) {
	return ($start_time + $interval + MAX_GAP > $time);
}

define('MAX_INTERVAL', 3600); // 60 minutes
function _within_interval($start_time, $interval, $time) {
	return ($start_time + MAX_INTERVAL > $time);
}

function update_sess(&$sess, &$row, &$comp_func) {
	if ($comp_func($sess['start_time'], $sess['duration'], $row['time'])) {
		$sess['duration'] = $row['time'] - $sess['start_time'];
		$sess['requests']++;
		$sess['traffic'] += $row['length'];
		if (empty($sess['dist']) && strlen($row['url_0']) < 31)
			$sess['dist'] = $row['url_0'];
		return true;
	}
	return false;
}

function output(&$row, $fp) {
	foreach ($row as $key => $value)
		$row[$key] = addcslashes($value, "'\"\t\r\n\\");
	fputs($fp, implode("\t", $row)."\n");
	unset($row);
}

// garbage collection
function gc_sess(&$sess, &$sess_count, $comp_func, &$curr_time, $fp) {
	static $gc_limit = 500;
	if ($sess_count > $gc_limit) {
		echo "[GC $sess_count => ";
		foreach ($sess as $hash => $row) {
			if (! $comp_func($row['start_time'], $row['duration'], $curr_time)) {
				output($row, $fp);
				unset($sess[$hash]);
			}
		}
		$sess_count = count($sess);
		$gc_limit = floor($sess_count * 1.5);
		echo "$sess_count]\t";
	}
}

function handle_sess(&$sess, &$sess_count, &$hash, &$row, $comp_func, $fp, &$curr_time) {
	if (!isset($sess[$hash])) {
		$sess[$hash] = new_session($row);
		$sess_count++;
	} else {
		$old = &$sess[$hash];
		if (!update_sess($old, $row, $comp_func)) {
			output($old, $fp);
			unset($old);
			$sess[$hash] = new_session($row);
		}
	}
	gc_sess($sess, $sess_count, $comp_func, $curr_time, $fp);
}


$sess_gap = array(); // gap
$sess_int = array(); // interval
$sess_gap_count = 0;
$sess_int_count = 0;
$curr_time = 0; // assume the records are in time increasing order
while ($row = fetch_row()) {
	$hash = $row['ipv4'].' '.$row['ipv6_0'].' '.$row['ipv6_1'].' '.$row['ipv6_2'].' '.$row['ipv6_3'];
	if ($row['time'] > $curr_time)
		$curr_time = $row['time'];

	handle_sess($sess_gap, $sess_gap_count, $hash, $row, '_within_gap', $fp_gap, $curr_time);
	handle_sess($sess_int, $sess_int_count, $hash, $row, '_within_interval', $fp_int, $curr_time);
}

foreach ($sess_gap as $row)
	output($row, $fp_gap);
fclose($fp_gap);

foreach ($sess_int as $row)
	output($row, $fp_int);
fclose($fp_int);
