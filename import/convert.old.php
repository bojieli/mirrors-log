<?php
if (empty($argv[1]) || empty($argv[2]))
	die('Usage: convert.php inputfile outputfile');

$fpi = fopen($argv[1], 'r');
if (empty($fpi))
	die('Cannot open input file');
$fpo = fopen($argv[2], 'w');
if (empty($fpo))
	die('Cannot open output file');

function base16_to_int($str) {
	$len = strlen($str);
	$ret = 0;
	for ($i=0; $i<$len; $i++) {
		$ret *= 16;
		if ($str[$i] >= '0' && $str[$i] <= '9')
			$ret += ord($str[$i]) - ord('0');
		else if ($str[$i] >= 'a' && $str[$i] <= 'z')
			$ret += ord($str[$i]) - ord('a') + 10;
		else if ($str[$i] >= 'A' && $str[$i] <= 'Z')
			$ret += ord($str[$i]) - ord('A') + 10;
		else // invalid base16
			return 0;
	}
	return $ret;
}

date_default_timezone_set('UTC');

$processed = 0;
while ($line = fgets($fpi)) {
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
		'yearday'=>'',
		'weekday'=>'',
		'daytime'=>'',
		'hour'=>'',
		'status'=>'',
		'length'=>'',
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
		'extension'=>'',
		'referer'=>'',
		'ua_0'=>'',
		'ua'=>''
		);
	
	preg_match('|^(.+) (.+) (.+) \[(.+)\] "(.+)" (\d+) (\d+) "(.+)" "(.+)"$|', $line, $components);
	if (count($components) != 10)
		continue;
	// component 0 is original line

	// IP
	$ip = $components[1];
	$ip_comps = explode('.', $ip);
//	if (preg_match('/([0-9]+\.){3}[0-9]+/', $ip)) {
	if (count($ip_comps) == 4) {
//		$ip_comps = explode('.', $ip);
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
			$tmp = 65536 * base16_to_int($ip_comps[$i*2]);
			$tmp += base16_to_int($ip_comps[$i*2+1]);
			$fields["ipv6_$i"] = $tmp;
		}
	}

	// I don't know what components 2 and 3 stand for

	// time
	// 11/Jul/2012:06:25:51 +0800
	$fields['time'] = strtotime($components[4]);
	$timestr = explode(' ', $components[4]);
	$time = strptime($timestr[0], "%d/%b/%Y:%H:%M:%S");
	$fields['daytime'] = $time['tm_sec'] + $time['tm_min'] * 60 + $time['tm_hour'] * 3600;
	$fields['hour'] = $time['tm_hour'];
	$fields['yearday'] = $time['tm_yday'];
	$fields['weekday'] = $time['tm_wday'];
	$fields['year'] = $time['tm_year'] + 1900;

	// DIE DIE mktime different from strptime and UNIX mktime: month is 1~12 instead of 0~11
	$fields['time'] = mktime($time['tm_hour'], $time['tm_min'], $time['tm_sec'], $time['tm_mon'] + 1, $time['tm_mday'], $time['tm_year'] + 1900);

//	preg_match("|([0-9]+)/([a-zA-Z]+)/([0-9]{4}):([0-9]{2}):([0-9]{2}):([0-9]{2}):([0-9]{2})|", $time, $matches);

	// url
	$req_header = explode(' ', $components[5]);
	// GET /centos/5/os/i386/repodata/repomd.xml HTTP/1.1
	if (!empty($req_header[1])) {
		$paths = explode('/', $req_header[1]);
		// path: /centos/5/os/i386/repodata/repomd.xml
		// url_* not include filename
		$count = count($paths)-2 < 10 ? count($paths)-2 : 10;
		for ($i=0; $i<$count; $i++)
			$fields["url_$i"] = $paths[$i+1];
		
		$filename = $paths[count($paths)-1];
		if (strpos($filename, '.')) {
			$separator = strrpos($filename, '.');
			$fields['filename'] = substr($filename, 0, $separator);
			$fields['extension'] = substr($filename, $separator + 1);
		} else {
			$fields['filename'] = $filename;
		}
	}

	$fields['status'] = $components[6];
	$fields['length'] = $components[7];

	// referer
	if ($components[8] != '-')
		$fields['referer'] = $components[8];
	
	// ua
	if ($components[9] != '-') {
		$fields['ua'] = $components[9];
		preg_match('/^[a-zA-Z0-9]+/', $fields['ua'], $matches);
		if (isset($matches[0]))
			$fields['ua_0'] = $matches[0];
	}

	fprintf($fpo, "%s\n", implode('`', $fields));
}
echo "Complete\n";
fclose($fpi);
fclose($fpo);
