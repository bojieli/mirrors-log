<?php
$stdin = fopen('php://stdin', 'r');
$stdout = fopen('php://stdout', 'w');

$i = 0;
$mod = 1;
$minsample = 1000;
$nexti = $minsample;
$accx = 0;
$accy = 0;
$lastfields = array();
$fields = array();
while ($line = fgets($stdin)) {
	$i++;
	while ($i > $nexti) {
		$mod++;
		$nexti = floor(pow(1.05, $mod)) + $minsample;
	}
	$fields = explode("\t", trim($line));
	$accx += $fields[0];
	$accy += $fields[1];
	$fields[2] = $accx;
	$fields[3] = $accy;
	if ($i % $mod == 0 || $accx != 0 && ($accx - $lastfields[2]) / $accx > 0.1 || $accy != 0 && ($accy - $lastfields[3]) / $accy > 0.1) {
		fputs($stdout, implode("\t", $fields)."\n");
		$lastfields = $fields;
	}
}
if ($i % $mod != 0)
	fputs($stdout, implode("\t", $fields)."\n");
