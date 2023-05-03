import requests

#r = requests.put('http://localhost:8000/', data={'key': 'value'})
#print(r.status_code)
#print(r.headers)
#print(r.text)

class S3Client(object):

    def __init__(self, endpoint):
        self.endpoint = endpoint

    def list_buckets(self):
        r = requests.get(self.endpoint)
        self.print_response(r)

    def get_bucket(self, bucket_name):
        r = requests.get(self.endpoint + bucket_name)
        self.print_response(r)

    def get_object(self, bucket_name, object_name):
        r = requests.get(self.endpoint + bucket_name + "/" + object_name)
        self.print_response(r)

    def put_object(self, bucket_name, object_name, data=None):
        if data:
            r = requests.put(self.endpoint + bucket_name + "/" + object_name, data=data)
        else:
            r = requests.put(self.endpoint + bucket_name + "/" + object_name)
        self.print_response(r)

    def put_bucket(self, bucket_name):
        r = requests.put(self.endpoint + bucket_name)
        self.print_response(r)

    def delete_bucket(self, bucket_name):
        r = requests.delete(self.endpoint + bucket_name)
        self.print_response(r)        

    def print_response(self, r):
        print(r.status_code)
        print(r.headers)
        print(r.text)    

if __name__ == "__main__":

    s3_client = S3Client('http://localhost:8000/')

    #s3_client.list_buckets()

    #s3_client.get_bucket("mybucket")

    #s3_client.put_bucket("mybucket")

    #s3_client.get_bucket("mybucket")

    #s3_client.put_object("mybucket", "myobject", {'key': 'value'})

    s3_client.get_object("mybucket", "myobject")


