#ifndef _COMMANDES_INTERNE_H
#define _COMMANDES_INTERNE_H

int cp(char**);
int cp_file(char*, char*);
int cp_directory(char*, char*);
void cp_retcode_handle(const int);
int cat(const char*);
void ls();
void find();

char* concat(const char*, const char*);
int isFile(const char*);
int isDirectory(const char*);

#endif