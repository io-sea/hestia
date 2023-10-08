#include "S3Bucket.h"

#include "XmlElement.h"

namespace hestia {

S3Bucket::S3Bucket(const std::string& name, const S3Timestamp& creation_date) :
    m_name(name), m_creation_date(creation_date)
{
}

S3Bucket::S3Bucket(const XmlElement& element)
{
    for (const auto& child : element.get_children()) {
        if (child->get_tag_name() == "CreationDate") {
            m_creation_date = child->get_text();
        }
        else if (child->get_tag_name() == "Name") {
            m_name = child->get_text();
        }
    }
}

XmlElementPtr S3Bucket::to_xml() const
{
    auto bucket_element = XmlElement::create("Bucket");

    auto creation_date_element = XmlElement::create("CreationDate");
    creation_date_element->set_text(m_creation_date.m_value);
    bucket_element->add_child(std::move(creation_date_element));

    auto name_element = XmlElement::create("Name");
    name_element->set_text(m_name);
    bucket_element->add_child(std::move(name_element));

    return bucket_element;
}

// https://docs.aws.amazon.com/AmazonS3/latest/userguide/bucketnamingrules.html
S3Status S3Bucket::validate_name(
    const std::string& tracking_id, const S3UserContext& user_context) const
{
    std::size_t max_length = 63;

    std::size_t length{0};
    std::string error_message;
    bool has_dot{false};
    bool has_non_digit{false};
    char last_char = 0;

    for (const auto c : m_name) {
        if (length == max_length) {
            error_message = "Bucket Name is Too Long. Max is :"
                            + std::to_string(max_length);
            break;
        }

        if (std::isalpha(c) != 0) {
            if (std::isupper(c) != 0) {
                error_message = "Bucket names must be lower case only.";
                break;
            }
            has_non_digit = true;
        }
        else if (std::isdigit(c) != 0) {
            // pass
        }
        else if (c == '_') {
            if (length == 0) {
                error_message = "Bucket names must start with alnum character";
                break;
            }
            has_non_digit = true;
        }
        else if (c == '-') {
            if (length == 0) {
                error_message = "Bucket names must start with alnum character";
                break;
            }

            if (last_char == '.') {
                error_message =
                    "Invalid character combination '.-' in bucket name.";
                break;
            }
            has_non_digit = true;
        }
        else if (c == '.') {
            if (length == 0) {
                error_message = "Bucket names must start with alnum character";
                break;
            }

            if (last_char == '.') {
                error_message = "Bucket names must not have two sucessive '.'";
                break;
            }
            has_dot = true;
        }
        else {
            error_message =
                "Invalid character in bucket name: " + std::to_string(c);
            break;
        }
        last_char = c;
        length++;
    }

    if (length < 3) {
        error_message = "Bucket name too short - minimum length is 3.";
    }

    if (has_dot && !has_non_digit) {
        error_message = "Bucket name seems 'IP Style' but has non-digits.";
    }

    if (error_message.empty()) {
        return {};
    }
    else {
        S3Request s3_request(user_context);
        s3_request.m_path.m_bucket_name = m_name;
        s3_request.m_tracking_id        = tracking_id;
        return {
            S3StatusCode::_400_INVALID_BUCKET_NAME, s3_request, error_message};
    }
}

const std::string& S3Bucket::get_location_constraint() const
{
    return m_location_constraint;
}

void S3Bucket::add_location_constraint(XmlElement& element) const
{
    if (!m_location_constraint.empty()) {
        auto constraint_element = XmlElement::create("LocationContrains");
        constraint_element->set_text(m_location_constraint);
        element.add_child(std::move(constraint_element));
    }
}
}  // namespace hestia