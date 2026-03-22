#include "../../../محرك_النصوص/string.h"
#include "../التعريف_العام/التعريف_العام.h"
#include "../نظام_العرب.h"
#include "المسارات.h"





int arabfs_resolve(const char* path)
{
    if (!path || !path[0])
        return 0;

    unsigned int current = 0;
    char name[32];
    int i = 0;

    if (path[0] == '/')
        i++;

    while (1)
    {
        int j = 0;

        while (path[i] && path[i] != '/')
        {
            name[j++] = path[i++];
        }

        name[j] = 0;

        if (j > 0)
        {
            int found = -1;

            for (int k = 0; k < MAX_FILES; k++)
            {
                if (table[k].used &&
                    table[k].parent_id == current &&
                    strcmp(table[k].name, name) == 0)
                {
                    found = k;
                    break;
                }
            }

            if (found < 0)
                return -1;

            current = found;
        }

        if (!path[i])
            break;

        i++;
    }

    return current;
}

static void strcpy_local(char* dest, const char* src) {
    int i = 0;
    while (src[i]) {
        dest[i] = src[i];
        i++;
    }
    dest[i] = 0;
}

int arabfs_get_parent(const char* path, char* parent_path, char* name)
{
    int i;
    int last = -1;

    for (i = 0; path[i]; i++) {
        if (path[i] == '/')
            last = i;
    }

    if (last <= 0) {

        parent_path[0] = '/';
        parent_path[1] = 0;

        strcpy_local(name, path + (path[0] == '/' ? 1 : 0));

        return 1;
    }

    for (i = 0; i < last; i++)
        parent_path[i] = path[i];

    parent_path[last] = 0;

    strcpy_local(name, path + last + 1);

    return 1;
}
