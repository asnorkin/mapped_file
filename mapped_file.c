#include "mapped_file.h"

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/stat.h>
#include <unistd.h>



#include "chunk_manager/chunk_manager.h"
#include "logger/logger.h"



mf_handle_t mf_open(const char *pathname)
{
    log_write(DEBUG, "mf_open: started");

    if(!pathname)
    {
        log_write(DEBUG, "mf_open: bad filename");
        return MF_OPEN_FAILED;
    }

    log_write(DEBUG, "mf_open: started");

    int fd = open(pathname, O_RDWR, 0666);
    if(fd == -1)
    {
        log_write(ERROR, "mf_open: can't open file:%s, errno=%d",
                  pathname, errno);
        return MF_OPEN_FAILED;
    }

    chpool_t *chpool = chp_init(fd, PROT_READ | PROT_WRITE);
    if(!chpool)
    {
        log_write(ERROR, "mf_open: can't init chunk pool");
        return MF_OPEN_FAILED;
    }

    log_write(DEBUG, "mf_open: started");
    return (mf_handle_t)chpool;
}






void *mf_map(mf_handle_t mf, off_t offset, size_t size,
             mf_mapmem_handle_t *mapmem_handle)
{
    if(mf == MF_OPEN_FAILED || offset < 0 ||
            offset + size > mf_file_size(mf))
        return NULL;

    log_write(DEBUG, "mf_map: started");

    if(!size)
        return NULL;

    //  cast offset to page size in bytes
    off_t pa_offset = offset & ~(sysconf(_SC_PAGESIZE) - 1);
    off_t new_size = size + offset - pa_offset;
    //  convert chunk size into pagesizes
    off_t index = pa_offset / get_chunk_size(1);
    off_t len = new_size / get_chunk_size(1);
    if(new_size % get_chunk_size(1))
        len++;

    int error = chp_find((chpool_t *)mf, index, len, (chunk_t **)mapmem_handle);
    if(error == ENOKEY)
    {
        *mapmem_handle = (mf_mapmem_handle_t *)ch_init(index, len, (chpool_t *)mf);
        if(!(*mapmem_handle))
        {
            log_write(ERROR, "mf_map: can't init new chunk");
            return NULL;
        }

        log_write(DEBUG, "mf_map: finished");
        return (void *)((off_t)(((chunk_t *)(*mapmem_handle))->data)
                        + (offset - pa_offset));
    }
    else if(error)
    {
        log_write(ERROR, "mf_map: chp_find returns error=%d", error);
        return NULL;
    }

    log_write(DEBUG, "mf_map: finished");
    return (void *)((int)(((chunk_t *)(*mapmem_handle))->data) +
         (offset - (int)((((chunk_t *)(*mapmem_handle))->index)
                                      * get_chunk_size(1))));
}



int mf_unmap(mf_handle_t mf, mf_mapmem_handle_t mapmem_handle)
{
    if(mf == MF_OPEN_FAILED || !mapmem_handle)
        return EINVAL;

    log_write(DEBUG, "mf_unmap: started");

    int error = chp_chunk_release((chunk_t *)mapmem_handle);
    if(error)
        return error;

    log_write(DEBUG, "mf_unmap: finished");

    return 0;
}



off_t mf_file_size(mf_handle_t mf)
{
    log_write(DEBUG,"mf_file_size: started");

    if(mf == MF_OPEN_FAILED)
    {
        log_write(ERROR, "mf_file_size: bad mf");
        return EINVAL;
    }

    int fd = ((chpool_t *)mf)->fd;
    if(fd < 0)
    {
        log_write(ERROR, "mf_file_size: bad file descriptor");
        return -1;
    }

    struct stat sb = {0};
    int error = fstat(fd, &sb);
    if(error)
    {
        log_write(ERROR, "mf_file_size: fd returns error=%d", error);
        return -1;
    }

    log_write(DEBUG,"mf_file_size: finished");
    return sb.st_size;
}
















