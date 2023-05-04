#pragma once

#include <string>

namespace hestia {

/**
 * @brief Generic 'Operation Status' for return value error handling
 *
 * This class provides a general 'return status' struct which can be used
 * as a return type when a 'status' is needed.
 */
struct OpStatus {
    enum class Status { OK, ERROR };

    OpStatus() = default;

    /**
     * Constructor
     *
     * @param status Is the Operation in 'OK' or 'ERROR' state
     * @param error_code A numeric error code if there is one
     * @param error_message A string error message if available
     */
    OpStatus(
        Status status,
        int error_code                   = 0,
        const std::string& error_message = {});

    /**
     * Return the error code as a string
     *
     * @return The error code as a string
     */
    std::string code() const;

    /**
     * Return the error message, can be empty
     *
     * @return The error message
     */
    const std::string& message() const;

    /**
     * True if we are not in an error state
     *
     * @return True if we are not in an error state
     */
    bool ok() const;

    /**
     * A string representation of the status - intended for debugging
     *
     * @return A string representation of the status
     */
    std::string str() const;

    Status m_status{Status::OK};
    int m_error_code{0};
    std::string m_error_message;
};
}  // namespace hestia