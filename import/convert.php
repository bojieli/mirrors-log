<?php
if (empty($argv[1]) || empty($argv[2]))
	die('Usage: convert.php inputfile outputfile');

$fpi = fopen($argv[1], 'r');
if (empty($fpi))
	die('Cannot open input file');
$fpo = fopen($argv[2], 'w');
if (empty($fpo))
	die('Cannot open output file');

date_default_timezone_set('UTC');

$processed = 0;
while ($line = stream_get_line($fpi, 4096, "\n")) {
	$processed++;
	if ($processed % 10000 == 0)
		echo "$processed\t";
	
	$fields = array(
		'ipv4'=>'',
		'ipv4_0'=>'',
		'ipv4_1'=>'',
		'ipv4_2'=>'',
		'ipv4_3'=>'',
		'ipv6_0'=>'',
		'ipv6_1'=>'',
		'ipv6_2'=>'',
		'ipv6_3'=>'',
		'time'=>'',
		'year'=>'',
		'month'=>'',
		'yearday'=>'',
		'monthday'=>'',
		'weekday'=>'',
		'daymin'=>'',
		'daytime'=>'',
		'hour'=>'',
		'status'=>'',
		'length'=>'',
		'url'=>'',
		'url_0'=>'',
		'url_1'=>'',
		'url_2'=>'',
		'url_3'=>'',
		'url_4'=>'',
		'url_5'=>'',
		'url_6'=>'',
		'url_7'=>'',
		'url_8'=>'',
		'url_9'=>'',
		'filename'=>'',
		'filename_noext'=>'',
		'extension'=>'',
		'referer'=>'',
		'ua_0'=>'',
		'ua'=>''
		);
	
	preg_match('|^([^ ]+) ([^ ]+) ([^ ]+) \[(.+)\] "(.+)" (\d+) (\d+) "(.+)" "(.+)"$|', $line, $components);
	if (count($components) != 10)
		continue;
	// component 0 is original line

	// IP
	$ip = $components[1];
	if (preg_match('/([0-9]+\.){3}[0-9]+/', $ip)) {
		$ip_comps = explode('.', $ip);
		$ip = 0;
		for ($i=0; $i<4; $i++) {
			$ip *= 256;
			$ip += $ip_comps[$i];
			$fields["ipv4_$i"] = $ip_comps[$i];
		}
		$fields['ipv4'] = $ip;
	} else {
		$ip_comps = explode(':', $ip);
		if (count($ip_comps) < 8) {
			$ip_comps[7] = $ip_comps[count($ip_comps) - 1];
			for ($i=count($ip_comps)-1; $i<7; $i++)
				$ip_comps[$i] = 0;
		}
		for ($i=0; $i<4; $i++) {
			$tmp = 65536 * base_convert($ip_comps[$i*2], 16, 10);
			$tmp += base_convert($ip_comps[$i*2+1], 16, 10);
			$fields["ipv6_$i"] = $tmp;
		}
	}

	// I don't know what components 2 and 3 stand for

	// time
	// 11/Jul/2012:06:25:51 +0800
	//$fields['time'] = strtotime($components[4]);
	$timestr = explode(' ', $components[4]);
	$time = strptime($timestr[0], "%d/%b/%Y:%H:%M:%S");
	$fields['daymin'] = $time['tm_min'] + $time['tm_hour'] * 60;
	$fields['daytime'] = $time['tm_sec'] + $fields['daymin'] * 60;
	$fields['hour'] = $time['tm_hour'];
	$fields['year'] = $time['tm_year'] + 1900;
	$fields['month'] = $time['tm_mon'];
	$fields['yearday'] = $time['tm_yday'];
	$fields['monthday'] = $time['tm_mday'];
	$fields['weekday'] = $time['tm_wday'];

	// DIE DIE mktime different from strptime and UNIX mktime: month is 1~12 instead of 0~11
	$fields['time'] = mktime($time['tm_hour'], $time['tm_min'], $time['tm_sec'], $time['tm_mon'] + 1, $time['tm_mday'], $time['tm_year'] + 1900);

	// url
	$req_header = explode(' ', $components[5]);
	// GET /centos/5/os/i386/repodata/repomd.xml HTTP/1.1
	if (!empty($req_header[1])) {
		$fields['url'] = substr($req_header[1], 0, 255);
		$paths = explode('/', $req_header[1]);
		// path: /centos/5/os/i386/repodata/repomd.xml
		// url_* not include filename
		$count = count($paths)-2 < 10 ? count($paths)-2 : 10;
		for ($i=0; $i<$count; $i++)
			$fields["url_$i"] = substr($paths[$i+1], 0, 255);
		
		$filename = $paths[count($paths)-1];
		$fields['filename'] = substr($filename, 0, 255);
		if (strpos($filename, '.')) {
			$separator = strrpos($filename, '.');
			$fields['filename_noext'] = substr($filename, 0, $separator);
			$fields['extension'] = substr($filename, $separator + 1);
			// some strange files use timestamp as extension
			if (strlen($fields['extension']) > 4) {
				$fields['extension'] = '';
				$fields['filename_noext'] = $filename;
			}
		} else {
			$fields['filename_noext'] = $filename;
		}
		$fields['filename_noext'] = substr($fields['filename_noext'], 0, 255);
	}

	$fields['status'] = $components[6];
	$fields['length'] = $components[7];

	// referer
	if ($components[8] != '-')
		$fields['referer'] = substr($components[8], 0, 255);
	
	// ua
	if ($components[9] != '-') {
		$fields['ua'] = substr($components[9], 0, 255);
		preg_match('/^[a-zA-Z0-9]+/', $fields['ua'], $matches);
		if (isset($matches[0]) && strlen($matches[0]) <= 31)
			$fields['ua_0'] = $matches[0];
	}

	foreach ($fields as $key => $value)
		$fields[$key] = addcslashes($value, "'\"\t\r\n\\");

	fputs($fpo, implode("\t", $fields) . "\n");
}
echo "Complete\n";
fclose($fpi);
fclose($fpo);
