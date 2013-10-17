#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

char PATH[5][1024] = {"","","","",""};
char HOME[1024];

void startup();
void setPathAndHome(char line[]);
int getProgramNameAndArgs(char array[]);
char* searchBinary(char *folder, char *programName);