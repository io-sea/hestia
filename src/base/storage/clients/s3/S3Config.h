#pragma once

#include "EnumUtils.h"
#include "SerializeableWithFields.h"

namespace hestia {
class S3Config : public SerializeableWithFields {

  public:
    STRINGABLE_ENUM(UriStyle, PATH, VIRTUAL)

    S3Config();

    const std::string& get_user_agent() const;

    const std::string& get_default_host() const;

    const std::string& get_access_key_id() const;

    std::string get_secret_access_key() const;

    const std::string& get_metadata_prefix() const
    {
        return m_metadataprefix.get_value();
    }

    const std::string& get_default_bucket_name() const
    {
        return m_default_bucket_name.get_value();
    }

    bool is_path_uri_style() const
    {
        return m_uri_style.get_value() == UriStyle::PATH;
    }

  private:
    void init();

    /**
     * This is appended to all metadata sent to the backend and stripped back
     *off when reading back in.
     **/
    StringField m_metadataprefix{"metadata_prefix"};

    /**
     * Can add a user agent string for dedicated backend logic
     **/
    StringField m_user_agent{"user_agent"};

    /**
     * Uses this instead of the hestia username when talking to the backend
     **/
    StringField m_s3_access_key_id{"s3_access_key_id"};

    /**
     * Specify an environment variable containing the secret access key. If set
     * it will be used instead of the user's hestia token
     **/
    StringField m_s3_secret_access_key_var{"m_s3_secret_access_key_var"};

    /**
     * Unless overriden in a specific request use this host
     **/
    StringField m_default_host{"default_host", "127.0.0.1:8000"};

    /**
     * If an object has no bucket set use this one
     **/
    StringField m_default_bucket_name{"default_bucket_name", "default"};

    /**
     * Whether to use PATH or Virtual Host style uris
     **/
    EnumField<UriStyle, UriStyle_enum_string_converter> m_uri_style{
        "uri_style", UriStyle::PATH};
};
}  // namespace hestia
