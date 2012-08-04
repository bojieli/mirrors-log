<?php
$stdin = fopen('php://stdin', 'r');
$stdout = fopen('php://stdout', 'w');

$i = 0;
$mod = 300;
$accx = 0;
$accy = 0;
while ($line = fgets($stdin)) {
	$i++;
	$fields = explode("\t", trim($line));
	$accx += $fields[0];
	$accy += $fields[1];
	$fields[2] = $accx;
	$fields[3] = $accy;
	if ($fields[0] > 1000000000 || $i % $mod == 0)
		fputs($stdout, implode("\t", $fields)."\n");
}
