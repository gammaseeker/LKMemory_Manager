#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include "../LKmalloc.h"
#include "lkreport_test.h"

int lkreport_wrapper(char *filepath, int flag){
    int fd = open(filepath, O_WRONLY);
    if(fd < 1) exit(1);
    int ret = lkreport(fd, flag);
    close(fd);
    return ret;
}

int test_write_header(){
    return lkreport_wrapper("test_outfile/test_write_header.txt",-1);
}

int test_orphan_free(){
    char *c;
    char *v;
    lkfree((void *)&c, 4);
    lkfree((void *)&v, 4);
    return lkreport_wrapper("test_outfile/test_orphan_free.txt", 8);
}

int test_bad_free(){
    char *success = NULL;
    char *fail = NULL;
    int ret = lkmalloc(5, (void *)&success, 0);
    if(ret != 0) return 1;

    ret = lkmalloc(5, (void *)&fail, 0);
    if(ret != 0) return 1;

    success += 2;
    fail += 2;

    lkfree((void *) &success, 3);
    lkfree((void *) &fail, 0);
    return lkreport_wrapper("test_outfile/test_bad_free.txt", 4);
}


int test_match(){
    // Normal
    char *m = NULL;
    int ret = lkmalloc(5, (void *)&m, 0);
    if(ret != 0) return 1;
    lkfree((void *)&m, 0);

    // Unalloc free
    char *n;
    lkfree((void*)&n, 0);

    // Double malloc w/ free
    char *p = NULL;
    ret = lkmalloc(5, (void *)&p, 0);
    if(ret != 0) return 1;
    ret = lkmalloc(5, (void *)&p, 0);
    if(ret == 255) return 1;
    lkfree((void *)&p, 0);

    return lkreport_wrapper("test_outfile/test_match.txt", 2);
}

int test_serious(){
    char *p = NULL;
    int ret = lkmalloc(5, (void *)&p, 0);
    if(ret != 0) return 1;

    char *q = NULL;
    ret = lkmalloc(5, (void *)&q, 0);
    if(ret != 0) return 1;
    lkfree((void *)&q, 0);

    char *r = NULL;
    ret = lkmalloc(5, (void *)&r, 0);
    if(ret != 0) return 1;

    return lkreport_wrapper("test_outfile/test_serious.txt", 1);
}

int test_rdouble_free(){
    char *p = NULL;
    int ret = lkmalloc(5, (void *)&p, 0);
    if(ret != 0) return 1;
    lkfree((void *)&p, 0);
    lkfree((void *)&p, 0);
    return lkreport_wrapper("test_outfile/test_double_free.txt", 0x10);
}

int test_approx(){
    char *p = NULL;
    int ret = lkmalloc(5, (void *)&p, 0);
    if(ret != 0) return 1;
    p += 2;
    lkfree((void *)&p, 1);
    return lkreport_wrapper("test_outfile/test_approx.txt", 0x20);
}
