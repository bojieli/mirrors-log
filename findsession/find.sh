#!/bin/bash
INFOBRIGHT="/home/boj/infobright/mysql-ib"
MYSQLSOCK="/tmp/mysql-ib.sock"
DIR=`pwd`

rm output.gap
rm output.int
mkfifo output.gap
mkfifo output.int

$INFOBRIGHT -u root -e "source $DIR/db_init.sql";

date
php findsession.php :$MYSQLSOCK output &
$INFOBRIGHT -u root -e "USE mirrors; set @bh_dataformat = 'txt_variable'; LOAD DATA INFILE '$DIR/output.gap' INTO TABLE \`sess\` FIELDS TERMINATED BY '\t' ESCAPED BY '\\\\' LINES TERMINATED BY '\n'" &
$INFOBRIGHT -u root -e "USE mirrors; set @bh_dataformat = 'txt_variable'; LOAD DATA INFILE '$DIR/output.int' INTO TABLE \`sess_int\` FIELDS TERMINATED BY '\t' ESCAPED BY '\\\\' LINES TERMINATED BY '\n'"
date

rm output.gap
rm output.int
