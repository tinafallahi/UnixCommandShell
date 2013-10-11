#include "bash.h"

int main()
{
	// Read in the profile file before starting the command line.
	// For now the PATH and HOME are printed. 
	startup();
	printf("PATH: %sHOME: %s\n", PATH, HOME);
	// Starts the command line with printing the current working 
	// directory and moving to a new line when character '\n' is 
	// reached. For the time being the line is printed. 
	char endOfLine = '\n';
	char line[128];
	while(endOfLine == '\n'){
		char cwd[1024];
        if (getcwd(cwd, sizeof cwd) != NULL) {
        	printf("%s>", cwd);
			fgets (line, sizeof line, stdin);
			endOfLine = line[strlen(line)-1];
			printf("%s", line);	
        }
        else {
        	perror("Could not get the current working directory:");
        	exit(EXIT_FAILURE);
        }
	}
    return 0;
}

// Reading the profile file and storing PATH and HOME variable.
void startup(){
	// TODO: Change the file extension.  
	char fileName[] = "profile.txt";
    char line [128];
    FILE *file;

    file = fopen(fileName,"r");
 
    if( file == NULL ) {
    	perror("Error while opening the file:\n");
    	exit(EXIT_FAILURE);
    }
 
    while( fgets (line, sizeof line, file) != NULL) {
    	if(!strncmp(line, "PATH=", 5)){
    		memmove(line, line+5, strlen(line));
    		strcpy(PATH, line);
    	}
    	if(!strncmp(line, "HOME=", 5)){
    		memmove(line, line+5, strlen(line));
    		strcpy(HOME, line);
    	}
    }

    // Checking if PATH or HOME are not defined or incorrectly defined.
    if(!strcmp(PATH, "")){
    	perror("PATH not defined correctly in profile:\n");
    	exit(EXIT_FAILURE);
    }

    if(!strcmp(HOME, "")){
    	perror("HOME not defined correctly in profile:\n");
    	exit(EXIT_FAILURE);
    }
 
    fclose(file);
}