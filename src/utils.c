
#include "utils.h"
#include <stddef.h>

char *strdup(const char *src)
{
    size_t len = strlen(src) + 1;
    char *dest = malloc(len);

    if (dest != NULL)
    {
        memcpy(dest, src, len);
    }

    return dest;
}