// Header to be included by cpp projects that intend to use the phobos API

#ifndef DEIMOS_PHO_CPP_LIB_H
#define DEIMOS_PHO_CPP_LIB_H

#if HAS_PHOBOS

#include <pho_attrs.h>

/**
 * Family of resource.
 * Families can be seen here as storage technologies.
 */
enum rsc_family {
    PHO_RSC_INVAL = -1,
    PHO_RSC_DISK  = 0, /**< Not supported yet */
    PHO_RSC_TAPE  = 1, /**< Tape, drive tape or tape library */
    PHO_RSC_DIR   = 2, /**< Directory */
    PHO_RSC_LAST,
    PHO_RSC_UNSPEC = PHO_RSC_LAST,
};

static const char* const rsc_family_names[] = {"disk", "tape", "dir"};

static inline const char* rsc_family2str(enum rsc_family family)
{
    if (family >= PHO_RSC_LAST || family < 0) {
        return nullptr;
    }
    return rsc_family_names[family];
}

static inline enum rsc_family str2rsc_family(const char* str)
{
    int i;
    for (i = 0; i < PHO_RSC_LAST; i++) {
        if (!strcmp(str, rsc_family_names[i])) return rsc_family(i);  // NOLINT
    }
    return PHO_RSC_INVAL;
}

/**
 * A simple array of tags (strings)
 */
struct tags {
    char** tags;   /**< The array of tags */
    size_t n_tags; /**< Number of tags */
};

/**
 * Transfer (GET / PUT / MPUT) flags.
 * Exact semantic depends on the operation it is applied on.
 */
enum pho_xfer_flags {
    /**
     * put: replace the object if it already exists (_not supported_)
     * get: replace the target file if it already exists
     */
    PHO_XFER_OBJ_REPLACE = (1 << 0),
    /* get: check the object's location before getting it */
    PHO_XFER_OBJ_BEST_HOST = (1 << 1),
};

/**
 * Multiop completion notification callback.
 * Invoked with:
 *  - user-data pointer
 *  - the operation descriptor
 *  - the return code for this operation: 0 on success, neg. errno on failure
 */
typedef void (*pho_completion_cb_t)(void* u, const struct pho_xfer_desc*, int);

/**
 * Phobos XFer operations.
 */
enum pho_xfer_op {
    PHO_XFER_OP_PUT,   /**< PUT operation. */
    PHO_XFER_OP_GET,   /**< GET operation. */
    PHO_XFER_OP_GETMD, /**< GET metadata operation. */
    PHO_XFER_OP_DEL,   /**< DEL operation. */
    PHO_XFER_OP_UNDEL, /**< UNDEL operation. */
    PHO_XFER_OP_LAST
};

/**
 * PUT parameters.
 * Family, layout_name and tags can be set directly or by using an alias.
 * An alias is a name defined in the phobos config to combine these parameters.
 * The alias will not override family and layout if they have been specified
 * in this struct but extend existing tags.
 */
struct pho_xfer_put_params {
    ssize_t size;            /**< Amount of data to write. */
    enum rsc_family family;  /**< Targeted resource family. */
    const char* layout_name; /**< Name of the layout module to use. */
    struct tags tags;        /**< Tags to select a media to write. */
    const char* alias;       /**< Identifier for family, layout,
                              *  tag combination
                              */
    bool overwrite;          /**< true if the put command could be an
                              *  update.
                              */
};

/**
 * GET parameters.
 * Node_name corresponds to the name of the node the object can be retrieved
 * from, if a phobos_get call fails.
 */
struct pho_xfer_get_params {
    char* node_name; /**< Node name [out] */
};

/**
 * Operation parameters.
 */
union pho_xfer_params {
    struct pho_xfer_put_params put; /**< PUT parameters. */
    struct pho_xfer_get_params get; /**< GET parameters */
};

/**
 * Xfer descriptor.
 * The source/destination semantics of the fields vary
 * depending on the nature of the operation.
 * See below:
 *  - pĥobos_getmd()
 *  - phobos_get()
 *  - phobos_put()
 *  - phobos_undelete()
 */
struct pho_xfer_desc {
    char* xd_objid;                  /**< Object ID to read or write. */
    char* xd_objuuid;                /**< Object UUID to read or write. */
    int xd_version;                  /**< Object version. */
    enum pho_xfer_op xd_op;          /**< Operation to perform. */
    int xd_fd;                       /**< FD of the source/destination. */
    struct pho_attrs xd_attrs;       /**< User defined attributes. */
    union pho_xfer_params xd_params; /**< Operation parameters. */
    enum pho_xfer_flags xd_flags;    /**< See enum pho_xfer_flags doc. */
    int xd_rc;                       /**< Outcome of this xfer. */
};

struct object_info {
    char* oid;
    char* uuid;
    int version;
    char* user_md;
    struct timeval deprec_time;
};

/**
 * Put N files to the object store with minimal overhead.
 * Each desc entry contains:
 * - objid: the target object identifier
 * - fd: an opened fd to read from
 * - size: amount of data to read from fd
 * - layout_name: (optional) name of the layout module to use
 * - attrs: the metadata (optional)
 * - flags: behavior flags
 * - tags: tags defining constraints on which media can be selected to put the
 *   data
 * Other fields are not used.
 *
 * Individual completion notifications are issued via xd_callback.
 * This function returns the first encountered error or 0 if all
 * sub-operations have succeeded.
 */
int phobos_put_cpp(
    struct pho_xfer_desc* xfers, size_t n, pho_completion_cb_t cb, void* udata);

/**
 * Retrieve N files from the object store
 * desc contains:
 * - objid: identifier of the object to retrieve
 * - fd: an opened fd to write to
 * - attrs: unused (can be NULL)
 * - flags: behavior flags
 * Other fields are not used.
 *
 * Individual completion notifications are issued via xd_callback.
 * This function returns the first encountered error or 0 if all
 * sub-operations have succeeded.
 */
int phobos_get_cpp(
    struct pho_xfer_desc* xfers, size_t n, pho_completion_cb_t cb, void* udata);

/**
 * Retrieve N file metadata from the object store
 * desc contains:
 * - objid: identifier of the object to retrieve
 * - attrs: unused (can be NULL)
 * - flags: behavior flags
 * Other fields are not used.
 *
 * Individual completion notifications are issued via xd_callback.
 * This function returns the first encountered error of 0 if all
 * sub-operations have succeeded.
 */
int phobos_getmd_cpp(
    struct pho_xfer_desc* xfers, size_t n, pho_completion_cb_t cb, void* udata);

/**
 * Delete an object from the object store
 *
 * This deletion is not a hard remove, and only deprecate the object.
 *
 * @param[in]   xfers       Objects to delete, only the oid field is used
 * @param[in]   num_xfers   Number of objects to delete
 *
 * @return                  0 on success, -errno on failure
 */
int phobos_delete_cpp(struct pho_xfer_desc* xfers, size_t num_xfers);

/**
 * Loops over pho_attr key/value pairs and calls
 * the cb function argument
 * desc contains:
 * - md: Phobos meta data struct
 * - cb: callback function
 * - udata: implementation details
 *
 * Individual completion notifications are issued via xd_callback.
 * This function returns the first encountered error of 0 if all
 * sub-operations have succeeded.
 */
int phobos_attrs_foreach_cpp(
    const struct pho_attrs* md, pho_attrs_iter_t cb, void* udata);

/** query metadata of the object store */
/* TODO int phobos_query(criteria, &obj_list); */
/* TODO int phobos_del(); */

/**
 * Free tags and attributes resources associated with this xfer,
 * as they are allocated in phobos.
 */
void pho_xfer_desc_destroy_cpp(struct pho_xfer_desc* xfer);

/**
 * Retrieve the objects that match the given pattern and metadata.
 * If given multiple objids or patterns, retrieve every item with name
 * matching any of those objids or patterns.
 * If given multiple objids or patterns, and metadata, retrieve every item
 * with name matching any of those objids or pattersn, but containing
 * every given metadata.
 *
 * The caller must release the list calling phobos_store_object_list_free().
 *
 * \param[in]       res             Objids or patterns, depending on
 *                                  \a is_pattern.
 * \param[in]       n_res           Number of requested objids or patterns.
 * \param[in]       is_pattern      True if search using POSIX pattern.
 * \param[in]       metadata        Metadata filter.
 * \param[in]       n_metadata      Number of requested metadata.
 * \param[in]       deprecated      true if search from deprecated objects.
 * \param[out]      objs            Retrieved objects.
 * \param[out]      n_objs          Number of retrieved items.
 *
 * \return                          0     on success,
 *                                 -errno on failure.
 */
int phobos_store_object_list_cpp(
    const char** res,
    int n_res,
    bool is_pattern,
    const char** metadata,
    int n_metadata,
    bool deprecated,
    struct object_info** objs,
    int* n_objs);

/**
 * Release the list retrieved using phobos_store_object_list().
 *
 * \param[in]       objs            Objects to release.
 * \param[in]       n_objs          Number of objects to release.
 */
void phobos_store_object_list_free_cpp(struct object_info* objs, int n_objs);

#endif

#endif
