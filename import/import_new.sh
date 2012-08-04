#!/bin/bash
~/infobright/mysql-ib -u root -e "source /home/boj/mirrors_log/import/db_init_new.sql";
rm decompressed
rm parsed
mkfifo decompressed
mkfifo parsed

# in time increasing order!
for (( i=52; i>=2; i--)) do
date
echo Processing access.log.$i.gz;
zcat ../data/access.log.$i.gz > decompressed &
php convert.php decompressed parsed &
~/infobright/mysql-ib -u root -e "USE mirrors; set @bh_dataformat = 'txt_variable'; LOAD DATA INFILE '/home/boj/mirrors_log/import/parsed' INTO TABLE \`log\` FIELDS TERMINATED BY '\t' ESCAPED BY '\\\\' LINES TERMINATED BY '\n'"
done
date

rm decompressed
rm parsed
