<?php
$stdin = fopen('php://stdin', 'r');
$stdout = fopen('php://stdout', 'w');

$i = 0;
$mod = 1;
$minsample = 1000;
$nexti = $minsample;
$lastfields = array();
$fields = array();
$acc = array();
while ($line = fgets($stdin)) {
	$i++;
	while ($i > $nexti) {
		$mod++;
		$nexti = floor(pow(1.05, $mod)) + $minsample;
	}
	$fields = explode("\t", trim($line));
	foreach ($fields as $idx => $value) {	
		if (!isset($acc[$idx]))
			$acc[$idx] = 0;
		$acc[$idx] += $value;
	}
	if ($i % $mod == 0) {
		fputs($stdout, implode("\t", $acc)."\n");
		$lastfields = $fields;
	}
}
if ($i % $mod != 0)
	fputs($stdout, implode("\t", $acc)."\n");
