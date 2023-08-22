import hestia
import os

def test_in_memory_buffer():
    client = hestia.HestiaClient()

    object_json = client.object_create()
    object_id = object_json[0]["id"]
    print("Created object: ", object_id)

    object_read_json = client.object_read()

    print(object_read_json)

    """
    content = b"The quick brown fox jumps over the lazy dog."
    activity_id = client.object_put(object_id, content)

    retrieved = client.object_get(object_id, len(content))
    print(retrieved)
    """

def test_file_descriptor_source(client):
    print("Into fd source")
    

    object_json = client.object_create()
    object_id = object_json[0]["id"]
    print("Created object: ", object_id)

    object_read_json = client.object_read()

    file_size = os.path.getsize('Makefile')

    with open("Makefile", 'r') as f:
        activity = client.object_put_fd(object_id, f.fileno(), file_size)
        print(activity)

    retrieved = client.object_get(object_id, file_size)
    print(len(retrieved), file_size)

if __name__ == "__main__":
    client = hestia.HestiaClient()

    #test_in_memory_buffer()

    for idx in range(100):
        test_file_descriptor_source(client)


