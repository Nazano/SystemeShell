#ifndef _COMMANDES_INTERNE_H
#define _COMMANDES_INTERNE_H

#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <time.h>

#define _POSIX_SOURCE

enum
{
    NAME,
    EXEC,
    NOEXPR,
};
typedef  struct
{
    int type;
    const char *motif;
} expression;


int cp(char**);
int cp_file(char*, char*);
int cp_directory(char*, char*);
void cp_retcode_handle(const int);
int cat(const char**);
int ls(char** argv, int argc);
void ls_error();
int find(char** , int );
void find_error();
void read_dir(const char*, const expression*);
void check_file(const char*, const expression*);
int all_match(const char*, const char*);
int file_name_match(const char*, const char*);
int executable(const struct stat*);
int setMyEnv(const char**, const int);
void setMyEnv_error(const int);

char* concat(const char*, const char*);
char* concat_carac(const char, const char);
int isFile(const char*);
int isDirectory(const char*);



#endif
