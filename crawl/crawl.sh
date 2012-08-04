#!/bin/bash
INFOBRIGHT="/home/boj/infobright/mysql-ib"
DIR=`pwd`

rm output.fifo
mkfifo output.fifo

$INFOBRIGHT -u root -e "source $DIR/db_init.sql";

date
php crawl.php output.fifo &
$INFOBRIGHT -u root -e "USE mirrors; set @bh_dataformat = 'txt_variable'; LOAD DATA INFILE '$DIR/output.fifo' INTO TABLE \`files\` FIELDS TERMINATED BY '\t' ESCAPED BY '\\\\' LINES TERMINATED BY '\n'"
date
php crawl_ftp.php output.fifo &
$INFOBRIGHT -u root -e "USE mirrors; set @bh_dataformat = 'txt_variable'; LOAD DATA INFILE '$DIR/output.fifo' INTO TABLE \`files\` FIELDS TERMINATED BY '\t' ESCAPED BY '\\\\' LINES TERMINATED BY '\n'"
date

rm output.fifo
