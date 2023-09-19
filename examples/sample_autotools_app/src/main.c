#include "hestia.h"

#include <stdio.h>
#include <string.h>
#include <stddef.h>

int main(int argc, char** argv)
{
    if (argc == 2) {
        hestia_initialize(argv[1], NULL, NULL);
    }
    else {
        hestia_initialize(NULL, NULL, NULL);
    }

    // Create an object
    char* object_id = NULL;
    int len_object_id = 0;
    int rc = hestia_create(HESTIA_OBJECT, HESTIA_IO_NONE, HESTIA_ID_NONE, NULL, 0, HESTIA_IO_IDS, &object_id, &len_object_id);
    if (rc != 0)
    {
        fprintf(stderr, "Failed to create object.\n");
        return rc;
    }
    printf("Created object with id: %s\n", object_id);

    // Assign USER attributes
    char attrs[] = "\n\ndata.key0,value0\ndata.key1,value1";
    int len_attrs = strlen(attrs);
    char input_buffer[2048] = {0};
    strncpy(input_buffer, object_id, len_object_id);
    strncpy(input_buffer + len_object_id, attrs, len_attrs);
    
    char* output_buffer = NULL;
    int len_output_buffer = 0;
    rc = hestia_update(HESTIA_USER_METADATA, HESTIA_IO_IDS | HESTIA_IO_KEY_VALUE, HESTIA_PARENT_ID, 
        input_buffer, len_object_id + len_attrs, HESTIA_IO_NONE, &output_buffer, &len_output_buffer);
    if (rc != 0)
    {
        fprintf(stderr, "Failed to update object.\n");
        return rc;
    }
    hestia_free_output(&output_buffer);

    // Add data
    char content[] = "The quick brown fox jumps over the lazy dog";
    rc = hestia_data_put(object_id, &content, strlen(content), 0, 0, &output_buffer, &len_output_buffer);
    if (rc != 0)
    {
        fprintf(stderr, "Failed to put data.\n");
        return rc;
    }
    printf("Put data with activity id: %s\n", output_buffer);
    hestia_free_output(&output_buffer);

    // Move data to different tier
    rc = hestia_data_move(HESTIA_OBJECT, object_id, 0, 1, &output_buffer, &len_output_buffer);
    if (rc != 0)
    {
        fprintf(stderr, "Failed to move data.\n");
        return rc;
    }
    printf("Moved data between tiers with activity id: %s\n", output_buffer);
    hestia_free_output(&output_buffer);

    // Get the data back from tier 1
    char return_buffer[200] = {0};
    size_t buffer_size = strlen(content);
    rc = hestia_data_get(object_id, &return_buffer, &buffer_size, 0, 1, &output_buffer, &len_output_buffer);
    if (rc != 0)
    {
        fprintf(stderr, "Failed to get data.\n");
        return rc;
    }
    printf("Got data: %s\n", return_buffer);
    hestia_free_output(&output_buffer);

    // Get the object SYSTEM attributes
    int total_count = 0;
    rc = hestia_read(HESTIA_OBJECT, HESTIA_QUERY_IDS, HESTIA_ID, 0, 0, object_id, len_object_id, 
        HESTIA_IO_JSON, &output_buffer, &len_output_buffer, &total_count);
    if (rc != 0)
    {
        fprintf(stderr, "Failed to get object system attributes.\n");
        return rc;
    }
    printf("Got object attributes: %s\n", output_buffer);
    hestia_free_output(&output_buffer);

    hestia_free_output(&object_id);
    hestia_finish();

    return 0;
}

