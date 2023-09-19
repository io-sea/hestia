class CurlCliClient(object):

    def __init__(self, endpoint) -> None:
        self.endpoint = endpoint 

    def process_headers(self, headers):
        header_str = ""
        if not headers:
            return header_str

        for key, value in headers.items():
            header_str += f'-H "{key}: {value}" '
        return header_str
    
    def get_cmd(self, headers, path):
        header_str = self.process_headers(headers)
        url_str = self.endpoint + path
        return f'curl {header_str} {url_str}'
    
    def post_cmd(self, path, body):
        body_str = f"-d '{body}'"
        url_str = self.endpoint + path
        return f'curl {body_str} -X POST {url_str}'
    
    def put_cmd(self, headers, path, body = None, file = None):
        header_str = self.process_headers(headers)
        url_str = self.endpoint + path

        body_str = ""
        if body is not None:
            body_str = f"-d '{body}'"

        file_str = ""
        if file is not None:
            file_str = f'--upload-file "{file}"'

        return f'curl {header_str} {body_str} {file_str} -X PUT {url_str}'