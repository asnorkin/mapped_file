# Mapped file Library

Library provides basic functions (open, close, read, write, mmap, ...) of linux like C interface for working with files, that do not fit into RAM. Herewith, user can work with them like usual files.

It was developed as a part of the Industrial Programming course at MIPT.

### Main idea

Library provides basic functions for working with file (open, close, read, write, mmap, ...). However, this functions do not load entire file into RAM. Instead, they maintain only those parts of the file which user are working with now. Herewith, the library interface is designed to hide chunk management from user.

### Usage
```
#include "mapped_file.h"

int main() {
    // Open Parallels VM image file that has 100 GB size
    mf_handle_t mf = mf_open('win10_image.pvm');

    // Create buffer with 1KB size where we will map part of file
    mf_mapmem_handle_t *buf = (mf_mapmem_handle_t *)calloc(1000, 1);

    // Map first 1KB of file into memory
    void *data = mf_map(mf, 0, 1000, buf);

    // Do something
    // ...

    // Unmap
    mf_unmap(mf, buf);

    // Close
    mf_close(mf);
    
    return 0;
}
```
