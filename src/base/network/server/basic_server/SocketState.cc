#include "SocketState.h"

bool SocketState::ok() const
{
    return (m_connect_status != ConnectStatus::FAILED)
           && (m_bind_status != BindStatus::FAILED);
}

bool SocketState::bound() const
{
    return m_bind_status == BindStatus::OK;
}

bool SocketState::connected() const
{
    return m_connect_status == ConnectStatus::OK;
}

void SocketState::on_connect_error(int code, const std::string& msg)
{
    m_connect_status = ConnectStatus::FAILED;
    m_error_code     = code;
    m_error_message  = msg;
}

void SocketState::on_bind_error(int code, const std::string& msg)
{
    m_bind_status   = BindStatus::FAILED;
    m_error_code    = code;
    m_error_message = msg;
}

void SocketState::set_disconnected()
{
    m_connect_status = ConnectStatus::UNSET;
    m_bind_status    = BindStatus::UNSET;
}

void SocketState::set_connected()
{
    m_connect_status = ConnectStatus::OK;
}

void SocketState::set_bound()
{
    m_bind_status = BindStatus::OK;
}