#!/bin/bash
INFOBRIGHT="/home/boj/infobright/mysql-ib"
DIR=`pwd`

$INFOBRIGHT -u root -e "source $DIR/db_init_new.sql";
date

rm output_ftp_new
rm output_http_new
mkfifo output_ftp_new
mkfifo output_http_new

php convert.php < output_ftp > output_ftp_new &
$INFOBRIGHT -u root -e "USE mirrors; set @bh_dataformat = 'txt_variable'; LOAD DATA INFILE '$DIR/output_ftp_new' INTO TABLE \`files\` FIELDS TERMINATED BY '\t' ESCAPED BY '\\\\' LINES TERMINATED BY '\n'"

date

php convert.php < output_http > output_http_new &
$INFOBRIGHT -u root -e "USE mirrors; set @bh_dataformat = 'txt_variable'; LOAD DATA INFILE '$DIR/output_http_new' INTO TABLE \`files\` FIELDS TERMINATED BY '\t' ESCAPED BY '\\\\' LINES TERMINATED BY '\n'"

date
rm output_ftp_new
rm output_http_new
