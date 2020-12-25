#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>

int main() {
	char dir_name[] = "files";
	struct stat st;

	// check if directory exists, if not mkdir
	while(1) {
		if(stat(dir_name, &st) == 0)
			chdir(dir_name);
		else if(stat("transmission", &st) == 0)
			chdir("transmission");
		else 
			break;
	}
	while(1) {
		if(stat("EX2", &st) == 0) 
			break;
		if(stat(dir_name, &st) == 0) {
			rmdir(dir_name);
		}
		else if(stat("transmission", &st) == 0)
			rmdir("transmission");
		else 
			chdir("..");
	}

}