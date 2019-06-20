#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <pwd.h>

#include "commandesInterne.c"

void getLine(char* line_, int* lineSz_)
{
    /*
		 * Lire une ligne (jusqu'au caractère '\n') ; la stocker dans line
		 *
		 * Pour éviter de poser une limite à la taille d'une ligne de commande,
		 * on va lire l'entrée par morceaux de BUF_SZ caractères (fonction fgets) et
		 * on va augmenter dynamiquement la taille de line (fonction realloc)
	*/
    const size_t BUF_SZ = 64;
	char buffer[BUF_SZ];
    bool eol = false;
	while (!eol)	// récupérer toute la ligne en découpant par bouts
	{
		fgets(buffer, BUF_SZ, stdin);
		size_t buf_sz = strlen(buffer); //taille effective
		if (buffer[buf_sz-1] == '\n')
		{
            if(buf_sz > 1 && buffer[buf_sz-2] == '\\')
            {
                buffer[buf_sz-2] = '\0';
                buffer[buf_sz-1] = ' ';
                buf_sz--;
            }
            else
            {
                eol = true;
            }
                

            buffer[buf_sz-1] = '\0';
            buf_sz--;
                
		}
		*lineSz_ += buf_sz;
		line_ = (char*)realloc(line_, ((*lineSz_)+1)*sizeof(char));
		strcat(line_, buffer);
	}
}

void getTokens(char** args_array, char* line, int* ntok_read, int* args_sz)
{
    // découper la ligne en tokens (fonction strtok)
	// stocker dans args_array et traiter les redirections
    char* token = strtok(line, " "); // premier token
		
	char *ifile = NULL, *ofile = NULL; // fichiers de redirection (entrée, sortie)
		
	while (token != NULL)
	{
		// Redirections ?
		if (!strcmp(token,"<") || !strcmp(token,">"))
		{
			char redirect = token[0];
			token = strtok(NULL, " "); //nom du fichier (token suivant)

			if (redirect=='<')
				ifile=token; // redirection d'entrée
			else
				ofile=token; // redirection de sortie
		}
		else
		{
			if (*ntok_read >= *args_sz) //pas assez de mémoire allouée pour args_array
			{
				*args_sz = 2*(*args_sz); // doubler la taille de args_array
				args_array = (char **)realloc(args_array, (*args_sz)*sizeof(char*));
			}
			args_array[*ntok_read] = token;
			++(*ntok_read);
		}
		token = strtok(NULL, " "); // token suivant
	}
	// stocker le NULL final dans args_array
	if (*ntok_read >= *args_sz)
		args_array = (char **)realloc(args_array, ((*args_sz)+1)*sizeof(char*));
	args_array[*ntok_read] = NULL; // marqueur pour execv pour savoir où terminer

}

int main(int argc, char** argv)
{
	
	const char *invite = "> ";

    char login[64];
    char cwd[64];
    char host[64];

    strcpy(login, (*getpwuid(getuid())).pw_name);
    gethostname(host, sizeof(host));
    getcwd(cwd, sizeof(cwd));
	
	
	char* opt[] = {};
		size_t s = 0;
		ls(argv[1], opt, s);
	//while (true)
	//{
		// Afficher invite
		/*printf("%s@%s:%s%s", login, host, cwd, invite);
        
		char* line = (char *)calloc(1, sizeof(char)); // calloc alloue une case mémoire contenant '\0'
		int line_sz = 0; // taille actuelle de line (sans le '\0' final)
		
        getLine(line, &line_sz); // Récupère la ligne

        int ntok_read = 0, args_sz = 10;
		char** args_array = (char **)calloc(10, sizeof(char*));
	
		getTokens(args_array, line, &ntok_read, &args_sz); // Sépare la ligne en tokens
        

        printf("END TOKENS:\n");*/
		find(argv, argc);
    //}
}
