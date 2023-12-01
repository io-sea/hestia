import hestia

import random

def run_sample():
    client = hestia.HestiaClient()

    object_id = str(random.randint(1000, 9999))

    # Create object with specified id
    object_json = client.object_create(object_id)[0]
    if (object_json["id"] != object_id):
        raise ValueError('Expected object id: ' + object_id + '  in Create but got ' + object_json[0]["id"])
    
    # Check that we can read it back ok
    object_read_json = client.object_read_ids([object_id])[0]
    if (object_read_json["id"] != object_id):
        raise ValueError('Expected object id: ' + object_id + '  in Read but got ' + object_read_json["id"])
    
    # Update an attribute and check that it is updated ok
    object_read_json["name"] = "my_object"
    object_updated_json = client.object_update(object_read_json)[0]
    if (object_updated_json["name"] != "my_object"):
        raise ValueError('Failed to update object name attr')
    
    # Remove the object
    client.object_remove(object_id)

    # Check that we get an empty read
    object_read_json = client.object_read_ids([object_id])
    if (len(object_read_json) > 0):
        raise ValueError('Expected no remaining object post delete - but found some')
    
    # Recreate the object
    object_json = client.object_create(object_id)[0]

    # Set some user attributes
    my_attributes = {
                    "key0" : "val0", 
                    "key1" : "val1"
                    }
    user_attrs = client.object_attrs_put(object_id, my_attributes)

    # Read the attributes back
    user_attrs_read = client.object_attrs_get(object_id)

    # Add some data
    content = b"The quick brown fox jumps over the lazy dog."
    action_id = client.object_data_put(object_id, content)

    # Read the action back to check the status
    action = client.action_read_ids([action_id])[0]
    if (action["status"] != "finished_ok"):
        raise ValueError('Put action did not complete ok')
    
    # Get the content back
    retrieved_content = client.object_data_get(object_id, len(content))
    if (retrieved_content != content):
        raise ValueError('Retrieved content does not match original')
    
    # Copy between tiers
    copied_action_id = client.object_data_copy(object_id, 0, 1)
    copied_action = client.action_read_ids([copied_action_id])[0]
    if (copied_action["status"] != "finished_ok"):
        raise ValueError('Copy action did not complete ok')

    # Move between tiers
    moved_action_id = client.object_data_move(object_id, 1, 2)
    moved_action = client.action_read_ids([moved_action_id])[0]
    if (moved_action["status"] != "finished_ok"):
        raise ValueError('Copy action did not complete ok')
    
    # Release a tier
    released_action_id = client.object_data_release(object_id, 0)
    release_action = client.action_read_ids([released_action_id])[0]
    if (release_action["status"] != "finished_ok"):
        raise ValueError('Copy action did not complete ok')

if __name__ == "__main__":
    run_sample()