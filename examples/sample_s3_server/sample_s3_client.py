import os
from pathlib import Path
import random
import string

import boto3
from botocore.client import Config

# Real AWS servers have a non-standard behaviour for the Http Expect header.
# The Boto library follows this behviouar - so in theory will only run correctly against AWS servers.
# Hestia (and Minio) follow Http standard behaviour so will hang without disabling this boto behaviour
# as below.

def _boto_patch_headers(request, **kwargs):
    # https://github.com/boto/boto/issues/840
    request.headers.pop("Expect", None)

def test_sample_s3_client():
    
    print("test_sample_s3_client: Starting")

    # In the example config auth is disabled in Hestia so the secret key can be anything.
    # If you want to enable auth you need to 'register/login' to the Hestia server http endpoint
    # to get a token.
    user_id = "hestia_default_user"
    user_secret_key = "MY_USER_KEY"
    hestia_server_url = "http://127.0.0.1:8090"

    # Add the user name and token obtained from the rest interface - described above
    session = boto3.session.Session(aws_access_key_id=user_id, 
                                    aws_secret_access_key=user_secret_key)

    # Hestia best supports path based addressing style for s3
    # You can enable payload signing once you are happy the demo is working for you
    client = session.client(service_name='s3', 
                            endpoint_url=hestia_server_url, 
                            config=Config(s3={'addressing_style': 'path',
                                              'payload_signing_enabled' : False}))
    client.meta.events.register("before-send.s3.*", _boto_patch_headers)

    # Get the current bucket count
    num_buckets = len(client.list_buckets()['Buckets'])

    # Create the bucket - Bucket and Dataset are interchangeable notions here
    bucket_name = f"bucket_{num_buckets}"
    client.create_bucket(Bucket=bucket_name) 

    # Create some objects and add some metadata
    num_objects = 3
    num_metadata_values = 3
    payload_size = 500 # bytes
    payload_file = "payload.tmp"

    for idx in range(num_objects):
        object_key = f"object_{idx}"

        # Prepare the metadata {key_0 = value_0, ...}
        object_metadata = {f"key_{jdx}": f"value_{jdx}" for jdx in range(num_metadata_values) }

        # Prepare the payload - k random characters
        payload = ''.join(random.choices(string.ascii_uppercase, k=payload_size))
        with open(payload_file, 'w') as f:
            f.write(payload)

        # Upload the file - this also creates the object and adds any metadata
        client.upload_file(Filename=payload_file, 
                           Bucket=bucket_name,
                           Key=object_key,
                           ExtraArgs={"Metadata": object_metadata})
        
    if os.path.exists(payload_file):
        os.remove(payload_file)

    # Check the number of objects is as expected
    num_objects_created = len(client.list_objects_v2(Bucket=bucket_name)["Contents"])
    assert num_objects_created == num_objects

    if num_objects_created > 0:
        # Check that the returned content is the same as the uploaded for the last object
        returned_file = Path(payload_file).stem + "_returned.dat"
        client.download_file(Filename=returned_file, Bucket=bucket_name, Key=object_key)

        with open(returned_file, 'r') as f:
            returned_payload = f.read()
        os.remove(returned_file)
        assert returned_payload == payload

        response = client.head_object(Bucket=bucket_name, Key=object_key)
        assert response["Metadata"]['key_0'] == "value_0"

    print("test_sample_s3_client: Everything is Ok 👍")
  
if __name__ == "__main__":
    test_sample_s3_client()
