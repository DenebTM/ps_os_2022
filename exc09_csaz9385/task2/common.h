#ifndef _COMMON_H
#define _COMMON_H

#define PORT 8000
#define MAX_NAME_LEN 32
#define BUF_SIZE 1024
#define CRLF "\r\n"
#define CTRL_C "\377\364\377\375\006"

#define BUF_MATCH(str) !strcmp(buffer, str)
#define BUF_MATCHCMD(str) !strncasecmp(buffer+1, str, strlen(str))

#define CATCH(exit_cond, ...)                       \
    do {                                            \
        if(exit_cond) {                             \
            fprintf(stderr, __VA_ARGS__);           \
            exit_cleanup(EXIT_FAILURE);             \
        }                                           \
    } while(0)

#endif
