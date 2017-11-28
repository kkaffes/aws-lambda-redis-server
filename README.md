What is this repository?
------------------------

This repository contains everything needed to build an AWS Lambda function
that starts a Redis server and a different AWS Lambda function that can
connect to the Redis server hosted in the first.

Instructions
------------

You need to build a modified statically linked version of Redis server.
This can be done easily by running:

    ./install_deps.sh

If you want to build and run the example code for port reuse, you need to have
Linux kernel version >= 4.6.

Create the .zip file needed for each AWS Lambda function by running:

    zip app.zip *
