#include <stdio.h>
#include <fcntl.h>

#include "logger/logger.h"
#include "chunk_manager/chunk_manager.h"
#include "mapped_file.h"



int main(void)
{
    int error = 0;
    error = log_set_default_loglvl(INFO);

    int flags = O_RDWR;
    int perms = 0666;
    int prot = (flags & O_RDWR) ? PROT_READ | PROT_WRITE :
               (flags & O_RDONLY) ? PROT_READ : PROT_WRITE;
    int fd = open("test.txt", flags, perms);

    printf("Start testing\n");



    /*chpool_t *chpl = chp_init(fd, prot);
    if(!chpl)
        printf("can't init chunk pool\n");

    dcl_print(chpl->free_list);

    chunk_t *chunk = ch_init(1, 1, chpl);
    if(!chunk)
        printf("can't init chunk\n");*/

    mf_handle_t mf = mf_open("test.txt");
    if(mf == MF_OPEN_FAILED)
        printf("can't open file\n");

    log_set_default_loglvl(DEBUG);

    mf_mapmem_handle_t *mapmem = (mf_mapmem_handle_t *)calloc(1, sizeof(mf_mapmem_handle_t));
    void *ptr = mf_map(mf, 4, 5, mapmem);
    if(!ptr)
        printf("can't map memory\n");
    else
    {
        int i = 0;
        while(((char *)ptr)[i] != EOF)
            printf("%c ", ((char *)ptr)[i++]);
    }










    printf("Finish testing\n");
    error = logger_deinit();

    return 0;
}
