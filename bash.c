#include "bash.h"

// TODO: In general see about replacing all the char[] with char*.
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
	char line[1024];
	while(endOfLine == '\n'){
		char cwd[1024];
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

// This function gets the program name from the command line and 
// two arguments(assuming there won't be more than two arguments).
// according to what the program name is then the correct function 
// will be called and executed. 
void getProgramNameAndArgs(char line[]) {
	int count = 0;
	char programName[80];
	char args[2][10];
	strcpy(args[0], "");
	strcpy(args[1], "");
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
    // Print program name and all the arguments.
    printf("Program name: %s\n", programName);
    int j=0;
    for(j=0;j<2;j++){
    	printf("Argument %d: %s\n", j, args[j]);
    }
    // From here according to what the program name is and what
    // the arguments are functions must be written. 
}

void setPathAndHome(char line[]){
	if(!strncmp(line, "PATH=", 5)){
    		memmove(line, line+5, strlen(line));
    		strcpy(PATH, line);
    }
    if(!strncmp(line, "HOME=", 5)){
    		memmove(line, line+5, strlen(line));
    		strcpy(HOME, line);
    }
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