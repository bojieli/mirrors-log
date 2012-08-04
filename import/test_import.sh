#!/bin/bash
~/infobright/mysqld-ib start
~/infobright/mysql-ib -u root -e "source /home/boj/mirrors_log/db_init.sql";
rm decompressed
rm parsed
mkfifo decompressed
mkfifo parsed

i=6
date
echo Processing access.log.$i.gz;
zcat access.log.$i.gz > decompressed &
php convert.php decompressed parsed &
~/infobright/mysql-ib -u root -e "USE mirrors; set @bh_dataformat = 'txt_variable'; LOAD DATA INFILE '/home/boj/mirrors_log/parsed' INTO TABLE \`log\` FIELDS TERMINATED BY '\t' ESCAPED BY '\\\\' LINES TERMINATED BY '\n'"
date

rm decompressed
rm parsed
