#!/bin/bash
INFOBRIGHT="/home/boj/infobright/mysql-ib"
DIR=`pwd`

date
rm output.fifo
mkfifo output.fifo

echo "Creating table..."
$INFOBRIGHT -u root -e "source $DIR/db_init.sql"
echo "Compiling..."
gcc -g -DDEBUGTIME -UDEBUG -O2 -o stat ./stat.c -I/usr/include/mysql -L/usr/lib/mysql -lmysqlclient

date
./stat output.fifo &
$INFOBRIGHT -u root -e "USE mirrors; set @bh_dataformat = 'txt_variable'; LOAD DATA INFILE '$DIR/output.fifo' INTO TABLE \`filestat\` FIELDS TERMINATED BY '\t' ESCAPED BY '\\\\' LINES TERMINATED BY '\n'"
date

rm output.fifo
