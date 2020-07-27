# LKmalloc Library
This is my implementation of the LKmalloc library.

    * Wrapper functions around malloc/free
    * Detection of memory leaks/errors
    * Report all calls to lkmalloc/lkfree
    * Regression Tests for various use cases

There is documentation within all header files.

## How to use with your program
Run `make depend` to generate the `lkmalloc.a` static library.
When you compile your program make sure to include `lkmalloc.a` in LDFLAGS
After compiling you may invoke all functions from the library.

## Makefile
Some notable make targets is `make lib` which generates the lkmalloc.a
static library that can be linked with a user's program. `make depend` is
implemented to compile all dependencies as per the HW Doc's request. `make tests`
will compile all test drivers and use test scrips to invoke them to check
if the tests pass or fail. `make clean` will destroy all binary files.

## Test Suites and Regression Tests 
There are 3 directories: `lk[malloc/free/report]_test_suite`. Each of which
contains tests and drivers that test different use case scenarios of the library.
Inside `reg_tests/` directory there are 3 test scripts

    * `test01.sh` invokes the test drivers in lkmalloc_test_suite
    * `test02.sh` invokes the test drivers in lkfree_test_suite
    * `test03.sh` invokes the test drivers in lkreport_test_suite

Upon running the test scripts you will see whether or not the tests fail and
the scenario it is testing. Some scenarios are harder to test, so they
require human verification.

### test01.sh
In order to verify whether or not certain flags were handled correctly, the
test will print out the contents of the buffer since some flags require a 
pattern to be written in. It is up to the user to determine if the pattern
is written correctly. For flags like LKM_UNDER and LKM_OVER, the pointer 
is moved into the appropriate location such that the test
will show the redzones and the payload.

### test03.sh
This test will have lkreport write to files in the dir test_outfile/ 
Since lkreport writes to a file and some of the data in the file is 
non-deterministic (like malloc'd address and arg_ptr address) the tests
can only report whether the expected number of records written is correct.
It is up to the user/operator to look into those output files and see
if the type of records expected is correct.

## Data Structures
### metadata
The way lkmalloc/lkfree "records" are stored is through a struct called 
`metadata`. Implementation resides in `metadata.c` and `metadata.h`
Documentation about function prototypes and struct defintions are in
`metadata.h`. This metadata struct is basically a node in a linked list. 
A note: metadata record type is inversed from what is specified in the
homework doc because I implemented this data strcture and used it in many places
when implementing lkmalloc/lkfree.
I did not realized I inversed the values until I reached lkreport where the
values for record type is INTRODUCED FOR THE FIRST TIME.
At that point, my codebase was too large to refactor for 
one value as it would risk introducing bugs over something so small. 
Do not worry, lkreport writes the correct values as specififed 
in the HW doc thanks to a few checks done in md_to_string().

### hash table
It was not until I was very deep in the homework 
that the professor said to use a BST... 
(https://piazza.com/class/k5wqqpth1lm1gz?cid=160) As a result, I had to live
with my sins. 

The primary data structure used to store all my records is in hash tables.
The reasoning behind this design decision is that the user is most likely
making mass calls to lkmalloc/lkfree so we are doing heavy insert/deletion
which is best suited for the use case of a hash table. 
There are two hash tables `ptr_hash_table[]` and `errors_hash_table[]` defined
and implemented in `hash_table.c hash_table.h`

### ptr_hash_table[]
A memory address (M) will go through get_hash() and a hash (H) 
will be computed from M. H is the index that M will be associated
with in the hash table. The data that lives in ptr_hash_table[H] is
a chain of metadata records that are associated with M. It is possible to have
collisions with two memory addresses map to the same H. The hash table will
proceed with the collision and insert metadata records, collisions 
become an issue when we get to LKmalloc.c functions.

### errors_hash_table[]
This hash table only has 5 indeces that store metadata 
that were created from memory errors. When writing lkreport, I ran into issues
with detecting certain memory errors, so it was easier to dedicate a 
data structure to tracking these errors. The indeces have specific meanings:

    * 0 - malloc w/o corresponding free (generated on LKR_SERIOUS)
    * 1 - bad free (updated when lkfree() called)
    * 2 - orphan free (updated when lkfree() called)
    * 3 - double free (updated when lkfree() called)
    * 4 - approx frees (updated when lkfree() called)

## LKmalloc
As per the professor's suggestion 
(https://piazza.com/class/k5wqqpth1lm1gz?cid=137) lkmalloc and lkfree
are macros that interally call lkmalloc_internal() and lkfree_internal 
in order to extract file name, function name, and line number of the caller.
Some IMPORTANT design decisions influenced by (https://piazza.com/class/k5wqqpth1lm1gz?cid=190): 
    * It is the USER'S RESPONSIBILITY to install lkreport_exit() using on_exit(3) or atexit(3)
    * There are two global variables `extern int file_desc; extern int lkreport_flags;` that must be SET BY THE USER if they want to use lkreport_exit()
    * The library does not prevent the user from creating memory leaks/errors, and proceeds with the action (https://piazza.com/class/k5wqqpth1lm1gz?cid=180)

### lkmalloc_internal()
There is a description of the function in `LKmalloc.h`. So I will detail
the algorithm here. The function will check what flags are set, then 
allocate memory based on the appropriate size since size changes if LKM_UNDER
or LKM_OVER are set. The function will write appropriate patterns in
the appropriate places based on flags set. It will also initialize
the hash tables if it has not been done yet. It will also warn if a
double malloc is detected and fprintf a warning to stderr. Upon completion,
the function will insert a malloc metadata record into ptr_hash_table.

LKM_INIT, LKM_OVER, LKM_UNDER can all be combined with one another. Redzones
combined with LKM_INIT will only zero out the payload, not the redzone.

### check_double_malloc()
The algorithm to check for double mallocs is similar to the balanced
parenthesis problem. This function will traverse over the linked list
of metadata structs and only considers records that have matching
arg_ptr fields in case of collisions. When a malloc record is found, the 
function will look for a corresponding free record, and keep going until it
reaches the end of the list. If by the end of the list we are left with a malloc
record with no matching free, then we have an incoming double malloc record.

### lkfree_internal()
There is a description of the function in `LKmalloc.h`. So I will detail
the algorithm here. There are handlers in the function for every flag that could
be set. In order to detect whether or not a pointer is unallocated, 
ptr_hash_table is queried to retrieve a malloc record that has the same
arg_ptr. If this query returns NULL, then we handle an unallocated pointer. 
If LKF_ERROR is set and triggered, the library will make a call to exit(); The
reason why data structures are not cleaned up immediately is because in the
event that the user wants a report via lkreport_exit(), we do not want to lose
our data that we want to report. In the case that the user does not install
lkreport_exit() it is trusted that the OS will clean up the internal 
data structures.
When a memory error is detected, the lkfree record is not inserted into
ptr_hash_table[], but also inserted into the appropriate index in
errors_hash_table[].

All flags except for LKF_REG can be combined with one another.

### check_double_free()
This algorithm works identical to check_double_malloc() except if by the
end of the list we are left with a free record, then we have an incoming
double free record.

### lkreport()
There are handlers for each flag that could possibly set and combined. The
function also verifies that fd is valid. It would not make much sense
to write to a negative fd or stdin.

All flags expcept for LKR_NONE can be combined with one another.

### lkreport_exit()
Wrapper function around lkreport that is meant to be installed
by the user when they use on_exit(3) or atexit(3)
Cleans up data structures in hash table and reads global vars
file_desc and lkreport_flags that must be SET BY THE USER

### find_mem_leaks()
Search through ptr_hash_table for malloc records without
corresponding frees and inserts those records into errors_hash_table[0].
Called by lkreport

### find_malloc()
Given a linked list of metadata records, it will return the next
malloc record that is NOT a double malloc. 
Called by find_mem_leaks() and match_pairs()

### match_pairs()
Find all malloc-free pairs in ptr_hash_table[] and write to file descriptor
