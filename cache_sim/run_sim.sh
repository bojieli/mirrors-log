#!/bin/bash
rm simulator
gcc -UDEBUG_FAST -DDEBUGTIME -DDEBUG -UDEBUGMEM -O2 -o simulator ./simulator.c -I/usr/include/mysql -L/usr/lib/mysql -lmysqlclient
./simulator >output 2>errlog &
