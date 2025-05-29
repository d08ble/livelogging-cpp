#ifndef __LIVELOG_C_H__

void livelog_begin(const wchar_t *name, const char *filename);
void livelog_log(const wchar_t *path, const wchar_t *s);
void livelog_flush(bool force);

#endif
