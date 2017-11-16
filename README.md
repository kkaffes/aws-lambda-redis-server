What is this repository?
------------------------

This repository contains everything needed to build an AWS Lambda function
that starts a local Redis server listening to a Unix Domain Socket and tests
whether it is working by putting and a getting a value.

Instructions
------------

The only thing needed is to build a statically linked version of Redis server
and store it as redis-server in the root of this repository.

Download Redis from Github, branch 4.0, and build it using:

    make CFLAGS="-static" EXEEXT="-static" LDFLAGS="-I/usr/local/include/"

Create the .zip file needed by AWS Lambda by running:

    zip app.zip *
