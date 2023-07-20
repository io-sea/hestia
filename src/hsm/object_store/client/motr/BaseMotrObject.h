/* A base class that the MotrObject and MockMotrObject classes can both derive from*/
#include <filesystem>
#include "BlockStore.h"
#include "File.h"
#include "UuidUtils.h"
#include "JsonUtils.h"
#include "Logger.h"
#include "InMemoryStreamSink.h"
#include "InMemoryStreamSource.h"
#include "IOContext.h"

namespace hestia{
class BaseMotrObject {
  public:
    BaseMotrObject(const Uuid& id, bool close_on_delete = true);

    virtual ~BaseMotrObject();

    int read_blocks();

    int read(WriteableBufferView& buffer, std::size_t length);

    void set_block_layout(std::size_t transfer_size);

    std::size_t get_last_transfer_size() const;

    void save_size();

    void load_size();

    virtual void set_handle_size();
    virtual void set_motr_id();
    virtual void close_entity();
    virtual int IoCtxt_read_blocks();
    virtual void set_IoContext(int num_blocks, size_t block_size, bool alloc_io_buff);

    Uuid m_uuid;
    bool m_close_on_delete{true};

    // Io helpers
    std::size_t m_total_size{0};
    int m_unread_block_count{0};
    std::size_t m_block_size{0};
    //std::size_t m_min_block_size{MOTR_MIN_BLOCK_SIZE};
    std::size_t m_start_offset{0};
    std::unique_ptr<IoContext> m_io_ctx;
};
} //namespace hestia 


