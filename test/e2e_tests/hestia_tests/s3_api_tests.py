import os
from pathlib import Path
import logging
import shutil
import filecmp

import hestia_tests.utils
from hestia_tests.fixtures.server_test_fixture import ServerTestFixture
from hestia_tests.clients.s3_client import S3Client

class S3ApiTestFixture(ServerTestFixture):

    def __init__(self, project_dir: Path, work_dir: Path, system_install: bool):
        super().__init__(project_dir, work_dir, system_install)

        self.name = "s3_api_tests"
        self.port = 8090

        self.setup()

    def get_token(self):
        self.start_server()
        self.port = 8080
        self.login()
        self.port = 8090
        self.stop_server()

    def run(self):
        logging.info("Running S3 tests")

        object_content = self.get_test_data("EmperorWu.txt")
        returned_object = self.runtime_path / "returned_object.dat"

        # Start the regular rest server and get an auth token - then fire up the s3 server
        self.get_token()

        self.start_server("s3_server", False)

        endpoint = f"http://{self.host}:{self.port}"
        logging.info("Hitting endpoint: " + endpoint + " with token " + self.token + " and file " + str(object_content))
        s3_client = S3Client(endpoint, self.user_name, self.token)

        buckets = s3_client.list_buckets()
        logging.info("Got: " + str(len(buckets)) + " buckets")

        bucket_name = "my_bucket"
        s3_client.create_bucket(bucket_name)

        object_name = "my_object"
        s3_client.put(object_content, bucket_name, object_name)

        objects = s3_client.list_objects(bucket_name)
        logging.info("Got: " + str(len(objects)) + "objects")

        s3_client.get(returned_object, bucket_name, object_name)

        logging.info("Finished S3 tests")

    
if __name__ == "__main__":

    work_dir = Path(os.getcwd())
    project_dir = work_dir.parent

    hestia_tests.utils.setup_default_logging(work_dir / 's3_api_tests.log')

    test_fixture = S3ApiTestFixture(project_dir, work_dir, False)
    test_fixture.run()