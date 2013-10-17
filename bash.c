#include "bash.h"

int main()
{
	// Read in the profile file before starting the command line.
	// For now the PATH and HOME are printed. 
	startup();
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

// Function that takes a line read in and changes the PATH and HOME 
// global variable according to what the content of that line is.
void setPathAndHome(char line[]){
	if(!strncmp(line, "HOME=", 5)){
    		memmove(line, line+5, strlen(line));
    		strcpy(HOME, line);
    }
	if(!strncmp(line, "PATH=", 5)){
		memmove(line, line+5, strlen(line));
		char *token = strtok (line,":");
		int count = 0;
		while (token != NULL) {
			strcpy(PATH[count], token);
    		token = strtok (NULL, ":");
    		count++;
    	}
    }
    printf("HOME: %s\n", HOME);
    printf("PATH1:%s PATH2:%s PATH3:%s\n", PATH[0], PATH[1], PATH[2]);
}

// Reading the profile file and storing PATH and HOME variable.
void startup(){
	// TODO: Change the file extension.  
	char fileName[] = "profile.txt";
    char line [1024];
    FILE *file;

    file = fopen(fileName,"r");
 
    if( file == NULL ) {
    	perror("Error while opening the file:\n");
    	exit(EXIT_FAILURE);
    }
 
    while( fgets (line, sizeof line, file) != NULL) {
    	strtok(line, "\n");
    	setPathAndHome(line);
    }

    // Checking if PATH or HOME are not defined or incorrectly defined.
    if(!strcmp(PATH[0], "")){
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
	//struct stat statbuf;
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
		printf("Cannot open the PATH directory %s.", folder);
	}
}

// Searches through all the paths given in the PATH variable for a
// binary and return the full path to that binary.
char* searchBinaries(char *programName){
	char *fullPath = "";
    int pathCount = 0;
    while(strcmp(PATH[pathCount], "")) {
    	fullPath = searchBinary(PATH[pathCount], programName);
    	if(strcmp(fullPath, "")){
    		break;
    	}
    	pathCount++;
    }
    return fullPath;
}

// Forks the parent process and executes the correct binary.
// TODO: Make arguments work.
void executeBinary(char *filePath, char *programName, char *args[]){
    printf("Executing: %s\n", programName);
    //printf("%s\n", args[0]);
    //printf("%s\n", args[1]);
	pid_t pid=fork();
	if (pid==0) {
		char *argv[]={programName,args[0],args[1],NULL};
		execv(filePath,argv);
	}
	else {
		waitpid(pid,0,0);
	}
}

// This function gets the program name from the command line and 
// two arguments(assuming there won't be more than two arguments).
// according to what the program name is then the correct function 
// will be called and executed. 
// For now prints which one of the three states it enters.
// TODO: Check why somethings don't work the first time around for fullPath.
int getProgramNameAndArgs(char line[]) {
	int count = 0;
	char programName[80]="";
	char args[][80]={"","",""};
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
    for(j=0;j<3;j++){
    	printf("Argument %d: %s\n", j, args[j]);
    }
    // According to what the program name is command are executed.
    // Directory needs to be changed.
    if (!strcmp(programName, "cd")) {
    	printf("Going into cd.\n");
    	return 0;
    }
    // Environmental variables need to be changed.
    if (!(strncmp(programName, "$HOME=", 6) && strncmp(programName, "$PATH=", 6))) {
    	printf("Going into changing environmental variables.\n");
    	//setPathAndHome(memmove(programName, programName+1, strlen(programName)));
    	// Also need to write into the profile file.
    	return 0;
    }
    // Checking if a binary for this program name exists.
    char *fullPath = searchBinaries(programName);
    if(strcmp(fullPath,"")){
    	printf("In binary execution for: %s\n", fullPath);
    	executeBinary(fullPath, programName, args);
    	return 0;
    }

    printf("Command not recognised.\n"); 
}