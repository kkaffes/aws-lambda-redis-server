from __future__ import print_function
import subprocess

from time import sleep


def handler(event, context):
    """
    This function starts a redis server.
    """

    # FIXME Do LCP magic here

    subprocess.Popen(['./redis-server', './redis.conf'])

    # FIXME Temp sleep so that server stays up
    sleep(300)

    return "Waking up and dying :("
