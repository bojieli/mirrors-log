~/infobright/mysqld-ib start
mkdir tmp
mkdir ps
DIRNAME=`dirname $0`
for f in `ls --color=no $DIRNAME | grep -v plotall | grep -v php`; do echo $f; bash ./$f; done
