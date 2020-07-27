/** @file metadata.h
 *  @brief Function prototypes for metadata data structure
 *  
 *  Metadata is a node to create a linked list of metadata records
 */
#ifndef METADATA_H
#define METADATA_H

#include <time.h>

struct metadata{
    char *filename;
    int line;
    char *fxn_name;
    void **ret_ptr;
    void **arg_ptr;
    clock_t time;
    /* alloc_free values
     * 0=free 1=alloc 
     */
    int alloc_free; 
    int retval;
/*  error codes
    0=none 1=double malloc 2=double free 3=freenull 
    4=freeunalloc 5=badfreesuccess 6=badfreefail
*/
    int error; 
    u_int size; // malloc only, else -1
    int flag; // free only, else -1
    struct metadata *next;
};

// Create a metadata node
struct metadata* md_init(char *filename, int line, const char *fxn_name,
    void *ret_ptr, void *arg_ptr, clock_t time, int alloc_free, int retval,
    int error, u_int size, int flag);

// Insert node into the chain of metadata structs, head
int md_insert(struct metadata **head, struct metadata *node); 

// Frees all freeable fields and the metadata struct
int md_cleanup(struct metadata **head);

/** @brief Transform metadata struct into a string for use by lkreport
 *
 *  Given a metadata struct, a string will be constructed into the
 *  proper format for use by lkreport
 */
char *md_to_string(struct metadata *md);

/** @brief debug function that prints out a chain of metadata structs
 *
 *  This function was only used during development to help me debug
 */
void md_print(struct metadata **head);

#endif /*METADATA_H*/
