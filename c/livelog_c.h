#ifndef __LIVELOG_C_H__

void livelog_init(const char *filename);
void livelog_log(const wchar_t *path, const wchar_t *s);
void livelog_flush();

#endif
