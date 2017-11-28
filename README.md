What is this repository?
------------------------

This repository contains everything needed to build an AWS Lambda function
that starts a Redis server and a different AWS Lambda function that can
connect to the Redis server hosted in the first.

Instructions
------------

The only thing needed is to build a modified statically linked version of
Redis server. This can be done easily by running:

    ./install_deps.sh

Create the .zip file needed for each AWS Lambda function by running:

    zip app.zip *
