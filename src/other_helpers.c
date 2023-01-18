#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <dirent.h>

#include "builtins.h"
#include "io_helpers.h"
#include "variables.h"
#include "other_helpers.h"



void fsub(DIR *dir, char *sub) {
	struct dirent *sdir;
	while((sdir=readdir(dir))!=NULL) {
		if(strstr(sdir->d_name, sub)){
			display_message(sdir->d_name);
			display_message("\n");
		}
	}
}

void recdf(char *path, int d) {
	struct dirent *sdir;
	if (d==1){
		DIR *dir = opendir(path);
		while(((sdir=readdir(dir))!=NULL)) {
			display_message(sdir->d_name);
			display_message("\n");
		}
		closedir(dir);
		return;
	}
	DIR *dir = opendir(path);	
	while((sdir=readdir(dir))!=NULL) {
		display_message(sdir->d_name);
		display_message("\n");
		if(sdir->d_type==DT_DIR && strncmp(sdir->d_name, ".", 1)!=0 && strncmp(sdir->d_name, "..", 2)!=0){
			char path_array[MAX_STR_LEN];
			strncpy(path_array, path, strlen(path));
			path_array[strlen(path)] = '\0';
			strcat(path_array, "/");
			strncat(path_array, sdir->d_name,strlen(sdir->d_name));
			recdf(path_array, d-1);

		}
	}
	closedir(dir);

}

void recdff(char *path, int d, char *sub) {
	struct dirent *sdir;
	if (d==1){
		DIR *dir = opendir(path);
		while(((sdir=readdir(dir))!=NULL)) {
			if(strstr(sdir->d_name, sub)){
				display_message(sdir->d_name);
				display_message("\n");
			}
        closedir(dir);
		}
		return;
	}
	DIR *dir = opendir(path);
	while((sdir=readdir(dir))!=NULL) {
		if(strstr(sdir->d_name, sub)){
			display_message(sdir->d_name);
			display_message("\n");
		}
		if(sdir->d_type==DT_DIR && strncmp(sdir->d_name, ".", 1)!=0 && strncmp(sdir->d_name, "..", 2)!=0){
			char path_array[MAX_STR_LEN];
			strncpy(path_array, path, strlen(path));
			path_array[strlen(path)] = '\0';
			strcat(path_array, "/");
			strncat(path_array, sdir->d_name,strlen(sdir->d_name));
			recdff(path_array, d-1, sub);


		}
	}
	closedir(dir);


}


