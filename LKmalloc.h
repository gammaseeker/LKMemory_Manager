/** @file LKmalloc.h
 *  @brief Prototype, macros, global variables for LKmalloc.c
 */
#ifndef LKMALLOC_H
#define LKMALLOC_H
#include <stdint.h>
#include "metadata.h"

#define LKM_REG 0x0
#define LKM_INIT 0x1
#define LKM_OVER 0x2
#define LKM_UNDER 0x4

#define LKF_REG 0x0
#define LKF_APPROX 0x1
#define LKF_WARN 0x2
#define LKF_UNKNOWN 0x4
#define LKF_ERROR 0x8

#define LKR_NONE 0x0
#define LKR_SERIOUS 0x1
#define LKR_MATCH 0x2
#define LKR_BAD_FREE 0x4
#define LKR_ORPHAN_FREE 0x8
#define LKR_DOUBLE_FREE 0x10
#define LKR_APPROX 0x20

/** Macros to extract metadata information, as per:
 *  https://piazza.com/class/k5wqqpth1lm1gz?cid=137
 */
#define lkmalloc(size, ptr, flags) lkmalloc_internal((size), (ptr), (flags), __FILE__, __func__, __LINE__)
#define lkfree(ptr, flags) lkfree_internal((ptr), (flags), __FILE__, __func__, __LINE__)

// For USER TO SET to be passed into lkreport inside lkreport_exit
extern int file_desc;
extern int lkreport_flags;

/** @brief Allocates memory based on size and flags, then writes to ptr
 *
 *  This function will also initialize the hash tables if not initialized
 *  already. It will create a lkmalloc metadata record and check
 *  if the lkmalloc call is a double malloc and report the warning, and
 *  record the metadata in the appropriate hash table
 *  
 *  @return codes:
 *  0 = success
 *  1 = double malloc
 *  2 = invalid flag combination
 *  255 = ENOMEM malloc failed
 */
int lkmalloc_internal(u_int size, void **ptr, u_int flags, 
                        char *filename, const char *fxn_name, int line);

/** @brief Frees memory based on flags passed in
 *
 *  This function will handle errors such as free NULL, unallocated,
 *  approximate frees, and double frees. The function will still proceed
 *  with free errors, and record them in metadata records.
 *
 *  @return codes:
 *  0 = sucess
 *  1 = invalid flag
 *  2 = double free
 *  3 = free NULL
 *  4 = free unalloc ptr
 *  6 = bad free (approx free that failed)
 *  Skipped return code 5 on purpose
 */
int lkfree_internal(void **ptr, u_int flags, 
                        char *filename, const char *fxn_name, int line);

/** @brief Write report of lkmalloc/lkfree calls into 
 *  file descriptor fd based on flags set
 *
 *  This function will also count how many records it writes and return it
 *
 *  @return code:
 *  0 = error or write 0 records (techincally both are true)
 *  value of ctr = number of records written
 */
int lkreport(int fd, u_int flags);

/** @brief Wrapper function around lkreport that is meant to be installed
 *  by the user when they use on_exit(3) or atexit(3)
 *
 *  Cleans up data structures in hash table and reads global vars
 *  file_desc and lkreport_flags that must be SET BY THE USER
 */
void lkreport_exit();

/** @brief Finds malloc records without corresponding frees
 *
 *  Search through ptr_hash_table for malloc records without 
 *  corresponding frees and inserts those records into errors_hash_table.
 *  Called by lkreport
 */
void find_mem_leaks();

/** @brief Finds a malloc record to begin a "malloc-free pair"
 *
 *  Given a linked list of metadata records, it will return the next
 *  malloc record that is NOT a double malloc.
 *  Called by find_mem_leaks() and match_pairs()
 *
 *  @return NULL upon no match or a malloc metadata record
 */
struct metadata *find_malloc(struct metadata *current);

/** @brief Find all malloc-free pairs and write to fd
 *
 *  When approx is 0, we only consider ALL malloc-free pairs 
 *  including approx frees
 *  When approx is 5, we only consider approx malloc-free pairs 
 *
 *  @return the number of records written, on write(2) failure
 *  the function will return the number of records already written before
 *  failure
 */
int match_pairs(int fd, int approx);

/** @brief Takes in a list of metadata records and checks if the
 *  incoming malloc record is a double malloc error
 *  
 *  @return 1 if double malloc detected, 0 no double malloc
 */
int check_double_malloc(struct metadata *md, uintptr_t arg_ptr);

/** @brief Takes in a list of metadata records and checks if the
 *  incoming free record is a double free error
 *  
 *  @return 1 if double free detected, 0 no double free
 */
int check_double_free(struct metadata *md, uintptr_t arg_ptr);

#endif /*LKMALLOC_H*/
