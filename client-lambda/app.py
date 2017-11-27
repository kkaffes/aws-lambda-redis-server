from __future__ import print_function
import uuid
import redis

# Initialize Redis Client
# FIXME Add redis server id here when known
r = redis.Redis()


def handler(event, context):
    """
    This function connects to a redis server and puts and gets an
    element from it.
    """

    # Create a random UUID... this will be the sample element we add to redis.
    uuid_inserted = uuid.uuid4().hex
    # Put the UUID to the cache.
    r.set('uuid', uuid_inserted)
    # Get item (UUID) from the cache.
    uuid_obtained = r.get('uuid')
    if uuid_obtained == uuid_inserted:
        # This print should go to the CloudWatch Logs and Lambda console.
        print("Success: Fetched value %s from redis" % (uuid_inserted))
    else:
        raise Exception("Value is not the same as we put :(."
                        " Expected %s got %s" % (uuid_inserted, uuid_obtained))

    return "Fetched value from redis: " + uuid_obtained
