#include "hestia_iosea.h"

#include <stdio.h>
#include <string.h>
#include <stddef.h>

#define ON_ERROR(print_input) \
    size_t error_buffer_size = 1024; \
    char error_buffer[error_buffer_size]; \
    hestia_get_last_error(error_buffer, error_buffer_size); \
    printf(print_input); \
    printf("\nCheck logs in the cache location below for more details.\n%s", error_buffer); \

int main(int argc, char** argv)
{
    if (argc == 2) {
        hestia_initialize(argv[1], NULL, NULL);
    }
    else {
        hestia_initialize(NULL, NULL, NULL);
    }

    // Create an object with some initial data - if running the example multiple times
    // you need to clear the cache or change subsequent 'puts' to use the HESTIA_UPDATE flag.
    HestiaId object_id;
    hestia_init_id(&object_id);
    object_id.m_lo = 1234;

    char content[] = "The quick brown fox jumps over the lazy dog";
    HestiaIoContext input_context;
    input_context.m_type   = HESTIA_IO_BUFFER;
    input_context.m_length = strlen(content) + 1;
    input_context.m_buffer = content;
    input_context.m_offset = 0;

    int tier0 = 0;
    int rc = hestia_object_put(&object_id, HESTIA_CREATE, &input_context, tier0);
    if (rc != 0)
    {
        ON_ERROR("Error in object put - if you've run the example before maybe you need to clear the cache.");
        return rc;
    }

    // Assign some metadata
    size_t num_kv_pairs = 3;
    HestiaKeyValuePair kv_pairs[num_kv_pairs];
    kv_pairs[0].m_key = "my_key0";
    kv_pairs[0].m_value = "my_val0";
    kv_pairs[1].m_key = "my_key1";
    kv_pairs[1].m_value = "my_val1";
    kv_pairs[2].m_key = "my_key2";
    kv_pairs[2].m_value = "my_val2";
    rc = hestia_object_set_attrs(&object_id, kv_pairs, num_kv_pairs);
    if (rc != 0)
    {
        ON_ERROR("Error in object set attrs.");
        return rc;
    }

    // Read the data back
    char return_buffer[strlen(content) + 1];
    HestiaIoContext output_context;
    output_context.m_type   = HESTIA_IO_BUFFER;
    output_context.m_length = strlen(content) + 1;
    output_context.m_buffer = return_buffer;
    output_context.m_offset = 0;
    rc = hestia_object_get(&object_id, &output_context, tier0);
    if (rc != 0)
    {
        ON_ERROR("Error in object get.");
        return rc;
    }

    if (strcmp(content, return_buffer) != 0)
    {
        printf("The output content doesn't match the input. Output is: %s", return_buffer);
        return -1;
    }

    // Read the object attributes - both user and system attrs
    HestiaObject object_attrs;
    rc = hestia_object_get_attrs(&object_id, &object_attrs);
    if (object_attrs.m_num_attrs != num_kv_pairs)
    {
        printf("Number of object user attributes doesn't match original inputs. Number returned: %zu", object_attrs.m_num_attrs);
        return -1;
    }
    for(size_t idx=0; idx<object_attrs.m_num_attrs; idx++)
    {
        printf("Get Attrs: key '%s' - value '%s'\n", object_attrs.m_attrs[idx].m_key, object_attrs.m_attrs[idx].m_value);
    }
    hestia_init_object(&object_attrs); // Frees and reinitializes the struct

    // Copy the object to a new tier
    int tier1 = 1;
    rc = hestia_object_copy(& object_id, tier0, tier1);
    if (rc != 0)
    {
        ON_ERROR("Error in object copy.");
        return rc;
    }

    // Move the object to another tier
    int tier2 = 2;
    rc = hestia_object_copy(& object_id, tier1, tier2);
    if (rc != 0)
    {
        ON_ERROR("Error in object move.");
        return rc;
    }

    // Remove the object from a tier
    int error_if_last_tier = 1;
    rc = hestia_object_release(& object_id, tier1, error_if_last_tier);
    if (rc != 0)
    {
        ON_ERROR("Error in object release.");
        return rc;
    }

    // List the overall available tiers
    uint8_t* tier_ids = NULL;
    size_t num_tier_ids = 0;
    rc = hestia_list_tiers(&tier_ids, &num_tier_ids);
    if (rc != 0)
    {
        ON_ERROR("Error in object list tiers.");
        return rc;
    }

    // 5 tiers are hardcoded into the hestia default config - you will need to change this
    // if using a different config.
    if (num_tier_ids != 5)
    {
        printf("Expected 5 tiers but got %zu.", num_tier_ids);
        return -1;
    }
    hestia_free_tier_ids(&tier_ids);

    // Check which tiers the object is on - we expect 2 (tier 0 and 2) after the copy - move - release sequence
    num_tier_ids = 0;
    rc = hestia_object_locate(&object_id, &tier_ids, &num_tier_ids);
    if (rc != 0)
    {
        ON_ERROR("Error in object locate.");
        return rc;
    }
    if (num_tier_ids != 2)
    {
        printf("Expected object to be on 2 tiers but got %zu.", num_tier_ids);
        return -1;
    }
    hestia_free_tier_ids(&tier_ids);

    // List the objects on tier 0 - we expect just one, our original object
    HestiaId* object_ids = NULL;
    size_t num_object_ids = 0;
    rc  = hestia_object_list(tier0, &object_ids, &num_object_ids);
    if (rc != 0)
    {
        ON_ERROR("Error in object list.");
        return rc;
    }
    if (num_object_ids != 1)
    {
        printf("Expected one object on tier 0 but got %zu.", num_object_ids);
        return -1;
    }

    if (object_ids[0].m_lo != object_id.m_lo)
    {
        printf("Expected object id %llu on tier 0 but got %llu.", object_id.m_lo, object_ids[0].m_lo);
        return -1;
    }
    hestia_free_ids(&object_ids, num_object_ids);

    printf("Example completed ok\n");
    return 0;
}
