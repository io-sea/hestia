#include "MockHestiaClient.h"

#include "InMemoryStreamSink.h"
#include "InMemoryStreamSource.h"

namespace hestia::mock {
MockHestiaClient::MockHestiaClient() {}

OpStatus MockHestiaClient::initialize(
    const std::string& config_path,
    const std::string& user_token,
    const Dictionary& extra_config)
{
    (void)config_path;
    (void)user_token;
    (void)extra_config;
    return {};
}

OpStatus MockHestiaClient::create(
    const HestiaType& subject,
    VecCrudIdentifier& ids,
    CrudAttributes& attributes,
    CrudAttributes::Format output_format)
{
    if (ids.empty()) {
        const auto next_id = "mock_id_" + std::to_string(m_created_id_count);
        m_created_id_count++;
        m_created_ids.push_back(next_id);

        CrudIdentifier id(next_id);
        ids.push_back(id);
    }
    else {
        for (const auto& id : ids) {
            m_created_ids.push_back(id.get_primary_key());
        }
    }

    if (attributes.buffer().empty()) {
        if (output_format == CrudAttributes::Format::JSON) {
            attributes.set_buffer("my_attr_body");
        }
        else {
            attributes.set_buffer("my_attr_key, my_attr_value");
        }
    }
    (void)subject;
    return {};
}

OpStatus MockHestiaClient::update(
    const HestiaType& subject,
    const VecCrudIdentifier& ids,
    CrudAttributes& attributes,
    CrudAttributes::Format output_format)
{
    (void)subject;
    (void)ids;
    (void)attributes;
    (void)output_format;
    return {};
}

OpStatus MockHestiaClient::read(const HestiaType& subject, CrudQuery& query)
{
    if (query.get_output_format() == CrudQuery::OutputFormat::ID) {
        VecCrudIdentifier ids;
        for (const auto& id : m_created_ids) {
            ids.push_back(CrudIdentifier(id));
        }
        query.set_ids(ids);
    }
    else {
        query.attributes().set_buffer("query_output");
    }

    (void)subject;
    return {};
}

OpStatus MockHestiaClient::remove(
    const HestiaType& subject, const VecCrudIdentifier& ids)
{
    (void)subject;
    (void)ids;
    return {};
}

std::string MockHestiaClient::get_runtime_info() const
{
    return {};
}

OpStatus MockHestiaClient::do_data_movement_action(HsmAction& action)
{
    (void)action;
    return {};
}

void MockHestiaClient::do_data_io_action(
    HsmAction& action, Stream* stream, dataIoCompletionFunc completion_func)
{
    if (action.get_action() == HsmAction::Action::PUT_DATA) {
        auto sink_func = [this, action](
                             const ReadableBufferView& buffer,
                             std::size_t) -> std::pair<bool, std::size_t> {
            m_buffer[action.get_target_tier()] = buffer.data();
            return {true, buffer.length()};
        };
        stream->set_sink(InMemoryStreamSink::create(sink_func));
    }
    else if (action.get_action() == HsmAction::Action::GET_DATA) {
        auto source_func = [this, action](
                               WriteableBufferView& buffer,
                               std::size_t) -> std::pair<bool, std::size_t> {
            auto& working_buffer = m_buffer[action.get_source_tier()];

            for (std::size_t idx = 0; idx < buffer.length(); idx++) {
                if (idx >= working_buffer.length()) {
                    return {true, idx};
                }
                buffer.data()[idx] = working_buffer[idx];
            }
            return {true, working_buffer.length()};
        };
        stream->set_source(InMemoryStreamSource::create(source_func));
    }
    auto stream_complete = [completion_func, action](StreamState) {
        completion_func({}, action);
    };
    stream->set_completion_func(stream_complete);
}

void MockHestiaClient::get_last_error(std::string& error)
{
    (void)error;
}

void MockHestiaClient::set_last_error(const std::string& msg)
{
    (void)msg;
}

OpStatus MockHestiaClient::run()
{
    return {};
}
}  // namespace hestia::mock