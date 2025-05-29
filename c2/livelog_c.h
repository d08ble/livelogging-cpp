// livelog.h
#ifndef LIVELOG_H
#define LIVELOG_H

#include <stdio.h>
#include <wchar.h>
#include <stdlib.h>
#include <string.h>

#define MAX_STACK_DEPTH 100
#define MAX_PATH_LEN 256
#define MAX_CHILDREN 100
#define MAX_TEXT_SIZE 4096

//typedef struct LCNode LCNode;

typedef struct {
    FILE *file;
    wchar_t path[MAX_PATH_LEN];
    wchar_t *stack[MAX_STACK_DEPTH];
    int stack_top;
} LiveLogBasic;

typedef struct LCNode_t {
    wchar_t *name;
    struct LCNode_t *children[MAX_CHILDREN];
    int child_count;
    wchar_t text[MAX_TEXT_SIZE];
} LCNode;

typedef struct {
    LiveLogBasic base;
    LCNode *tree;
    char filename[1260];
} LiveLog;

// LiveLogBasic functions
void LiveLogBasic_init(LiveLogBasic *log, const char *filename);
void LiveLogBasic_begin(LiveLogBasic *log, const wchar_t *path);
void LiveLogBasic_end(LiveLogBasic *log);
void LiveLogBasic_put(LiveLogBasic *log, const wchar_t *s);
void LiveLogBasic_flush(LiveLogBasic *log);
void LiveLogBasic_cleanup(LiveLogBasic *log);

// LCNode functions
LCNode *LCNode_create(const wchar_t *name);
void LCNode_destroy(LCNode *node);
void LCNode_log(LCNode *node, wchar_t **path, int depth, const wchar_t *s);
LCNode *LCNode_allocNode(LCNode *node, wchar_t **path, int depth, int index);
void LCNode_put(LCNode *node, const wchar_t *s);

// LiveLog functions
void LiveLog_init(LiveLog *log, const char *filename);
void LiveLog_log(LiveLog *log, const wchar_t *path, const wchar_t *s);
void LiveLog_flush(LiveLog *log);
void LiveLog_flushNode(LiveLog *log, LCNode *node);
void LiveLog_cleanup(LiveLog *log);

#endif
