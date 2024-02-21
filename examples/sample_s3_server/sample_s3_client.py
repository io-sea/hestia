import boto3
from botocore.client import Config

# Add the user name and token obtained from the rest interface - described above
session = boto3.session.Session(aws_access_key_id="MY_USER_NAME", aws_secret_access_key="MY_USER_TOKEN")

# Here HESTIA_ENDPOINT is something like http://127.0.0.1:8090
# Hestia best supports path based addressing style for s3
# Payload signing is currently not supported
client = session.client(service_name='s3', 
  endpoint_url="HESTIA_ENDPOINT", 
  config=Config(s3={'addressing_style': 'path','payload_signing_enabled' : False}))

# Bucket and Dataset are interchaneable notions here
my_bucket_name = "my_bucket"
my_object_name = "my_object"
my_metadata = {"mykey0" : "myvalue0"}
my_file_to_upload = "test.dat"

# Create the bucket
client.create_bucket(Bucket=my_bucket_name) 

# Upload the file - this also creates the object and adds any metadata
client.upload_file(Filename=my_file_to_upload,
  Bucket=my_bucket_name,
  Key=my_object_name,
  ExtraArgs={"Metadata": my_metadata})

