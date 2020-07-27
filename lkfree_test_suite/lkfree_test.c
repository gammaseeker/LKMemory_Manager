#include <stdio.h>
#include <stdlib.h>
#include "../LKmalloc.h"
#include "lkfree_test.h"

int test_double_free(){
    char *buf = NULL;
    int ret = lkmalloc(1, (void *)&buf, 0);
    if(ret != 0) return 1;
    ret = lkfree((void *)&buf, 0);
    ret = lkfree((void *)&buf, 0);
    return ret; // should be 1
}

int test_reg_free(){
    char *buf = NULL;
    int ret = lkmalloc(1, (void *)&buf, 0);
    if(ret != 0) return 1;
    ret = lkfree((void *)&buf, 0);
    return ret;
}

int test_lkf_null(){
    char *buf = NULL;
    int ret = lkfree((void *)&buf, 0);
    return ret;
}

int test_lkf_unalloc(){
    char *buf;
    int ret = lkfree((void *)&buf, 0);
    return ret;
}

int test_lkf_approx_pass(){
    char *buf = NULL;
    int ret = lkmalloc(5, (void *)&buf, 0);
    if(ret != 0) return 1;
    buf += 2;
    ret = lkfree((void *)&buf, 1);
    return ret;
}

int test_lkf_approx_fail(){
    char *buf = NULL;
    int ret = lkmalloc(5, (void *)&buf, 0);
    if(ret != 0) return 1;
    buf += 2;
    ret = lkfree((void *)&buf, 0);
    return ret;
}

int test_lkf_warn(){
    char *buf = NULL;
    int flags = 3;
    int ret = lkmalloc(5, (void *)&buf, 0);
    if(ret != 0) return 1;
    buf += 2;
    ret = lkfree((void *)&buf, flags);
    return ret;
}

int test_lkf_unknown(){
    char *buf;
    int flags = 4;
    int ret = lkfree((void *)&buf, flags);
    return ret;
}

int test_lkf_warn_error(){
    char *buf = NULL;
    int flags = 11;
    int ret = lkmalloc(5, (void *)&buf, 0);
    if(ret != 0) return 1;
    buf += 2;
    ret = lkfree((void *)&buf, flags);
    return ret;
}

int test_lkf_unknown_error(){
    char *buf;
    int flags = 12;
    int ret = lkfree((void *)&buf, flags);
    return ret;
}
