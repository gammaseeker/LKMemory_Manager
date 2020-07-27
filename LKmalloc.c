#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include "hash_table.h"
#include "metadata.h"
#include "LKmalloc.h"

int init = 0;
int lkmalloc_internal(u_int size, void **ptr, u_int flags, 
                    char *filename, const char *fxn_name, int line){
    if(flags < 0) return 2; // invalid flags
    int init_f, over_f, under_f, retval, double_malloc;
    unsigned int hash;
    u_int local_size;
    void *addr;

    init_f = 0, over_f = 0, under_f = 0, retval = 0;
    hash = get_hash((uintptr_t) ptr);
    double_malloc = check_double_malloc(ptr_hash_table[hash], (uintptr_t) ptr);
    local_size = size;

    if(flags & LKM_INIT) init_f = 1;
    if(flags & LKM_OVER){
        over_f = 1;
        local_size += 8;
    }
    if(flags & LKM_UNDER){
        under_f = 1;
        local_size += 8;
    }

    addr = malloc(local_size * sizeof(void));
    if(errno == ENOMEM) return 255; // failed malloc doesnt alloc

    *ptr = addr;
    char *temp_addr = addr;
    if(init_f) memset(addr, 0, local_size);

    if(under_f){
        memset(temp_addr, 0x6b, 8);
        *ptr = addr + 8;
    }

    if(over_f){
        if(under_f){
            memset(addr+8+size, 0x5a, 8);
            *ptr = addr + 8;
        }else{
            memset(addr+size, 0x5a, 8);
        }
    }

    if(init == 0){
        ht_init();
        init = 1;
    }

    if(double_malloc){ 
        fprintf(stderr, "Warning: double malloc detected\n");
        retval = 1;
    }
    struct metadata *m = md_init(filename, line, fxn_name, 
                                addr, ptr, time(NULL), 1, retval, double_malloc,
                                size, -1);
    ht_insert(m);
    return retval;
}

int lkfree_internal(void **ptr, u_int flags, 
                    char *filename, const char *fxn_name, int line){
    if(flags < 0) return 1; // invalid flags

    if(init == 0){
        ht_init();
        init = 1;
    }

    int approx_f, warn_f, unknown_f, error_f;
    approx_f = 0, warn_f = 0, unknown_f = 0, error_f = 0;

    if(flags & LKF_APPROX) approx_f = 1; 
    if(flags & LKF_WARN) warn_f = 1;
    if(flags & LKF_UNKNOWN) unknown_f = 1;
    if(flags & LKF_ERROR) error_f = 1;

    // NULL ptr handler
    if(*ptr == NULL){
        struct metadata *null_record = md_init(filename, line, fxn_name, 
                                NULL, ptr, time(NULL), 0, 3, 3, -1, flags);
        ht_insert(null_record);
        free(NULL);
        return 3;
    }

    // Find addr returned by malloc
    void *original_buff = NULL;
    struct metadata *m;
    unsigned int hash = get_hash((uintptr_t) ptr);
    struct metadata *current = ptr_hash_table[hash];
    while(current != NULL){
        if((uintptr_t)(current->arg_ptr) == (uintptr_t)ptr){ // check arg_ptr
            if(current->alloc_free == 1){ // check malloc record
                original_buff = current->ret_ptr; // grabs redzones too
            }
        }
        current = current->next;
    }
    
    // Unalloc ptr handler
    if(original_buff == NULL){
        if(unknown_f) fprintf(stderr, "Warning: ptr never allocated\n");
        if(error_f) exit(1); 
        struct metadata *unalloc_record = md_init(filename, line, fxn_name, 
                                    NULL, ptr, time(NULL), 0, 4, 4, -1, flags);
        ht_insert(unalloc_record);
        unalloc_insert(unalloc_record);
        return 4;
    }

    // Free mid mem chunk but LKF_APPROX not set
    if(!approx_f && original_buff != *ptr){
        if(warn_f) fprintf(stderr,"Warning: freeing in middle of a valid allocation\n");
        if(error_f) exit(1); 
        struct metadata *bad_free = md_init(filename, line, fxn_name, 
                            NULL, ptr, time(NULL), 0, 6, 6, -1, flags);
        ht_insert(bad_free);
        bad_free_insert(bad_free);
        return 6;
    }

    // Double free handler
    if(check_double_free(ptr_hash_table[hash], (uintptr_t) ptr)){
        fprintf(stderr, "Warning: double free detected\n");
        struct metadata *double_free = md_init(filename, line, fxn_name, 
                                NULL, ptr, time(NULL), 0, 2, 2, -1, flags);
        ht_insert(double_free);
        double_free_insert(double_free);
        return 2;
    }

    // Free mid mem chunk handler
    if(approx_f && original_buff != *ptr){
        free(original_buff);
        m = md_init(filename, line, fxn_name, NULL, ptr, 
                    time(NULL), 0, 0, 5, -1, flags);
        ht_insert(m);
        approx_insert(m);
        if(warn_f){
            fprintf(
                stderr, "Warning: freeing in middle of a valid allocation\n");
            if(error_f) exit(1);
        }
    }
    
    // Regular free
    if(flags == LKF_REG){
        free(*ptr);
        m = md_init(filename, line, fxn_name, 
                    NULL, ptr, time(NULL), 0, 0, 0, -1, flags);
        ht_insert(m);
    }
    return 0;
}

int lkreport(int fd, u_int flags){
    if(flags == 0) return 0;
    if(fd < 0 || fd == STDIN_FILENO) return 0;

    char *header = "record_type,filename,fxname,line_num,timestamp,ptr_passed,retval,size_or_flags,alloc_addr_returned\n";
    int bytes_written = write(fd, header, strlen(header));
    if(bytes_written < 0) return 0;
    if(flags < 0) return 0;

    int serious_f, match_f, bad_free_f, orphan_f, double_f, approx_f, ctr;
    serious_f = 0, match_f = 0, bad_free_f = 0; 
    orphan_f = 0, double_f = 0, approx_f = 0;
    ctr = 0;

    if(flags & LKR_SERIOUS) serious_f = 1;
    if(flags & LKR_MATCH) match_f = 1;
    if(flags & LKR_BAD_FREE) bad_free_f = 1;
    if(flags & LKR_ORPHAN_FREE) orphan_f = 1;
    if(flags & LKR_DOUBLE_FREE) double_f = 1;
    if(flags & LKR_APPROX)  approx_f = 1;

    if(serious_f){
        find_mem_leaks();
        struct metadata *current = errors_hash_table[0];
        while(current != NULL){
            char *record = md_to_string(current);
            bytes_written = write(fd, record, strlen(record));
            free(record);
            ctr++;
            current = current->next;
        }
        md_cleanup(&(errors_hash_table[0]));
    }

    if(match_f){
        ctr += match_pairs(fd, 0);
    }

    if(bad_free_f){
        struct metadata *current = errors_hash_table[1];
        while(current != NULL){
            char *record = md_to_string(current);
            bytes_written = write(fd, record, strlen(record));
            free(record);
            ctr++;
            current = current->next;
        }
    }

    if(orphan_f){
        struct metadata *current = errors_hash_table[2];
        while(current != NULL){
            char *record = md_to_string(current);
            bytes_written = write(fd, record, strlen(record));
            free(record);
            ctr++;
            current = current->next;
        }
    }

    if(double_f){
        struct metadata *current = errors_hash_table[3];
        while(current != NULL){
            char *record = md_to_string(current);
            bytes_written = write(fd, record, strlen(record));
            free(record);
            ctr++;
            current = current->next;
        }
    }

    if(approx_f){
        ctr += match_pairs(fd, 5);
    }
    return ctr;
}

void lkreport_exit(){
    int fd = file_desc; 
    int flags = lkreport_flags;
    lkreport(fd, flags);
    ht_free(); // Clean up DS
}

void find_mem_leaks(){
    int i;
    int break_stat = 0;
    for(i=0; i < HASH_TABLE_SIZE; i++){
        if(ptr_hash_table[i] != 0){
            struct metadata *current = ptr_hash_table[i];
            while(current != NULL){
                struct metadata *malloc_pair = find_malloc(current);

                if(malloc_pair != NULL){
                    struct metadata *runner = NULL;

                    if(malloc_pair->next != NULL){
                        runner = malloc_pair->next;
                    }

                    while(runner != NULL){
                        if(((uintptr_t) malloc_pair->arg_ptr == 
                            (uintptr_t)runner->arg_ptr) && 
                                (runner->error == 0 || runner->error == 5) 
                                && (runner->alloc_free == 0)){
                                    // found pair free, move on
                                    break_stat = 1;
                                    break;
                                }
                        runner = runner->next;
                    }

                    if(!break_stat){
                        mem_leak_insert(malloc_pair);
                    }else{
                        break_stat = 0;
                    }
                }
                current = current->next;
            }
        }
    }
}

struct metadata *find_malloc(struct metadata *current){
    struct metadata *cursor = current;
    while(cursor != NULL){
        if(cursor->alloc_free == 1 && cursor->error == 0){
            return cursor;
        }
        cursor = cursor->next;
    }
    return NULL;
}

int match_pairs(int fd, int approx){
    int ctr;
    int i;
    ctr = 0;
    for(i=0; i < HASH_TABLE_SIZE; i++){
        if(ptr_hash_table[i] != 0){
            struct metadata *current = ptr_hash_table[i];
            while(current != NULL){
                // Init first malloc node
                struct metadata *malloc_pair = find_malloc(current);
                if(malloc_pair != NULL){
                    struct metadata *runner = NULL;
                    if(malloc_pair->next != NULL) runner = malloc_pair->next;
                    while(runner != NULL){
                        if(approx == 0){
                            if(((uintptr_t) malloc_pair->arg_ptr == 
                                (uintptr_t)runner->arg_ptr) && 
                                    (runner->error == 0 || runner->error == 5) 
                                    && (runner->alloc_free == 0)){
                                        char *record = md_to_string(malloc_pair);
                                        int bytes_written = write(fd, record, strlen(record));
                                        if(bytes_written < 0) return ctr;
                                        free(record);
                                        ctr += 1;

                                        record = md_to_string(runner);
                                        bytes_written = write(fd, record, strlen(record));
                                        if(bytes_written < 0) return ctr;
                                        free(record);
                                        ctr += 1;
                                    }
                        }
                        if(approx == 5){
                            if(((uintptr_t) malloc_pair->arg_ptr == 
                                (uintptr_t)runner->arg_ptr) && 
                                    (runner->error == 5) && (runner->alloc_free == 0)){
                                        char *record = md_to_string(malloc_pair);
                                        int bytes_written = write(fd, record, strlen(record));
                                        if(bytes_written < 0) return ctr;
                                        free(record);
                                        ctr += 1;

                                        record = md_to_string(runner);
                                        bytes_written = write(fd, record, strlen(record));
                                        if(bytes_written < 0) return ctr;
                                        free(record);
                                        ctr += 1;
                                    }
                        }
                        runner = runner->next;
                    }
                }
                current = current->next;
            }
        }
    }
    return ctr;
}

int check_double_malloc(struct metadata *md, uintptr_t arg_ptr){
    struct metadata *current = md;
    int toggle = 0; // 0=free malloc pair found 1=malloc only
    while(current != NULL){
        // in case of collisions, must do this check
        if(arg_ptr == (uintptr_t)current->arg_ptr){ 
            if((current->error == 0) || (current->error == 5)){
                if((toggle == 0) && (current->alloc_free == 1)){
                    toggle = 1;
                }
                if((toggle == 1) && (current->alloc_free == 0)){
                    toggle = 0;
                }
            }
        }
        current = current->next;
    }

    if(toggle == 1) return 1; // double malloc found
    return 0; // no double malloc
}

int check_double_free(struct metadata *md, uintptr_t arg_ptr){
    struct metadata *current = md;
    int toggle = 0; // 0=free malloc pair found 1=malloc only
    while(current != NULL){
        if(arg_ptr == (uintptr_t)current->arg_ptr){ 
            if((current->error == 0) || (current->error == 5)){
                if((toggle == 0) && (current->alloc_free == 1)){
                    toggle = 1;
                }
                if((toggle == 1) && (current->alloc_free == 0)){
                    toggle = 0;
                }
            }
        }
        current = current->next;
    }
    if(toggle == 0) return 1; // double free found
    return 0; // no double free 
}
