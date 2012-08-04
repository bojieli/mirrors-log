<?php
$stdin = fopen('php://stdin', 'r');
$stdout = fopen('php://stdout', 'w');

$i = 0;
$mod = 1;
$minsample = 1000;
$nexti = $minsample;
$acc = 0;
while ($line = fgets($stdin)) {
	$i++;
	while ($i > $nexti) {
		$mod++;
		$nexti = floor(pow(1.05, $mod)) + $minsample;
	}
	$fields = explode("\t", trim($line));
	$acc += $fields[1];
	$fields[2] = $acc;
	if ($i % $mod == 0)
		fputs($stdout, implode("\t", $fields)."\n");
}
