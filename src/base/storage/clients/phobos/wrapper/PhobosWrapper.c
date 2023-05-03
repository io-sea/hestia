#if HAS_PHOBOS
#include "pho_attrs.h"
#include "phobos_store.h"

int phobos_put_cpp(struct pho_xfer_desc* xfers, size_t n, pho_completion_cb_t cb, void* udata)
{
    return phobos_put(xfers, n, cb, udata);
}

int phobos_get_cpp(struct pho_xfer_desc* xfers, size_t n, pho_completion_cb_t cb, void* udata)
{
    return phobos_get(xfers, n, cb, udata);
}

int phobos_attrs_foreach_cpp(const struct pho_attrs* md, pho_attrs_iter_t cb, void* udata)
{
    return pho_attrs_foreach(md, cb, udata);
}

int phobos_getmd_cpp(struct pho_xfer_desc* xfers, size_t n, pho_completion_cb_t cb, void* udata)
{
    return phobos_getmd(xfers, n, cb, udata);
}

int phobos_delete_cpp(struct pho_xfer_desc* xfers, size_t num_xfers)
{
    return phobos_delete(xfers, num_xfers);
}

void pho_xfer_desc_destroy_cpp(struct pho_xfer_desc* xfer)
{
    return pho_xfer_desc_clean(xfer);
}

int phobos_store_object_list_cpp(const char** res,int n_res, bool is_pattern, const char** metadata,
    int n_metadata, bool deprecated, struct object_info** objs, int* n_objs)
{
    return phobos_store_object_list(res, n_res, is_pattern, metadata, n_metadata, deprecated, objs, n_objs);
}

void phobos_store_object_list_free_cpp(struct object_info* objs, int n_objs)
{
    return phobos_store_object_list_free(objs, n_objs);
}

#endif
