#include "commandesInterne.h"



//------------------------------------------------
// Commande interne cp (copy). Attend la ligne d'argument
// correspondant à l'appel de cette commande
//------------------------------------------------
int cp(char** argv){
    if(isFile(argv[1]))
        cp_file(argv[1], argv[2]);
    else if(isDirectory(argv[1]))
        cp_directory(argv[1], argv[2]);
    exit(0);
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
        cp_retcode_handle(-1);
    }

    if(stat(destination, &st) == -1){
        mkdir(destination, 0700);
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
        cp_retcode_handle(-1);
    }   
    
    if((destination = fopen(dest, "w")) == NULL){
        cp_retcode_handle(-2);
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
// Concatène deux caractères en une chaine de caractères.
//------------------------------------------------
char* concat_carac(const char s1, const char s2)
{
    char *str=malloc(2*sizeof(char));
    if(str==NULL)
        return NULL;
    str[0]=s1;
    str[1]=s2;
    return str;
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
    exit(0);
}

//------------------------------------------------
// Commande interne cat. Affiche sur l'entrée standard
// le contenu du fichier passé en argument.
//------------------------------------------------
int cat(const char** argv){
    FILE *source;
    int c;
    if((source = fopen(argv[1],  "r")) == NULL) {
        cp_retcode_handle(-1);
    }   

    while((c = fgetc(source)) != EOF){
        fputc( c, stdout);
    }

    fclose(source);

    exit(0);
}

//------------------------------------------------
// Commande interne ls. Affiche le contenu du répertoire
// donné. path : chemin vers le fichier, options : a et
// l supportés, opt_size : nombre d'options
//------------------------------------------------
int ls(char** argv, int argc){

    char* path = argv[1];
    int opt_size = 0;

    if(argc < 2)
        ls_error();
    
    if(argc > 2){
        if(argv[2][0] == '-'){
            while (argv[2][opt_size + 1] != '\0') {
                opt_size++;
            }
        }
        else{
            ls_error();
        }
    }
    
    char* options[opt_size];
    
    for (size_t i = 1; i < (opt_size + 1); i++) {
        options[i-1] = concat_carac(argv[2][i], '\0');
    }
    
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
    
    exit(0);
}

//------------------------------------------------
// Message d'erreur ls
//------------------------------------------------
void ls_error(){
    printf("ls <path> <options>\n Les options doivent être de la forme (-a, -l, -al, -la)\n");
    exit(1);
}

//------------------------------------------------
// Commande interne find. Appel find <path> <expression>
// Supporte -name et -exec. find <path> <pattern> trouve
// toutes les noms de fichiers contenant le pattern dans leur
// nom.
// Si -name ou -exec utilisé trouve la première occurence.
//------------------------------------------------

int find(char** argv, int argc){
    if(argc < 2)
        find_error();
    
    int definedExpression = 0;
    int i = 1;
    char *path;
    expression exp;
    
    path = argv[i];
        
    i++;

    while(i < argc){
        if(argv[i][0] == '-' && !definedExpression){
            if(strcmp(argv[i]+1, "name") == 0)
                exp.type = NAME;
            else if(strcmp(argv[i]+1, "executable") == 0)
                exp.type = EXEC;
          
            definedExpression = 1;
        } else if (definedExpression){
            switch(exp.type){
                case NAME:
                    exp.motif = argv[i];
                    break;
                default:
                    find_error();
                    break;
            }
        } else {
            exp.type = NOEXPR;
            exp.motif = argv[i];
        }
        i++;
    }
    read_dir(path, &exp);
    
    exit(0);
	
}

//------------------------------------------------
// Message d'erreur pour la commande find
//------------------------------------------------
void find_error(){
    printf("find <path> <expression>\nexpression :\n-name <name>\n-exec\n");
    exit(1);
}

//------------------------------------------------
// Parcours de répertoire récursif. Traite chaque élément.
// Si c'est un fichier apelle traiter_fichier(),
// sinon s'apelle sur le répertoire.
//------------------------------------------------
void read_dir(const char* path, const expression* exp){
    DIR *dir = opendir(path);
    struct dirent *entry;
    struct stat dir_stat;
    
    if (!dir) {
        perror(path);
        return;
    }
    
    while((entry = readdir(dir)) != NULL){
        char buff[strlen(path)+strlen(entry->d_name)+2];
        sprintf(buff,"%s/%s",path,entry->d_name);
        stat(buff, &dir_stat);
        
        if(strcmp(entry->d_name,"..") != 0){
            if(((dir_stat.st_mode & S_IFMT) == S_IFDIR)  && strcmp(entry->d_name, ".") != 0)
                read_dir(buff, exp);
            else
                check_file(buff, exp);
        }
    }
    closedir(dir);
}


//------------------------------------------------
// Traite un fichier passé en paramètre. L'évalue en
// fonction de l'expression associée.
//------------------------------------------------

void check_file(const char* path, const expression* exp){
    struct stat file;
    stat(path, &file);
    
    switch(exp->type){
        case NAME:
            if(file_name_match(path,exp->motif))
                printf("%s\n", path);
            break;
        case EXEC:
            if(executable(&file))
                printf("%s\n", path);
            break;
        case NOEXPR:
            if(all_match(path, exp->motif))
                printf("%s\n", path);
            break;
    }
}

//------------------------------------------------
// Aucune expression passée en argument. Retourne vrai
// si le nom du fichier est contenu dans le pattern.
//------------------------------------------------
int all_match(const char* path, const char* pattern){
    return strstr(strrchr(path,'/')+1, pattern) != NULL;
}

//------------------------------------------------
// -name appelé. Affichie retourne vrai si le nom
// du fichier correspond exactement au pattern.
//------------------------------------------------
int file_name_match(const char* path, const char* pattern){
    return strcmp(strrchr(path,'/')+1,pattern) == 0;
}

//------------------------------------------------
// -exec appelé. Appelle find avec le résultat de
// l'expression.
//------------------------------------------------
int executable(const struct stat* buf){
    return ((buf->st_mode & S_IXUSR) == S_IXUSR) || ((buf->st_mode & S_IXGRP) == S_IXGRP) || ((buf->st_mode & S_IXOTH) == S_IXOTH);
}

int setMyEnv(const char** argv, const int argc){
    if(argc != 3)
        setMyEnv_error(-1);
    
    if(setenv(argv[1], argv[2], 1)){
        setMyEnv_error(-2);
    }
    
    exit(0);
        
}

void setMyEnv_error(const int error_code){
    switch(error_code){
        case -1:
            printf("setenv <var_name> <new_value>\n");
            break;
        case -2:
            printf("Erreur d'attribution de valeur pour la vraiable d'environnement");
            break;
    }
    
    exit(0);
}
