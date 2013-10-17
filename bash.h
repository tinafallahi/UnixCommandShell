#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#define MAXARGS 2
#define MAXLENGTH 1024
#define PATHMAX 5
#define FILENAME "profile.txt"

char PATH[MAXLENGTH];
char PATHS[PATHMAX][MAXLENGTH];
char HOME[MAXLENGTH];

void setPathAndHome(char line[]);
void startup();
char* searchBinary(char *folder, char *programName);
char* searchBinaries(char *programName);
void executeBinary(char *filePath, char *programName, char *args[]);
void changeDirectory(char *args[]);
void writeProfile(char *programName);
int getProgramNameAndArgs(char array[]);