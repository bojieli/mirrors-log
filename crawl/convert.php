<?php
$fpi = fopen("php://stdin", "r");
$fpo = fopen("php://stdout", "w");

while ($line = fgets($fpi)) {
	$row = explode("\t", $line);
	save($row);
}
fclose($fpi);
fclose($fpo);

define('START_TIME', time());
function save($row) {
	static $processed = 0;
	$processed++;
	
	$fields = array(
		'id' => $processed,
		'url'=> '',
		'url_0'=> $row[0],
		'url_1'=> $row[1],
		'url_2'=> $row[2],
		'url_3'=> $row[3],
		'url_4'=> $row[4],
		'url_5'=> $row[5],
		'url_6'=> $row[6],
		'url_7'=> $row[7],
		'url_8'=> $row[8],
		'url_9'=> $row[9],
		'filename'=> $row[10].($row[11] ? '.'.$row[11] : ''),
		'filename_noext'=> $row[10],
		'extension'=> $row[11],
		'filesize' => trim($row[12])
	);
	$fields['url'] = '/';
	for ($i=0; $i<10; $i++)
		if (!empty($fields["url_$i"]))
			$fields['url'] .= $fields["url_$i"].'/';
		else break;
	if (!empty($fields['filename']))
		$fields['url'] .= $fields['filename'];
	
        global $fpo;
        fputs($fpo, implode("\t", $fields) . "\n");
}
