import boto3
from botocore.client import Config
import uuid
import sys

class S3Client:
    def __init__(self, server_url):
        self.session = boto3.session.Session(
            aws_access_key_id="OPEN_KEY", aws_secret_access_key="SECRET_KEY")
        self.client = self.session.client(service_name='s3', endpoint_url=server_url, config=Config(s3={'addressing_style': 'path'}))

    def put(self, filename, bucket_name, key, meta_data={}):
        self.client.upload_file(Filename=filename,
                                Bucket=bucket_name,
                                Key=key,
                                ExtraArgs={"Metadata": meta_data})

    def get(self, filename, bucket_name, key):
        self.client.download_file(Filename=filename,
                                  Bucket=bucket_name,
                                  Key=key)

    def create_bucket(self, bucket_name):
        self.client.create_bucket(Bucket=bucket_name)

    def get_md(self, bucket_name, key):
        return self.client.head_object(Bucket=bucket_name, Key=key)["Metadata"]

    def list_objects(self, bucket_name):
        return self.client.list_objects_v2(Bucket=bucket_name)

    def list_buckets(self):
        return self.client.list_buckets()['Buckets']

    def delete_object(self, bucket_name, key):
        return self.client.delete_object(Bucket=bucket_name, Key=key)
    
if __name__ == "__main__":

    client = S3Client("http://127.0.0.1:8080")

    buckets = client.list_buckets()
    print("Got: " + str(len(buckets)) + " buckets")

    client.create_bucket("my_bucket")

    buckets = client.list_buckets()
    print("Got: " + str(len(buckets)) + " buckets")

    objects = client.list_objects("my_bucket")



    # client.put('test_data/EmperorWu.txt', "my_bucket", "my_obj")





