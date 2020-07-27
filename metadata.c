#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <errno.h>
#include "metadata.h"

struct metadata *md_init(char *filename, int line, const char *fxn_name, 
    void *ret_ptr, void *arg_ptr, clock_t time, int alloc_free, int retval,
    int error, u_int size, int flag){
    struct metadata *md = malloc(sizeof(struct metadata));
    if(errno == ENOMEM) return NULL;
    md->filename = malloc(strlen(filename));
    md->fxn_name = malloc(strlen(fxn_name));
    md->line = line;
    md->ret_ptr = ret_ptr;
    md->arg_ptr = arg_ptr;
    md->time = time;
    md->alloc_free = alloc_free;
    md->retval = retval;
    md->error = error;
    md->size = size;
    md->flag = flag;
    md->next = NULL;
    strcpy(md->filename, filename);
    strcpy(md->fxn_name, fxn_name);

    return md;
}

int md_insert(struct metadata **head, struct metadata *node){
    if(node == NULL) return 1;

    if(*head == 0){
        *head = node;
        return 0;
    }
    struct metadata *cursor = *head;
    while(cursor->next != NULL){
        cursor = cursor->next;
    }
    cursor->next = node;
    return 0;
}

int md_cleanup(struct metadata **head){
    struct metadata *temp_head = *head;
    struct metadata *temp_next = (*head)->next;
    
    while(temp_next != NULL){
        free(temp_head->filename);
        free(temp_head->fxn_name);
        free(temp_head);
        temp_head = temp_next;
        temp_next = temp_next->next;
    }
    free(temp_head->filename);
    free(temp_head->fxn_name);
    free(temp_head);
    return 0;
}

unsigned int get_digits(long num){
    int ctr = 0;
    while(num != 0){
        num /= 10;
        ctr++;
    }
    return ctr;
}

char *md_to_string(struct metadata *md){
    unsigned int line_digits = get_digits((long)md->line);
    unsigned int time_digits = get_digits(md->time);
    char *record = malloc(8 + 1 + 1 + 16 + 16 + strlen(md->filename) 
        + strlen(md->fxn_name) + line_digits + time_digits); // 8 commas, 1 for type, 1 for \n, 16 and 16 for ret and arg ptrs

    // Inverse values by accident. Too deep in hw to change
    int record_type;
    if(md->alloc_free == 0){ // free record
        record_type = 1;
    }else{
        record_type = 0; // malloc record
    }
    
    char type[2];
    char arg_ptr[17];
    char ret_ptr[17];
    char time[time_digits + 1];
    char line[line_digits + 1];
    char retval[2];
    sprintf(type, "%d", record_type);
    sprintf(arg_ptr,"%p", md->arg_ptr);
    sprintf(ret_ptr, "%p", md->ret_ptr);
    sprintf(time, "%ld", md->time);
    sprintf(line, "%d", md->line);
    sprintf(retval, "%d", md->retval);
    strcat(record, type);
    strcat(record, ",");
    strcat(record, md->filename);
    strcat(record, ",");
    strcat(record, md->fxn_name);
    strcat(record, ",");
    strcat(record, line);
    strcat(record, ",");
    strcat(record, time);
    strcat(record, ",");
    strcat(record, arg_ptr);
    strcat(record, ",");
    strcat(record, retval);
    strcat(record, ",");
    if(md->alloc_free == 0){
        char flags[5];
        sprintf(flags, "%d", md->flag);
        strcat(record, flags);
    }else{
        unsigned int size_digits = get_digits(md->size);
        char size[size_digits + 1];
        sprintf(size, "%d", md->size);
        strcat(record, size);
    }
    strcat(record, ",");
    strcat(record, ret_ptr);
    strcat(record, "\n");
    return record;
}

// Debug function
void md_print(struct metadata **head){
    struct metadata *cursor = *head;
    int index = 0;
    while(cursor != NULL){
        printf("index: %d\n", index);
        printf("filename: %s\n", cursor->filename);
        printf("line: %d\n", cursor->line);
        printf("fxn_name: %s\n", cursor->fxn_name);
        printf("ret_ptr: %p\n", cursor->ret_ptr);
        printf("arg_ptr: %p\n", cursor->arg_ptr);
        printf("time: %ld\n", cursor->time);
        printf("alloc_free: %d\n", cursor->alloc_free);
        printf("retval: %d\n", cursor->retval);
        printf("error: %d\n", cursor->error);
        printf("size: %d\n", cursor->size);
        printf("flag: %d\n", cursor->flag);
        printf("----------------------------------------------\n");
        cursor = cursor->next;
        index++;
    }
}
