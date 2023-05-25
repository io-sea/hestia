# Copyright (c) 2020, Irish Centre for High End Computing (ICHEC), NUI Galway
# Authors:
#     Ciarán O'Rourke <ciaran.orourke@ichec.ie>,
#     Sophie Wenzel-Teuber <sophie.wenzel-teuber@ichec.ie>
#
# This source code is licensed under the MIT license found in the
# LICENSE file in the root directory of this source tree.

import unittest

import subprocess
import os
from multiprocessing import Process

import utils.s3_client as s3_client
import utils.compare as compare
import utils.random_string as random_string


class Server_test_case(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.bucket_name = random_string.random_string('ichec.phobos.', 12)
        cls.client = s3_client.S3_client('http://localhost:11000/')

        # start the server daemon
        subprocess.run("../../deimos start localhost", shell=True)

        cls.put_file_name = 'data/EmperorWu.txt'
        cls.get_file_name = 'data/EmperorWu_get.txt'

        cls.put_empty_file_name = 'data/empty.txt'
        cls.get_empty_file_name = 'data/empty_get.txt'
        cls.key = random_string.random_string('key_', 12)
        cls.key2 = random_string.random_string('key_', 12)

    @classmethod
    def tearDownClass(cls):
        # stop the server daemon
        subprocess.run("../../deimos stop", shell=True)

        if os.path.isfile(cls.get_file_name):
            os.remove(cls.get_file_name)
        if os.path.isfile(cls.get_empty_file_name):
            os.remove(cls.get_empty_file_name)

    def test_1_file_put_and_get(self):
        self.client.put(self.put_file_name, self.bucket_name, self.key)
        self.client.get(self.get_file_name, self.bucket_name, self.key)
        self.assertTrue(
            compare.files_are_equal(self.put_file_name, self.get_file_name))

    def test_2_empty_file_put_and_get(self):
        key = random_string.random_string('key_', 12)

        self.client.put(self.put_empty_file_name, self.bucket_name, key)
        self.client.get(self.get_empty_file_name, self.bucket_name, key)
        self.assertTrue(
            compare.files_are_equal(self.put_empty_file_name,
                                    self.get_empty_file_name))

    @unittest.expectedFailure
    def test_3_put_with_used_key(self):
        self.assertTrue(
            self.client.put(self.put_file_name, self.bucket_name, self.key))

    @unittest.expectedFailure
    def test_4_get_with_bad_key(self):
        self.assertTrue(
            self.client.get(self.get_file_name, self.bucket_name, 'bad_key'))

    def test_5_get_meta_data(self):
        key = random_string.random_string('key_', 12)
        put_meta_data = random_string.random_map(5, 8)

        self.client.put(self.put_file_name,
                        self.bucket_name,
                        key,
                        meta_data=put_meta_data)

        get_meta_data = self.client.get_md(self.bucket_name, key)

        self.assertEqual(put_meta_data, get_meta_data)

    def test_6_list_objects(self):
        object_list = self.client.list_objects(self.bucket_name)

        self.assertEqual(object_list['Name'], self.bucket_name)

        # The bucket should contain three objects up to this point
        self.assertEqual(object_list['KeyCount'], 3)

        num_objects = 0
        for obj in object_list['Contents']:
            if (obj['Key'] == self.key):
                num_objects += 1
                self.assertEqual(obj['Size'],
                                 os.path.getsize(self.put_file_name))

        self.assertEqual(num_objects, 1)

    def test_7_create_and_list_buckets(self):
        bucket_name = random_string.random_string('bucket_', 9)
        self.client.create_bucket(bucket_name)

        bucket_list = self.client.list_buckets()
        # One bucket is created during the previous tests, but if the tests
        # have been run before then there are others
        # We will just check that our buckets exists
        match = 0
        for bucket in bucket_list:
            if (bucket['Name'] == self.bucket_name
                    or bucket['Name'] == bucket_name):
                match += 1

        self.assertEqual(match, 2)

    @unittest.expectedFailure
    def test_8_create_existing_bucket(self):
        self.assertTrue(self.client.create_bucket(self.bucket_name))

    def test_9_empty_bucket_list_objects(self):
        object_list = self.client.list_objects("bad_bucket_name")

        self.assertEqual(object_list['Name'], "bad_bucket_name")
        self.assertEqual(object_list['KeyCount'], 0)

    def test_10_delete_object(self):
        self.client.put(self.put_file_name, self.bucket_name, self.key2)
        self.assertTrue(self.client.delete_object(self.bucket_name, self.key2))
        try:
            self.client.get_md(self.bucket_name, self.key2)
        except:
            pass
            return

        self.fail("Deleted object still exists")

    def client_process(self):
        key = random_string.random_string('key_', 12)
        get_file_name = random_string.random_string('data/', 11)

        self.client.put(self.put_file_name, self.bucket_name, key)
        self.client.get(get_file_name, self.bucket_name, key)
        get_meta_data = self.client.get_md(self.bucket_name, key)
        self.assertTrue(
            compare.files_are_equal(self.put_file_name, get_file_name))

        # clean up files
        os.remove(get_file_name)

        return 0

    def test_8_stress(self):
        processes = []
        for i in range(10):
            process = Process(target=self.client_process())
            processes.append(process)
            process.start()

        for process in processes:
            process.join()
