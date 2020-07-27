#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "../LKmalloc.h"
#include "lkmalloc_test.h"

int test_double_malloc(){
    char *buf = NULL;
    int ret = lkmalloc(5, (void *)&buf, 0);
    ret = lkmalloc(5, (void *)&buf, 0); // Fails does not allocate
    free(buf);
    return ret; // should be 1
}

int test_reg_malloc(){
    char *buf = NULL;
    int ret = lkmalloc(5, (void *)&buf, 0);
    free(buf);
    return ret;
}

int test_lkm_init(){
    char *buf = NULL;
    int flags = 1;
    int ret = lkmalloc(5, (void *)&buf, flags);
    if(ret != 0) return ret;
    int ctr = 0;
    printf("Buffer size = 5\n");
    while(ctr < 5){
        if(buf[ctr] != 0){
            return 1;
        }
        printf("buf[%d]=%d\n", ctr, buf[ctr]);
        ctr++;
    }
    free(buf);
    return 0;
}


int test_lkm_under(){
    char *buf = NULL;
    int flags = 4;
    int ret = lkmalloc(5, (void *)&buf, flags);
    if(ret != 0) return ret;
    int ctr = 0;
    buf -= 8;
    while(ctr < 8){
        if(buf[ctr] != 0X6b){
            return 1;
        }
        printf("buf[%d]=%d\n", ctr, buf[ctr]);
        ctr++;
    }
    free(buf);
    return 0;
}

int test_lkm_over(){
    char *buf = NULL;
    int flags = 2;
    int ret = lkmalloc(5, (void *)&buf, flags);
    if(ret != 0) return ret;
    int ctr = 0;
    buf += 5;
    while(ctr < 8){
        if(buf[ctr] != 0X5a){
            return 1;
        }
        printf("buf[%d]=%d\n", ctr, buf[ctr]);
        ctr++;
    }
    buf -= 5;
    free(buf);
    return 0;
}


int test_lkm_init_under(){
    char *buf = NULL;
    int flags = 5;
    int ret = lkmalloc(5, (void *)&buf, flags);
    if(ret != 0) return ret;
    int ctr = 0;
    buf -= 8;
    while(ctr < 8){
        if(buf[ctr] != 0X6b){
            return 1;
        }
        printf("buf[%d]=%d\n", ctr, buf[ctr]);
        ctr++;
    }
    buf += 8;
    while(ctr < 13){
        if(buf[ctr] != 0){
            return 1;
        }
        printf("buf[%d]=%d\n", ctr, buf[ctr]);
        ctr++;
    }
    return 0;
}

int test_lkm_init_over(){
    char *buf = NULL;
    int flags = 3;
    int ret = lkmalloc(5, (void *)&buf, flags);
    if(ret != 0) return ret;
    int ctr = 0;
    while(ctr < 5){
        if(buf[ctr] != 0){
            return 1;
        }
        printf("buf[%d]=%d\n", ctr, buf[ctr]);
        ctr++;
    }
    buf += 5;
    ctr = 0;
    while(ctr < 8){
        if(buf[ctr] != 0X5a){
            return 1;
        }
        printf("buf[%d]=%d\n", ctr, buf[ctr]);
        ctr++;
    }
    return 0;
}

int test_lkm_under_over(){
    char *buf = NULL;
    int flags = 6;
    int ret = lkmalloc(1, (void *)&buf, flags);
    if(ret != 0) return ret;
    int ctr = 0;
    buf -= 8;
    while(ctr < 8){
        if(buf[ctr] != 0X6b){
            return 1;
        }
        printf("buf[%d]=%d\n", ctr, buf[ctr]);
        ctr++;
    }
    ctr = 0;
    buf += 9;
    while(ctr < 8){
        if(buf[ctr] != 0X5a){
            return 1;
        }
        printf("buf[%d]=%d\n", ctr, buf[ctr]);
        ctr++;
    }
    buf -= 9;
    free(buf);
    return 0;
}

int test_lkm_init_under_over(){
    char *buf = NULL;
    int flags = 7;
    int ret = lkmalloc(1, (void *)&buf, flags);
    if(ret != 0) return ret;
    int ctr = 0;
    buf -= 8;
    while(ctr < 8){
        if(buf[ctr] != 0X6b){
            return 1;
        }
        printf("buf[%d]=%d\n", ctr, buf[ctr]);
        ctr++;
    }

    ctr = 0;
    buf += 8;
    while(ctr < 1){
        if(buf[ctr] != 0){
            return 1;
        }
        printf("buf[%d]=%d\n", ctr, buf[ctr]);
        ctr++;
    }

    ctr = 0;
    buf += 1;
    while(ctr < 8){
        if(buf[ctr] != 0X5a){
            return 1;
        }
        printf("buf[%d]=%d\n", ctr, buf[ctr]);
        ctr++;
    }
    buf -= 9;
    free(buf);
    return 0;
}
