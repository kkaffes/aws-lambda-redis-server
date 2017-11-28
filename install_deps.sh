#!/bin/bash

mkdir deps
cd deps
git clone https://github.com/kkaffes/redis
cd redis
make CFLAGS="-static" EXEEXT="-static" LDFLAGS="-I/usr/local/include/"
cp src/redis-server ../../server-lambda/
