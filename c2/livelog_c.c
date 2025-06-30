#include "../../livelogging-cpp/c2/livelog_c.h"

// ==== LiveLogBasic Implementation ====

void LiveLogBasic_init(LiveLogBasic *log, const char *filename) {
    if (!log)
        return;
    log->file = fopen(filename, "w");
    log->stack_top = 0;
    log->path[0] = L'\0';
}

void LiveLogBasic_begin(LiveLogBasic *log, const wchar_t *path) {
    if (!log || !log->file)
        return;
    if (log->stack_top < MAX_STACK_DEPTH) {
        log->stack[log->stack_top++] = wcsdup(log->path);
    }
    wcsncpy(log->path, path, MAX_PATH_LEN);
    fwprintf(log->file, L"# %ls[\n", log->path);
}

void LiveLogBasic_end(LiveLogBasic *log) {
    if (!log || !log->file)
        return;
    fwprintf(log->file, L"# %ls]\n", log->path);
    log->path[0] = L'\0';

    if (log->stack_top > 0) {
        wcscpy(log->path, log->stack[--log->stack_top]);
        free(log->stack[log->stack_top]);
    }
}

void LiveLogBasic_put(LiveLogBasic *log, const wchar_t *s) {
    if (!log || !log->file)
        return;
    fwprintf(log->file, L"%ls\n", s);
}

void LiveLogBasic_flush(LiveLogBasic *log) {
    if (!log || !log->file)
        return;
    fflush(log->file);
}

void LiveLogBasic_cleanup(LiveLogBasic *log) {
    if (!log)
        return;
    for (int i = 0; i < log->stack_top; i++) {
        free(log->stack[i]);
    }
    if (log->file) fclose(log->file);
}

// ==== LCNode Implementation ====

LCNode *LCNode_create(const wchar_t *name) {
    LCNode *node = (LCNode *)calloc(1, sizeof(LCNode));
    node->name = wcsdup(name);
    node->text[0] = L'\0';
    return node;
}

void LCNode_put(LCNode *node, const wchar_t *s) {
    wcsncat(node->text, s, MAX_TEXT_SIZE - wcslen(node->text) - 1);
}

void LCNode_log(LCNode *node, wchar_t **path, int depth, const wchar_t *s) {
    LCNode *target = LCNode_allocNode(node, path, depth, 0);
    if (target) {
        LCNode_put(target, s);
    }
}

LCNode *LCNode_allocNode(LCNode *node, wchar_t **path, int depth, int index) {
    if (index >= depth) return node;

    for (int i = 0; i < node->child_count; i++) {
        if (wcscmp(node->children[i]->name, path[index]) == 0) {
            return LCNode_allocNode(node->children[i], path, depth, index + 1);
        }
    }

    if (node->child_count < MAX_CHILDREN) {
        LCNode *child = LCNode_create(path[index]);
        node->children[node->child_count++] = child;
        return LCNode_allocNode(child, path, depth, index + 1);
    }

    return NULL;
}

void LCNode_destroy(LCNode *node) {
    for (int i = 0; i < node->child_count; i++) {
        LCNode_destroy(node->children[i]);
    }
    free(node->name);
    free(node);
}

// ==== LiveLog Implementation ====

void LiveLog_init(LiveLog *log, const char *filename) {
    LiveLogBasic_init(&log->base, filename);
    log->tree = LCNode_create(L"LiveLog++");
    strncpy(log->filename, filename, sizeof(log->filename) - 1);
}

void LiveLog_log(LiveLog *log, const wchar_t *path, const wchar_t *s) {
    wchar_t *tokens[MAX_STACK_DEPTH];
    int depth = 0;
    wchar_t *str = wcsdup(path);
    wchar_t *token = wcstok(str, L"/", &str);
    while (token && depth < MAX_STACK_DEPTH) {
        tokens[depth++] = token;
        token = wcstok(NULL, L"/", &str);
    }
    LCNode_log(log->tree, tokens, depth, s);
    free(str);
}

void LiveLog_flushNode(LiveLog *log, LCNode *node) {
    if (!node) return;

    LiveLogBasic_begin(&log->base, node->name);
    LiveLogBasic_put(&log->base, node->text);

    for (int i = 0; i < node->child_count; i++) {
        LiveLog_flushNode(log, node->children[i]);
    }

    LiveLogBasic_end(&log->base);
}

void LiveLog_flush(LiveLog *log) {
    if (log->base.file) {
        fclose(log->base.file);
    }
    log->base.file = fopen(log->filename, "w");

    LiveLog_flushNode(log, log->tree);

    LiveLogBasic_flush(&log->base);
}

void LiveLog_cleanup(LiveLog *log) {
    LCNode_destroy(log->tree);
    LiveLogBasic_cleanup(&log->base);
}
