import requests

class HttpClient(object):

    def __init__(self, endpoint):
        self.endpoint = endpoint

    def get(self, url):
        r = requests.get(self.endpoint + url)
        self.print_response(r)

    def put(self, url, data):
        r = requests.put(self.endpoint + url, data)
        self.print_response(r)
    
    def print_response(self, r):
        print(r.status_code)
        print(r.headers)
        print(r.text)    

if __name__ == "__main__":

    http_client = HttpClient('http://localhost:8000/')

    http_client.put("/", {'key': 'value'})

    http_client.get("/")




