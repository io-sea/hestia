import hestia

if __name__ == "__main__":

    client = hestia.HestiaClient()

    object_id = client.object_create()
    print(object_id)

    content = b"The quick brown fox jumps over the lazy dog."
    client.object_put(object_id, content)

    retrieved = client.object_get(object_id, len(content))
    print(retrieved)

