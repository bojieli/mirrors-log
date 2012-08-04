<?php
include 'save.inc.php';

$host = "mirrors.ustc.edu.cn";
$dirs = array('CPAN', 'CRAN');

if (!($fp = fopen($argv[1], 'w')))
	die("Cannot open output file");

if (!($ftp = ftp_connect($host)))
	die("Cannot connect to host $host");
if (!(ftp_login($ftp, 'anonymous', '')))
	die("Cannot login $host anonymous");

foreach ($dirs as $dir) {
	traverse($ftp, $dir);
}
fclose($fp);

function traverse($ftp, $path) {
	$lines = ftp_rawlist($ftp, $path);
	foreach ($lines as $line) {
		if (! preg_match("|^([^ ]+ +){4}(\d+) +([^ ]+ +){3}([^ ]+)|", $line, $matches))
			continue;
		$size = $matches[2];
		$name = $matches[4];
		if ($line[0] == 'd') {
			save($path.'/'.$name);
			traverse($ftp, $path.'/'.$name);
		}
		else {
		// drwxr-x---  15 vincent  vincent      4096 Nov  3 21:31 public_html
			save($path.'/'.$name, $size);
		}
	}
}
