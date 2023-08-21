import hestia

if __name__ == "__main__":

    client = hestia.HestiaClient()

    object_json = client.object_create()
    print(object_json)

    object_read_json = client.object_read()
    print(object_read_json)

    """
    content = b"The quick brown fox jumps over the lazy dog."
    client.object_put(object_id, content)

    retrieved = client.object_get(object_id, len(content))
    print(retrieved)
    """

