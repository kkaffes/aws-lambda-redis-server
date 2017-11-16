from __future__ import print_function
import uuid
import redis
import subprocess

# Initialize Redis Client
r = redis.Redis(unix_socket_path='/tmp/redis.sock')


def handler(event, context):
    """
    This function starts a redis server and puts and gets an element from it.
    """

    subprocess.Popen(['./redis-server', './redis.conf'])

    # Wait for Redis to start
    while True:
        try:
            foo = r.get('dummy')
            break
        except redis.exceptions.ConnectionError:
            pass

    # Create a random UUID... this will be the sample element we add to redis.
    uuid_inserted = uuid.uuid4().hex
    # Put the UUID to the cache.
    r.set('uuid', uuid_inserted)
    # Get item (UUID) from the cache.
    uuid_obtained = r.get('uuid')
    if uuid_obtained == uuid_inserted:
        # This print should go to the CloudWatch Logs and Lambda console.
        print("Success: Fetched value %s from memcache" % (uuid_inserted))
    else:
        raise Exception("Value is not the same as we put :(."
                        " Expected %s got %s" % (uuid_inserted, uuid_obtained))

    return "Fetched value from redis: " + uuid_obtained
