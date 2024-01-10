#pragma once

#include "ErrorUtils.h"

#include <filesystem>
#include <vector>

namespace hestia {

/**
 * @brief Convenience class for a filesystem File
 *
 * A filesystem File with some convenience wrappers for opening, closing
 * and error handling.
 */
class File {
  public:
    using Path = std::filesystem::path;

    /**
     * Constructor
     *
     * @param path Path to the file
     */
    File(const Path& path = {});

    /**
     * Destructor
     */
    ~File();

    /**
     * Close the file
     * @return Status of the close operation, reports if e.g. a stream flush fails.
     */
    OpStatus close();

    struct ReadState {
        std::size_t m_size_read{0};
        bool m_finished{false};
    };

    /**
     * Read from the file - the file is opened if needed
     * @param data buffer to be read into
     * @param length length of the buffer to read into
     * @return Status of the read operation, number of bytes read and if EOF is hit
     */
    std::pair<OpStatus, ReadState> read(char* data, std::size_t length);

    OpStatus read(std::string& buffer);

    std::pair<OpStatus, ReadState> read_lines(std::vector<std::string>& lines);

    OpStatus seek_to(std::size_t offset);

    void set_path(const Path& path);

    /**
     * Write to the file - the file is opened if needed
     * @param data buffer to write from
     * @param length length of the buffer to write from
     * @return Status of the write operation
     */
    OpStatus write(const char* data, std::size_t length);

    OpStatus write_lines(const std::vector<std::string>& lines);

    uintmax_t get_size() const;

  private:
    OpStatus open_for_read() noexcept;
    OpStatus open_for_write() noexcept;

    Path m_path;
    int m_fd{-1};
};
}  // namespace hestia