<?php
define('START_TIME', time());
function save($path, $filesize = -1) {
	static $processed = 0;
	echo (time()-START_TIME)." [$processed] $path". ($filesize > 0 ? " ($filesize B)" : ''). "\n";
	$processed++;
	
	if (!is_numeric($filesize) || $filesize < 0)
		$filesize = '';

	$fields = array(
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
		'filesize' => $filesize
	);
	
	$paths = explode('/', $path);
	if ($paths[count($paths)-1] === '')
		unset($paths[count($paths)-1]);
	
	if ($fields['filesize'] !== '') { // is a file
		$filename = substr($paths[count($paths)-1], 0, 255);
		if (strpos($filename, '.')) {
			$separator = strrpos($filename, '.');
			$fields['filename'] = substr($filename, 0, $separator);
			$fields['extension'] = substr($filename, $separator + 1);
			// some strange files use timestamp as extension
			if (strlen($fields['extension']) > 7) {
				$fields['extension'] = '';
				$fields['filename'] = $filename;
			}
		} else {
			$fields['filename'] = $filename;
		}
		unset($paths[count($paths)-1]);
	}

	$count = count($paths)-1 < 10 ? count($paths)-1 : 10;
	for ($i=0; $i<$count; $i++)
		$fields["url_$i"] = substr($paths[$i+1], 0, 255);

        foreach ($fields as $key => $value)
                $fields[$key] = addcslashes($value, "'\"\t\r\n\\");

        global $fp;
        fputs($fp, implode("\t", $fields) . "\n");
}
