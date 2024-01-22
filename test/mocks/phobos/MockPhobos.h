#pragma once

#include "Map.h"

#include <functional>
#include <string>
#include <unordered_map>
#include <vector>

struct pho_attrs {
    hestia::Map attr_set;
};

enum pho_xfer_op {
    PHO_XFER_OP_PUT,   /**< PUT operation. */
    PHO_XFER_OP_GET,   /**< GET operation. */
    PHO_XFER_OP_GETMD, /**< GET metadata operation. */
    PHO_XFER_OP_DEL,   /**< DEL operation. */
    PHO_XFER_OP_UNDEL, /**< UNDEL operation. */
    PHO_XFER_OP_LAST
};

enum pho_xfer_flags {
    PHO_XFER_OBJ_REPLACE   = (1 << 0),
    PHO_XFER_OBJ_BEST_HOST = (1 << 1),
};

enum rsc_family {
    PHO_RSC_INVAL = -1,
    PHO_RSC_DISK  = 0, /**< Not supported yet */
    PHO_RSC_TAPE  = 1, /**< Tape, drive tape or tape library */
    PHO_RSC_DIR   = 2, /**< Directory */
    PHO_RSC_LAST,
    PHO_RSC_UNSPEC = PHO_RSC_LAST,
};

struct pho_xfer_put_params {
    ssize_t size{0};                         /**< Amount of data to write. */
    enum rsc_family family { PHO_RSC_DISK }; /**< Targeted resource family. */
    std::string layout_name;       /**< Name of the layout module to use. */
    std::vector<std::string> tags; /**< Tags to select a media to write. */
    std::string alias;    /**< Identifier for family, layout, tag combination */
    bool overwrite{true}; /**< true if the put command could be an update. */
};

struct pho_xfer_get_params {
    std::string node_name; /**< Node name [out] */
};

union pho_xfer_params {
    struct pho_xfer_put_params put; /**< PUT parameters. */
    struct pho_xfer_get_params get; /**< GET parameters */
};

struct pho_xfer_desc {
    std::string xd_objid;   /**< Object ID to read or write. */
    std::string xd_objuuid; /**< Object UUID to read or write. */
    int xd_version{0};      /**< Object version. */
    pho_xfer_op xd_op;      /**< Operation to perform. */
    int xd_fd{0};           /**< FD of the source/destination. */
    pho_attrs xd_attrs;     /**< User defined attributes. */
    pho_xfer_put_params xd_put_params;

    // union pho_xfer_params xd_params; /**< Operation parameters. */
    enum pho_xfer_flags xd_flags; /**< See enum pho_xfer_flags doc. */
    int xd_rc{0};                 /**< Outcome of this xfer. */
};

struct object_info {
    std::string oid;
    std::string uuid;
    int version{0};
    std::string user_md;
    struct timeval deprec_time;
};

class MockPhobos {
  public:
    using pho_completion_cb_t =
        std::function<void(void* u, const pho_xfer_desc*, int)>;
    int phobos_put(
        pho_xfer_desc* xfers, size_t n, pho_completion_cb_t cb, void* udata);

    int phobos_get(
        pho_xfer_desc* xfers, size_t n, pho_completion_cb_t cb, void* udata);

    int phobos_getmd(
        pho_xfer_desc* xfers, size_t n, pho_completion_cb_t cb, void* udata);

    int phobos_delete(pho_xfer_desc* xfers, size_t num_xfers);

    using pho_attrs_iter_t =
        std::function<int(const char* key, const char* val, void* udata)>;
    int phobos_attrs_foreach(
        const pho_attrs* md, pho_attrs_iter_t cb, void* udata);

    void pho_xfer_desc_destroy(pho_xfer_desc* xfer);

    int phobos_store_object_list(
        const char** res,
        int n_res,
        bool is_pattern,
        const char** metadata,
        int n_metadata,
        bool deprecated,
        object_info** objs,
        int* n_objs);

    void phobos_store_object_list_free(object_info* objs, int n_objs);

    void set_root(const std::string& root);

  private:
    void write_md(const std::string& id, const hestia::Map& map);

    void write_data(const std::string& id, const std::vector<char>& data);

    bool read_md(const std::string& id, hestia::Map& map);

    bool read_data(const std::string& id, std::vector<char>& data);

    bool remove_data(const std::string& id);

    bool remove_md(const std::string& id);

    std::string m_root;
    std::unordered_map<std::string, std::vector<char>> m_data_cache;
    std::unordered_map<std::string, hestia::Map> m_metadata_cache;
};