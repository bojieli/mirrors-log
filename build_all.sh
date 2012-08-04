INFOBRIGHT="/home/boj/infobright/mysqld-ib"
MYSQL="/home/boj/infobright/mysql-ib"

$INFOBRIGHT start
$MYSQL -u root -e "DROP DATABASE IF EXISTS mirrors;"
$MYSQL -u root -e "CREATE DATABASE mirrors;"

# import log data
cd ./import/
bash ./import_new.sh | tee ./import.log
# import files date
cd ../crawl/
bash ./load.sh
# find sessions
cd ../findsession/
bash ./find.sh | tee ./find.log
# stat request info of files
cd ../filestat_size/
bash ./stat.sh | tee ./stat.log
# plot graphs
cd ../plot/
bash ./plotall.sh
# build PDF
cd ../latex/
xelatex ./mirrors.tex
