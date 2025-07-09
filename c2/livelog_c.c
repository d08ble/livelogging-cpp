#include "../../livelogging-cpp/c2/livelog_c.h"

// ==== LiveLogBasic Implementation ====

void LiveLogBasic_init(LiveLogBasic *log, const char *filename) {
    if (!log)
        return;
        // todo: check open file and error [
        // todo: check open file and error ]
    log->file = fopen(filename, "w");
    log->stack_top = 0;
    log->path[0] = L'0';
}

void LiveLogBasic_begin(LiveLogBasic *log, const wchar_t *path) {
    if (!log || !log->file)
        return;
    if (log->stack_top < MAX_STACK_DEPTH) {
        log->stack[log->stack_top++] = wcsdup(path);
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
//    node->text[0] = L'\0';
    node->text = NULL;
    node->text_len = 0;
    node->text_alloc_size = 0;
    LCNode_put(node, L""); // for alloc
//    LCNode_put(node, L"123");
//    LCNode_put(node, L"456");
//    node->text[1] = 0;
//    LCNode_put(node, L"q");
    return node;
}

#define LCNODE_TEXT_ALLOC_SIZE 32

void LCNode_put(LCNode *node, const wchar_t *s) {
    int len = wcslen(s);
    int max = (node->text_len + len);
    if (max >= node->text_alloc_size) {
        int n = (max < LCNODE_TEXT_ALLOC_SIZE / 2) ? LCNODE_TEXT_ALLOC_SIZE : max * 2; // todo: review
        if (node->text) {
            node->text = realloc(node->text, (n + 1) * sizeof(*s));
        }
        else {
            node->text = calloc(1, (n + 1) * sizeof(*s));
        }
        if (!node->text) {
            // todo: show ERROR
            return;
        }
        node->text_alloc_size = n;
    };
    wcsncat(node->text, s, len);
    node->text_len += len;
    printf("%i/%i\n", node->text_len, node->text_alloc_size);
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
    if (!node)
        return;
    for (int i = 0; i < node->child_count; i++) {
        LCNode_destroy(node->children[i]);
    }
    free(node->name);
    free(node);
}

// ==== LiveLog Implementation ====

void LiveLog_init(LiveLog *log, const char *filename) {
    if (!log)
        return;
    LiveLogBasic_init(&log->base, filename);
    log->tree = LCNode_create(L"LiveLog++");
    strncpy(log->filename, filename, sizeof(log->filename) - 1);
}

void LiveLog_log(LiveLog *log, const wchar_t *path, const wchar_t *s) {
    if (!log)
        return;
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
    if (!log)
        return;
    if (!node) return;

    LiveLogBasic_begin(&log->base, node->name);
    LiveLogBasic_put(&log->base, node->text);

    for (int i = 0; i < node->child_count; i++) {
        LiveLog_flushNode(log, node->children[i]);
    }

    LiveLogBasic_end(&log->base);
}

void LiveLog_flush(LiveLog *log) {
    if (!log)
        return;
    if (log->base.file) {
        fclose(log->base.file);
    }
    log->base.file = fopen(log->filename, "w");

    LiveLog_flushNode(log, log->tree);

    LiveLogBasic_flush(&log->base);
}

void LiveLog_cleanup(LiveLog *log) {
    if (!log)
        return;
    LCNode_destroy(log->tree);
    LiveLogBasic_cleanup(&log->base);
}

// log int
void LiveLog_log_i(LiveLog *log, const wchar_t *path, const wchar_t *s, int v) {
    if (!log)
        return;
    wchar_t text[PATH_MAX];
	swprintf(text, sizeof(text), L"%s = %i\n", s, v);
	LiveLog_log(log, path, text);
}

// llog API

#include <pwd.h>
#include <unistd.h>  // for getuid()

void llog_init(LiveLog **llog, const char *relative_path)
{
	if (*llog) {
		// Already initialized
		return;
	}
	const char *llog_on = getenv("LLOG_OFF");
	if (llog_on != NULL) {
        // skip
        return;
    }

	*llog = calloc(1, sizeof(LiveLog));
	const char *home = getenv("HOME");
	if (home == NULL) {
		struct passwd *pw = getpwuid(getuid());
		if (pw == NULL) {
			fprintf(stderr, "LLOG: HOME environment variable not set.\n");
			return;
		}
		home = pw->pw_dir;
	}
    char full_path[PATH_MAX];

   	const char *dir = getenv("LLOG_DIR");
	if (dir != NULL) {
	    snprintf(full_path, sizeof(full_path), "%s/%s", dir, relative_path);
    }
    else {
	    snprintf(full_path, sizeof(full_path), "%s/%s", home, relative_path);
    }
	LiveLog_init(*llog, full_path);
	LiveLog_log(*llog, L"TEST", L"HELLO");
	LiveLog_flush(*llog);
}

