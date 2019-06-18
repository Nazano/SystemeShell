#include "commandesInterne.h"
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>

//------------------------------------------------
// Commande interne cp (copy). Attend la ligne d'argument
// correspondant à l'appel de cette commande
//------------------------------------------------
int cp(char** argv){
    if(isFile(argv[1]))
        cp_retcode_handle(
            cp_file(argv[1], argv[2]));
    else if(isDirectory(argv[1]))
        cp_retcode_handle(
            cp_directory(argv[1], argv[2]));
    return 0;
}

//------------------------------------------------
// Copie un répertoire. Format des répertoires
// atttendus sans '/' à la fin (ex: "test", non pas "test/"").
// Source est le répertoire à copier vers
// destination.
//------------------------------------------------
int cp_directory(char* source, char* destination){
    source = concat(source, "/");
    destination = concat(destination, "/");

    DIR *directory;
    struct dirent *dp;
    struct stat st = {0};
    char * file_name;

    if((directory = opendir(source)) == NULL){
         return -1;
    }

    if(stat(destination, &st) == -1){
        mkdir(destination);
    }

    while((dp = readdir(directory)) != NULL) {
        printf("debug: %s\n", dp->d_name);
        if( !(!strcmp(dp->d_name, ".") || !strcmp(dp->d_name, ".."))){
            file_name = concat(source, dp->d_name);
            printf("à copier: \"%s\"\n",file_name);
            if(isDirectory(file_name))
                cp_directory(file_name, concat(destination,  dp->d_name));
            else if(isFile(file_name))
                cp_file(file_name, concat(destination, dp->d_name));
            
        }
    }
    closedir(directory);
    return 0;
}

//---------------------------------------
// Copie de fichiers. 
// Src est le fichier à copier vers dest.
//---------------------------------------
int cp_file(char* src, char* dest){
    FILE *source, *destination;
    int c;
    if((source = fopen(src,  "r")) == NULL) {
        return -1;
    }   
    
    if((destination = fopen(dest, "w")) == NULL){
         return -2;
    }

    while((c = fgetc(source)) != EOF){
        fputc( c, destination);
    }

    fclose(source);
    fclose(destination);

    return 0;
}

//------------------------------------------------
// Concatène deux chaine de caractères.
//------------------------------------------------
char* concat(const char* s1, const char* s2)
{
    //Utiliser asprintf() si on a GNU, fais tout ça en 1 ligne
    const size_t len1 = strlen(s1);
    const size_t len2 = strlen(s2);
    char *result; 
    if((result = malloc(len1 + len2 + 1)) == NULL){ // +1 for the null-terminator
        printf("malloc failed");
    }
    strcpy(result, s1);
    strcat(result, s2);
    return result;
}

//------------------------------------------------
// Vérifie que le path coresspond bien à un fichier 
//------------------------------------------------
int isFile(const char* path){
    struct stat path_stat;
    stat(path, &path_stat);
    return S_ISREG(path_stat.st_mode);
}

//------------------------------------------------
// Vérifie que le path coresspond bien à un dossier 
//------------------------------------------------
int isDirectory(const char* path) {
   struct stat path_stat;
   stat(path, &path_stat);
   return S_ISDIR(path_stat.st_mode);
}

//------------------------------------------------
// Gère l'affichage des erreures en fonction du code retour
//------------------------------------------------
void cp_retcode_handle(const int id){
    if(id == -1)
        printf("Chemin vers source invalide\n");
    else if(id == -2)
        printf("Chemin vers destination invalide\n");
    else if(id == -3)
        printf("Nombre d'arguments invalides pour la fonction cp\n Format: cp <source> <destination>\n");
}

//------------------------------------------------
// Commande interne cat. Affiche sur l'entrée standard
// le contenu du fichier passé en argument
//------------------------------------------------
int cat(const char* path){
    FILE *source;
    int c;
    if((source = fopen(path,  "r")) == NULL) {
        return -1;
    }   

    while((c = fgetc(source)) != EOF){
        fputc( c, stdout);
    }

    fclose(source);

    return 0;
}

int ls(const char* path, const char** options, const int opt_size){
    DIR *directory;
    struct dirent *dp;
    
    int is_a = -1, is_l = -1;
    for(size_t i = 0; i<opt_size; i++){
        if(strstr(options[i], "a") != NULL)
            is_a = 0;
        else if(strstr(options[i], "l") != NULL)
            is_l = 0;
    }

    if((directory = opendir(path)) == NULL){
         return -1;
    }
    
    struct stat filestat;
    int nb_files = 0;

    while((dp = readdir(directory)) != NULL) { 
        nb_files++;
    }
    char* files_names[nb_files];


    if((directory = opendir(path)) == NULL){
         return -1;
    }

    size_t j = 0;
    if( !is_a ){ //Option -a activé
        while((dp = readdir(directory)) != NULL) { 
            if(isDirectory(dp->d_name))
                files_names[j] = concat(dp->d_name, "/");
            else
                files_names[j] = dp->d_name;
            j++;
        }
    }
    else{
        while((dp = readdir(directory)) != NULL) { 
            if( !(!strcmp(dp->d_name, ".") || !strcmp(dp->d_name, ".."))){
                if(isDirectory(dp->d_name))
                    files_names[j] = concat(dp->d_name, "/");
                else
                    files_names[j] = dp->d_name;
                j++;
            }
        }
    }


    if( !is_l ){

        printf("Protection\tTaille\tLiens\tUID\tNom fichier\n");
        for (size_t i = 0; i < nb_files; i++)
        {
            stat(files_names[i], &filestat);
            //Permissions
            printf( (S_ISDIR(filestat.st_mode)) ? "d" : "-");
            printf( (filestat.st_mode & S_IRUSR) ? "r" : "-");
            printf( (filestat.st_mode & S_IWUSR) ? "w" : "-");
            printf( (filestat.st_mode & S_IXUSR) ? "x" : "-");
            printf( (filestat.st_mode & S_IRGRP) ? "r" : "-");
            printf( (filestat.st_mode & S_IWGRP) ? "w" : "-");
            printf( (filestat.st_mode & S_IXGRP) ? "x" : "-");
            printf( (filestat.st_mode & S_IROTH) ? "r" : "-");
            printf( (filestat.st_mode & S_IWOTH) ? "w" : "-");
            printf( (filestat.st_mode & S_IXOTH) ? "x" : "-");
            printf("\t");
            printf("%ld",filestat.st_size);
            printf("\t");
            printf("%d",filestat.st_nlink);
            printf("\t");
            printf("%d",filestat.st_uid);
            printf("\t");
            printf("%s",files_names[i]);
            printf("\n");

        }
        
    }
    else
    {
        for (size_t i = 0; i < nb_files; i++)
        {
            printf("%s",files_names[i]);
            printf("\n");

        }
    }
    
    
    
    return 0;
}