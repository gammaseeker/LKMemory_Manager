#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "hash_table.h"

int init = 0;

unsigned int get_hash(uintptr_t mem_addr){
    unsigned int hash = 0;
    int buff_size = (int)log10(mem_addr) + 1;
    char buff[buff_size];
    sprintf(buff, "%ld", mem_addr);
    for(int i = 0; i < strlen(buff); i++){
        hash += buff[i];
    }
    hash = hash % HASH_TABLE_SIZE;
    return hash;
}

void ht_init(){
    memset(ptr_hash_table, 0x0, HASH_TABLE_SIZE*sizeof(ptr_hash_table[0]));
    memset(errors_hash_table, 0x0, 3*sizeof(errors_hash_table[0]));
}

void ht_insert(struct metadata *md){
    unsigned int hash = get_hash((uintptr_t) (md->arg_ptr));
    md_insert(&(ptr_hash_table[hash]), md);
}

void mem_leak_insert(struct metadata *md){
    if(md_insert(&(errors_hash_table[0]), md) != 0) 
        perror("Error: Insert null node\n");
}

void bad_free_insert(struct metadata *md){
    if(md_insert(&(errors_hash_table[1]), md) != 0)
        perror("Error: Insert null node\n");
}

void unalloc_insert(struct metadata *md){
    if(md_insert(&(errors_hash_table[2]), md) != 0)
        perror("Error: Insert null node\n");
}

void double_free_insert(struct metadata *md){
    if(md_insert(&(errors_hash_table[3]), md) != 0)
        perror("Error: Insert null node\n");
}

void approx_insert(struct metadata *md){
    if(md_insert(&(errors_hash_table[4]), md) != 0)
        perror("Error: Insert null node\n");
}

void ht_free(){
    int i;
    for(i = 0; i < HASH_TABLE_SIZE; i++){
        if(ptr_hash_table[i] != 0){
            md_cleanup(&(ptr_hash_table[i]));
        }
    }

    i = 0;
    for(i = 0; i < 5; i++){
        if(errors_hash_table[i] != 0){
            md_cleanup(&(errors_hash_table[i]));
        }
    }
}
