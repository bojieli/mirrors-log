<?php
$stdin = fopen('php://stdin', 'r');
$stdout = fopen('php://stdout', 'w');

$i = 0;
$mod = 300;
$acc = 0;
while ($line = fgets($stdin)) {
	$i++;
	$fields = explode("\t", trim($line));
	$acc += $fields[1];
	$fields[2] = $acc;
	if ($fields[0] > 1000000000 || $i % $mod == 0)
		fputs($stdout, implode("\t", $fields)."\n");
}
