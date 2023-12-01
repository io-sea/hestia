#include "hestia.h"

#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>

struct ScopedBuffer
{
    ~ScopedBuffer()
    {
        reset();
    }

    void reset()
    {
        if (m_content != nullptr)
        {
            hestia_free_output(&m_content);
            m_content = nullptr;
        }
        m_length = 0;
    }

    char* m_content{nullptr};
    int m_length{0};
};

int main(int argc, char** argv)
{
    if (argc == 2) {
        hestia_initialize(argv[1], nullptr, nullptr);
    }
    else {
        hestia_initialize(nullptr, nullptr, nullptr);
    }

    ScopedBuffer object_id;
    auto rc = hestia_create(HESTIA_OBJECT, HESTIA_IO_NONE, HESTIA_ID_NONE, nullptr, 0, HESTIA_IO_IDS, 
        &object_id.m_content, &object_id.m_length);
    if (rc != 0)
    {
        std::cerr << "Failed to create object." << std::endl;
        return rc;
    }
    std::cout << "Created object with id: " << object_id.m_content << std::endl;

    // Assign USER attributes
    std::unordered_map<std::string, std::string> user_attrs = {{"my_key0", "my_val0"}, {"my_key1","my_val1"}};
    std::string input_buffer(object_id.m_content);
    input_buffer += "\n\n";
    for(const auto& [key, value] : user_attrs)
    {
        input_buffer += "data." + key + "=" + value + "\n";
    }

    ScopedBuffer output_buffer;
    rc = hestia_update(HESTIA_USER_METADATA, static_cast<hestia_io_format_e>(HESTIA_IO_IDS | HESTIA_IO_KEY_VALUE), HESTIA_PARENT_ID, 
        input_buffer.data(), input_buffer.size(), HESTIA_IO_NONE, &output_buffer.m_content, &output_buffer.m_length);
    if (rc != 0)
    {
        std::cerr << "Failed to update object." << std::endl;
        return rc;
    }
    output_buffer.reset();

    // Add data
    std::string content = "The quick brown fox jumps over the lazy dog";
    rc = hestia_data_put(object_id.m_content, content.data(), content.size(), 
        0, 0, &output_buffer.m_content, &output_buffer.m_length);
    if (rc != 0)
    {
        std::cerr << "Failed to put data." << std::endl;
        return rc;
    }
    std::cout << "Put data with activity id" << output_buffer.m_content << std::endl;
    output_buffer.reset();

    // Move data to different tier
    rc = hestia_data_move(HESTIA_OBJECT, object_id.m_content, 0, 1, &output_buffer.m_content, &output_buffer.m_length);
    if (rc != 0)
    {
        std::cerr << "Failed to move data." << std::endl;
        return rc;
    }
    std::cout << "Moved data between tiers with activity id" << output_buffer.m_content << std::endl;
    output_buffer.reset();

    // Get the data back
    std::string return_buffer;
    return_buffer.resize(content.size());
    std::size_t buffer_size = content.size();
    rc = hestia_data_get(object_id.m_content, return_buffer.data(), &buffer_size, 0, 1, 
        &output_buffer.m_content, &output_buffer.m_length);
    if (rc != 0)
    {
        std::cerr << "Failed to get data." << std::endl;
        return rc;
    }
    std::cout << "Got data: " << return_buffer << std::endl;
    output_buffer.reset();

    // Get the object SYSTEM attributes
    int total_count = 0;
    rc = hestia_read(HESTIA_OBJECT, HESTIA_QUERY_IDS, HESTIA_ID, 0, 0, object_id.m_content, object_id.m_length, 
        HESTIA_IO_JSON, &output_buffer.m_content, &output_buffer.m_length, &total_count);
    if (rc != 0)
    {
        std::cerr << "Failed to get object system attributes." << std::endl;
        return rc;
    }
    std::cout << "Got object attributes: " << output_buffer.m_content << std::endl;
    output_buffer.reset();

    // Get the object USER attributes
    total_count = 0;
    rc = hestia_read(HESTIA_USER_METADATA, HESTIA_QUERY_IDS, HESTIA_PARENT_ID, 0, 0, object_id.m_content, object_id.m_length, 
        HESTIA_IO_JSON, &output_buffer.m_content, &output_buffer.m_length, &total_count);
    if (rc != 0)
    {
        std::cerr << "Failed to get object system attributes." << std::endl;
        return rc;
    }
    std::cout << "Got object user attributes: " << output_buffer.m_content << std::endl;

    hestia_finish();
    return 0;
}

