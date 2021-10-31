#define _GNU_SOURCE
#include <dlfcn.h>
#include <libgen.h>
#include <string.h>

typedef int (*orig_remove_f_type)(const char *pathname);

int remove(const char *pathname)
{
    char *name;

    name = basename(strdup(pathname));
    if (NULL != strstr(name, "PROTECT")) {
        return 0;
    }

    orig_remove_f_type orig_remove;
    orig_remove = (orig_remove_f_type)dlsym(RTLD_NEXT, "remove");
    return orig_remove(pathname);
}