/** @file hash_table.h
 *  @brief Function prototypes for hash table data structure
 *  
 *  Contains prototypes, macros and data structures needed to 
 *  build and use the hash table(s)
 */
#ifndef HASH_TABLE_H
#define HASH_TABLE_H
#include "metadata.h"
#define HASH_TABLE_SIZE 10007 // big prime number reduces collisions

/** @brief hash table where key is ptr passed into lkmalloc/lkfree,
 *  maps key to linked lists of lkmalloc/lkfree records
 */
struct metadata *ptr_hash_table[HASH_TABLE_SIZE];

/** @brief hash table where every index contains a linked list of
 *  records with errors:
 *  index - description
 *  0 - malloc w/o corresponding free (generated on LKR_SERIOUS)
 *  1 - bad free (updated when lkfree() called)
 *  2 - orphan free (updated when lkfree() called)
 *  3 - double free (updated when lkfree() called)
 *  4 - approx frees (updated when lkfree() called)
 */
struct metadata *errors_hash_table[5]; 

/** @brief returns index to go to in ptr_hash_table given some mem address
 *  
 *  The parameter will be the mem address of the pointer passed into
 *  lkmalloc/lkfree and it will compute a hash by summing all digits
 *  of the memory address then taking the sum % HASH_TABLE_SIZE to get
 *  the index the pointer should be mapped to
 *  
 *  @param mem_addr the memory address to hash 
 *  @return an index to ptr_hash_table
 */
unsigned int get_hash(uintptr_t mem_addr);

// zeros our ptr_hash_table and errors_hash_table
void ht_init();

/** @brief metadata record gets inserted into the
 *  appropriate index in ptr_hash_table
 *
 *  The function will get the arg_ptr from the metadata and compute
 *  a hash on it. Then insert that metadata struct into the correct
 *  index in ptr_hash_table
 */
void ht_insert(struct metadata *md);

// Insert metadata record into index 0 of errors_hash_table
void mem_leak_insert(struct metadata *md);

// Insert metadata record into index 1 of errors_hash_table
void bad_free_insert(struct metadata *md);

// Insert metadata record into index 2 of errors_hash_table
void unalloc_insert(struct metadata *md);

// Insert metadata record into index 3 of errors_hash_table
void double_free_insert(struct metadata *md);

// Insert metadata record into index 4 of errors_hash_table
void approx_insert(struct metadata *md);

/** free all metadata structs in all hash tables
 *  only called in lkreport_exit()
 */
void ht_free();

#endif /*HASH_TABLE_H*/
