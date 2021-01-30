#include "headers.h"
#include "color.h"

char ret[512] = { 0 };

char *get_value(const char *path, const char *key) {
    if (path == NULL || key == NULL) {
        fprintf(stderr, RED " function get_value() from %s get %s error\n" NOCOLOR , path, key);
        errno = EINVAL;
        exit(EXIT_FAILURE);
        return NULL;
    }
    FILE *fp = NULL;

    if ((fp = fopen(path, "r")) == NULL) {
        fprintf(stderr, RED " function get_value() fopen error\n" NOCOLOR );
        exit(EXIT_FAILURE);
        return NULL;
    }

    size_t nread, lsize;
    char *lcontent = NULL;

    while ((nread = getline(&lcontent, &lsize, fp)) != -1) {
        char *slocate;
        if ((slocate = strstr(lcontent, key)) == NULL) { continue; }
        if (lcontent[strlen(key)] == '=' && slocate == lcontent) {
            // printf( CYAN "find %s at %s\n", key, lcontent);

            strcpy(ret, lcontent + strlen(key) + 1);
            ret[strlen(ret) - 1] = '\0';
            free(lcontent);
            fclose(fp);
            return ret;
        }
    }

    free(lcontent);
    fclose(fp);

    return ret;
    
}
