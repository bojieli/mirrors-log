<?php
include 'save.inc.php';

// should not add backslashes
define('BASEURL', 'http://mirrors.ustc.edu.cn');

if (empty($argv[1]))
	die('Usage: crawl.php outputfile');

$fp = fopen($argv[1], 'w');
if (empty($fp))
	die('Cannot open output file');

date_default_timezone_set('UTC');

$index = file(BASEURL);
if (empty($index))
	die('Cannot open url');

foreach ($index as $line) {
	preg_match('|<a href="(/[a-zA-Z0-9_-]+)">|', $line, $components);
	if (!empty($components)) {
		save($components[1].'/');
		traverse($components[1].'/', $index);
	}
}
fclose($fp);

function traverse($path, &$parent) {
	$dir = file(BASEURL . $path);
	if (empty($dir))
		return;

	// check symlink recursion
	if (count($dir) == count($parent)) {
		$num = count($dir);
		// directory listing from the 5th line
		for ($idx=4; $idx<$num; $idx++)
			if ($dir[$idx] !== $parent[$idx])
				break;
		if ($idx == $num)
			return; // this may be a symlink recursion
	}
	
	// files
	foreach ($dir as $idx => $line) {
		if (preg_match('|<a href="([^/"]+)"|', $line, $matches)) {
			preg_match('|(\d+)\cM$|', $line, $filesize);
			if (!empty($filesize))
				save($path.$matches[1], $filesize[1]);
		}
		else if (preg_match('|<a href="([^/"]+/)"|', $line, $matches)
		     && $matches[1] != '../' && $matches[1] != './') {
			save($path.$matches[1]);
			traverse($path.$matches[1], $dir);
		}
	}
}

