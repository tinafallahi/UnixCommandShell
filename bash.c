#include "bash.h"

// TODO: Change some of the arrays to pointers instead with malloc. 
int main()
{
	// Read in the profile file before starting the command line.
	startup();
	// Strts the command line with printing the current working 
	// directory and moving to a new line when character '\n' is 
	// reached.
	char endOfLine = '\n';
	char line[MAXLENGTH];
	while(endOfLine == '\n'){
		char cwd[MAXLENGTH];
        if (getcwd(cwd, sizeof cwd) != NULL) {
        	printf("%s>", cwd);
			fgets (line, sizeof line, stdin);
			endOfLine = line[strlen(line)-1];
			// Removing the '\n' character at the end of the line and
			// calling the function that divides and executes the 
			// corresponding action.
			line[strlen(line)-1] = '\0';
			getProgramNameAndArgs(line);	
        }
        else {
        	perror("Could not get the current working directory:");
        	exit(EXIT_FAILURE);
        }
	}

    return 0;
}

// Function that takes a line read in and changes the PATH and HOME 
// global variable according to what the content of that line is.
void setPathAndHome(char line[]){
	if(!strncmp(line, "HOME=", 5)){
    		memmove(line, line+5, strlen(line));
    		strcpy(HOME, line);
    }
	if(!strncmp(line, "PATH=", 5)){
        int i = 0;
        for(i=0;i<PATHMAX;i++) {
            strcpy(PATHS[i], "");
        }
		memmove(line, line+5, strlen(line));
        strcpy(PATH, line);
		char *token = strtok (line,":");
		int count = 0;
		while (token != NULL) {
			strcpy(PATHS[count], token);
    		token = strtok (NULL, ":");
    		count++;
    	}
    }
}

// Reading the profile file and storing PATH and HOME variable.
void startup(){
    char line [MAXLENGTH];
    FILE *file;

    file = fopen(FILENAME,"r");
 
    if( file == NULL ) {
    	perror("Error while opening the profile file:\n");
    	exit(EXIT_FAILURE);
    }
 
    while( fgets (line, sizeof line, file) != NULL) {
    	strtok(line, "\n");
    	setPathAndHome(line);
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

// Searches through a given directory and returns 
// the complete path to the file.
char* searchBinary(char *folder, char *programName){
	DIR *dir;
	struct dirent *ent;
	char *result = malloc(strlen(programName)+strlen(folder)+2);
	if ((dir = opendir (folder)) != NULL) {	
  		while ((ent = readdir (dir)) != NULL) {
  			if(!strcmp(ent->d_name, programName)) {
  				strcpy(result, folder);
    			strcat(result, "/");
    			strcat(result, programName);
    		}

  		}
  		closedir (dir);
  		return result;
	} 
	else {
		perror("Cannot open the PATH directory.");
        exit(EXIT_FAILURE);
	}
}

// Searches through all the paths given in the PATH variable for a
// binary and return the full path to that binary.
char* searchBinaries(char *programName){
	char *fullPath = "";
    int pathCount = 0;
    while(strcmp(PATHS[pathCount], "")) {
    	fullPath = searchBinary(PATHS[pathCount], programName);
    	if(strcmp(fullPath, "")){
    		break;
    	}
    	pathCount++;
    }
    return fullPath;
}

// Forks the parent process and executes the correct binary.
void executeBinary(char *filePath, char *programName, char *args[]){
    int i = 0;
    for(i=0; i<MAXARGS; i++) {
        if(!strcmp(args[i],"")){
            args[i] = NULL;
        }
    }
	pid_t pid=fork();
	if (pid==0) {
        // If MAXARGS is changed then this line needs to be modified as well.
        // TODO: Ask if I should even consider a different case!
		char *argv[]={programName,args[0],args[1],NULL};
		execv(filePath,argv);
	}
	else {
		waitpid(pid,0,0);
	}
}

// Executing cd according to what the arguments are.
void changeDirectory(char *args[]) {
    int i = 0;
    int count=0;
    for(i = 0; i<MAXARGS; i++) {
        if(!strcmp(args[i],"")){
            args[i] = NULL;
            count++;
        }
    }
    if(count == MAXARGS) {
        // If no arguments then change directory to home.
        if(chdir(HOME) == -1) {
            perror("Failed to find HOME directory:");
            exit(EXIT_FAILURE);
        }
    } else {
        // If any arguments then go to the correct directory.
        if(chdir(args[0]) == -1) {
            printf("Failed to find %s directory\n", args[0]);
        }
    }

}

// Opens the profile file and changes the PATH and HOME.
// TODO: Ask if multiple which one needs to be written.
void writeProfile(char *programName) {
    FILE *file;
    if( file == NULL ) {
        perror("Error while opening the profile file:\n");
        exit(EXIT_FAILURE);
    }
    char line [MAXLENGTH];
    file = fopen(FILENAME, "w");
    fprintf(file, "PATH=%s\nHOME=%s", PATH, HOME);
    fclose(file);
}

// This function gets the program name from the command line and 
// two arguments(if the number needs to be changed MAXARGS variable 
// can be changed). According to what the program name is then the 
// correct function will be called and executed. 
// TODO: Check why somethings don't work the first time around for fullPath.
int getProgramNameAndArgs(char line[]) {
	int count = 0;
	char programName[MAXLENGTH]="";
	char *args[MAXARGS];
    int i = 0;
    for(i=0; i<MAXARGS; i++) {
        args[i] = malloc(MAXLENGTH);
    } 
	char *token = strtok (line," ");
	while (token != NULL) {
		if(count==0) {
    		strcpy(programName, token);
    	} else {
    		strcpy(args[count-1], token);
    	}
    	token = strtok (NULL, " ");
    	count ++;
    }
    // According to what the program name is command are executed.
    // Directory needs to be changed.
    if (!strcmp(programName, "cd")) {
        changeDirectory(args);
    	return 0;
    }
    // Environmental variables need to be changed.
    if (!(strncmp(programName, "$HOME=", 6) && strncmp(programName, "$PATH=", 6))) {
    	setPathAndHome(memmove(programName, programName+1, strlen(programName)));
    	writeProfile(programName);
    	return 0;
    }
    // Checking if a binary for this program name exists.
    char *fullPath = searchBinaries(programName);
    if(strcmp(fullPath,"")){
    	executeBinary(fullPath, programName, args);
    	return 0;
    }

    printf("Command not recognised.\n"); 
}