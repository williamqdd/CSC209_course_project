#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <dirent.h>
#include <poll.h>
#include <sys/wait.h>
#include <errno.h>

#include "builtins.h"
#include "io_helpers.h"
#include "variables.h"
#include "other_helpers.h"
#include "commands.h"



// ====== Command execution =====

/* Return: index of builtin or -1 if cmd doesn't match a builtin
 */
bn_ptr check_builtin(const char *cmd) {
	ssize_t cmd_num = 0;
	while (cmd_num < BUILTINS_COUNT &&
			strncmp(BUILTINS[cmd_num], cmd, MAX_STR_LEN) != 0) {
		cmd_num += 1;
	}
	return BUILTINS_FN[cmd_num];
}


// ===== Builtins =====

/* Prereq: tokens is a NULL terminated sequence of strings.
 * Return 0 on success and -1 on error ... but there are no errors on echo.
 */
ssize_t bn_echo(char **tokens) {
	ssize_t index = 1;

	if (tokens[index] != NULL) {
		// TODO:
		// Implement the echo command
	}
	while (tokens[index] != NULL) {
		// TODO:
		// Implement the echo command
		if(strncmp(tokens[index], "$", 1)==0) {
			char * msg = find_value(&front, tokens[index]+1);

			if (msg == NULL){
				display_message(tokens[index]);
				if (tokens[index+1] != NULL){
					char *space = " ";
					display_message(space);
				}
				index+=1;

			}
			if (msg != NULL){
				display_message(msg);
				if (tokens[index+1] != NULL){
					char *space = " ";
					display_message(space);
				}

				index+=1;	 
			}
		}

		else{
			display_message(tokens[index]);

			if (tokens[index+1] != NULL){
				char *space = " ";
				display_message(space);
			}
			index += 1;
		}
	}
	display_message("\n");

	return 0;
}

ssize_t bn_ls(char **tokens) {
	int recfg=0;
	int dfg=0;
	for(int i=0; tokens[i]!=NULL;i++) {
		if (strcmp(tokens[i], "--rec")==0) {
			recfg=1;
		}
		if(strcmp(tokens[i], "--d")==0){
			dfg=1;
		}
	}
	if(recfg!=dfg){
		display_error("ERROR: --rec and --d are not provided together", "");
		return -1;
	}
	struct dirent *sdir;
	DIR *dir;
	char path[MAX_STR_LEN];
	getcwd(path, sizeof(path));
	if (tokens[1] == NULL) {
		dir = opendir(path);
		while((sdir=readdir(dir))!=NULL) {
			display_message(sdir->d_name);
			display_message("\n");
		}
		closedir(dir);
		return 0;
	}
	if(strncmp(tokens[1], "$", 1)==0) {
		char *dollar = find_value(&front, tokens[1]+1);

		dir = opendir(dollar);
		if (dir == NULL){
			display_error("ERROR: Invalid path", "");
			return -1;
		}
		while((sdir=readdir(dir))!=NULL) {
			display_message(sdir->d_name);
			display_message("\n");
		}
		closedir(dir);
		return 0;
	}

	if(tokens[1]!=NULL && tokens[2] == NULL){

		dir = opendir(tokens[1]);
		if (dir == NULL){
			display_error("ERROR: Invalid path", "");
			return -1;
		}
		while((sdir=readdir(dir))!=NULL) {
			display_message(sdir->d_name);
			display_message("\n");
		}
		closedir(dir);
		return 0;

	}
	int f_index=0;
	int rec_index=0;
	int d_index=0;

	for (int i=0; tokens[i]!=NULL; i++) {
		if (strncmp(tokens[i], "--f", 3)==0){
			f_index=i+1;
		}
		if (strncmp(tokens[i], "--rec",5)==0){
			rec_index=i+1;
		}
		if(strncmp(tokens[i], "--d", 3)==0){
			d_index=i+1;
		}
	}


	if(recfg==0 && f_index!=0){
		dir = opendir(tokens[1]);
		if(dir!=NULL) {
			fsub(dir, tokens[f_index]);
			closedir(dir);
			return 0;
		}
		if(dir==NULL){
			display_error("Error: Invalid Path", "");
			return -1;
		}
		closedir(dir);
	}
	if(recfg==1 && f_index==0) {
		dir=opendir(tokens[rec_index]);
		if(dir==NULL){
			display_error("Error: Invalid Path", "");
		}
		closedir(dir);

		recdf(tokens[rec_index], atoi(tokens[d_index]));
		return 0;
	}
	if(recfg==1 && f_index!=0) {
		dir=opendir(tokens[rec_index]);
		if(dir==NULL){
			display_error("Error: Invalid Path", "");
		}
		closedir(dir);

		recdff(tokens[rec_index], atoi(tokens[d_index]), tokens[f_index]);	
		return 0;
	}

	return 0;

}

ssize_t bn_cd(char **tokens) {
	if(tokens[1] == NULL){
		chdir(getenv("HOME"));
		return 0;
	}
	else if ((strncmp(tokens[1], "..", 2) ==0) && strlen(tokens[1])>2) {
		tokens[1]+=2;
		for (int i=0; i<strlen(tokens[1]); i++) {
			if(tokens[1][i]!='.') {
				display_error("ERROR: Invalid Path", "");
				return -1;
			}
		}

		chdir("..");		
		for (int i=0; i<strlen(tokens[1]); i++) {
			chdir("..");
		}
		return 0;

	}


	else if((strncmp(tokens[1],"~",1) == 0) && strlen(tokens[1])==1) {
		chdir(getenv("HOME"));
		return 0;
	}

	int err = chdir(tokens[1]);
	if (err==0) {
		return 0;
	}
	display_error("ERROR: Invalid pathi ", "");
	return -1;
}

ssize_t bn_cat(char **tokens) {
	if( tokens[1] == NULL){
		struct pollfd fds;
		fds.fd = 0;
		fds.events = POLLIN;
		int ret = poll(&fds, 1, 20);
		if(ret==0){
			display_error("ERROR: No input source provided", "");
			return -1;
		}
		char line[MAX_STR_LEN];
		while (fgets(line, MAX_STR_LEN,stdin)!= NULL){
			display_message(line);
		}

		return 0;
	}
	FILE *f;
	f = fopen(tokens[1], "r");
	if (f == NULL) {
		display_error("ERROR: Cannot open file", "");
		return -1;
	}
	char c[2];
	c[1]='\0';
	while ((c[0]=fgetc(f))!= EOF){
		char *ch=&c[0];
		display_message(ch);
	}
	fclose(f);
	return 0;

}

ssize_t bn_wc(char **tokens) {
	int cc = 0;
	int wc = 0;
	int nc = 0;
	int flag = 0;
	char ch;
	FILE *f;
	if (tokens[1] == NULL) {
		struct pollfd fds;
		fds.fd = 0;
		fds.events = POLLIN;
		int ret = poll(&fds, 1, 20);
		if (ret==0){
			display_error("ERROR: No input source provided", "");
			return -1;

		}
		f = stdin;
	}
	if (tokens[1]!=NULL){

		f = fopen(tokens[1], "r");
		if (f == NULL) {
			display_error("ERROR: Cannot open file", "");
			return -1;
		}
	}
	while ((ch = fgetc(f)) != EOF) {
		cc++;
		if(ch == '\n' ||ch == '\t'|| ch=='\r'|| ch == ' ' ){
			if(ch == '\n'|| ch == '\0') {
				nc++;

			}
			if(flag){
				wc++;
				flag=0;
			}
		}
		else{
			flag = 1;
		}
	}
	//fclose(f);
	char ccc[20];
	sprintf(ccc, "%d", cc);
	char wcc[20];
	sprintf(wcc, "%d", wc);
	char ncc[20];
	sprintf(ncc, "%d", nc);

	display_message("word count ");
	display_message(wcc);
	display_message("\n");

	display_message("character count ");
	display_message(ccc);
	display_message("\n");

	display_message("newline count ");
	display_message(ncc);
	display_message("\n");

	return 0;

}

int pipe_exec(char *input_buf) {
	char *token_pipe_arr[MAX_STR_LEN] = {NULL};

	size_t pipe_count = tokenize_pipe(input_buf, token_pipe_arr);
	int fds[pipe_count][2], ret;

	for (int i=0;i < pipe_count;i++) {

		if ((pipe(fds[i])) == -1) {
			perror("pipe");
			exit(-1);
		}
		if ((ret=fork())<0) {
			perror("fork");
			exit(-1);
		}
		else if (ret==0) {
			for (int j= 0; j<i-1 ;j++){
				close(fds[j][0]);
				close(fds[j][1]);
			}

			if (i!=0) {
				close(fds[i-1][1]);
				dup2(fds[i-1][0], STDIN_FILENO);
			}
			if(i!=pipe_count-1){
				dup2(fds[i][1], STDOUT_FILENO);
			}



			char *token_arr[MAX_STR_LEN] = {NULL};
			char input_buf_2[MAX_STR_LEN];
			strncpy(input_buf_2, token_pipe_arr[i], MAX_STR_LEN);
			input_buf_2[MAX_STR_LEN-1]='\0';


			size_t token_count = tokenize_input(input_buf_2, token_arr);


			if (strncmp("exit", token_arr[0], 4) == 0){
				return -1;
				exit(-1);
			}

			if (token_count >= 1) {
				bn_ptr builtin_fn = check_builtin(token_arr[0]);
				if (builtin_fn != NULL) {

					ssize_t err = builtin_fn(token_arr);

					if (err == -1) {
						display_error("ERROR: Builtin failed: ", token_arr[0]);

					}
				}
				else if(builtin_fn==NULL){
					int ret = fork();
					if (ret<0) {
						perror("fork");
						exit(-1);
					}
					if (ret==0){
						int rst =execvp(token_arr[0], token_arr);
						if (rst==-1){
							display_error("ERROR: Unrecognized command: ", token_arr[0]);
						}
						exit(1);
					}
					wait(NULL);

				}

				else if((builtin_fn == NULL) && (strstr(token_arr[0], "=")!= NULL) && (token_count==1)) {
					split_assign(&front, token_arr[0]);

				}

				else {
					display_error("ERROR: Unrecognized command: ", token_arr[0]);
				}

			}


			exit(1);
		}

	}
	for(int i=0; i< pipe_count;i++){
		close(fds[i][0]);
		close(fds[i][1]);
	}
	for(int i=0; i< pipe_count;i++){
		wait(NULL);
	}
	return 1;
}


int ampersand_exec(char *input_buf, char **token_arr, int token_count) {
	for (int i=0; input_buf[i]!='\0'; i++){
		if (input_buf[i]=='&') {
			input_buf[i]= '\0';
		}
	}


	if (strchr(input_buf, '|') !=NULL) {
		pipe_exec(input_buf);
		return EXIT_SUCCESS;
	}


	if (strchr(token_arr[token_count-1], '&')!=NULL){
		if (strlen(token_arr[token_count-1])==1){
			token_arr[token_count-1]=NULL;
			token_count-=1;
		}
		else{
			token_arr[token_count-1][strlen(token_arr[token_count-1])-1] = '\0';
		}
	}

	// Command execution
	if (token_count >= 1) {
		bn_ptr builtin_fn = check_builtin(token_arr[0]);
		if (builtin_fn != NULL) {

			ssize_t err = builtin_fn(token_arr);

			if (err == -1) {
				display_error("ERROR: Builtin failed: ", token_arr[0]);

			}
		}
		else if((builtin_fn == NULL) && (strstr(token_arr[0], "=")!= NULL) && (token_count==1)) {
			split_assign(&front, token_arr[0]);

		}
		else if(builtin_fn==NULL){
			int ret = fork();
			if (ret<0) {
				perror("fork");
			}
			if (ret==0){
				int rst =execvp(token_arr[0], token_arr);
				if (rst==-1){
					display_error("ERROR: Unrecognized command: ", token_arr[0]);
				}
				exit(1);
			}
			wait(NULL);

		}

		else {
			display_error("ERROR: Unrecognized command: ", token_arr[0]);
		}

	}


	return 0;

}

ssize_t bn_ps(char **tokens) {
	Pid_table *curr = frontp;
	while (curr!=NULL){
		display_message(curr->command);
		display_message(" ");
		display_message(curr->pid_value);
		display_message("\n");
		curr = curr->next;
	}
	return 0;
}

ssize_t bn_kill(char **tokens) {
	if(tokens[1]==NULL){
		display_error("ERROR: No pid provided", "");
		return -1;
	}
	if (tokens[2]==NULL) {
		if(strncmp(tokens[1], "$", 1)==0) {
			tokens[1] = find_value(&front, tokens[1]+1);
			if (tokens[1]==NULL) {
				display_error("ERROR: Variable does not exist", "");
				return -1;
			}
		}
		int ret=kill(atoi(tokens[1]), 15);
		if (ret<0) {
			if(errno==ESRCH) {
				display_error("ERROR: The process does not exist", "");
				return -1;
			}
		}
		return 0;

	} 
	if(strncmp(tokens[1], "$", 1)==0) {
		tokens[1] = find_value(&front, tokens[1]+1);
		if (tokens[1]==NULL) {
			display_error("ERROR: Variable does not exist", "");
			return -1;
		}
	}
	if(strncmp(tokens[2], "$", 1)==0) {
		tokens[2] = find_value(&front, tokens[2]+1);
		if (tokens[2]==NULL) {
			display_error("ERROR: Variable does not exist", "");
			return -1;
		}
	}
	int ret=kill(atoi(tokens[1]), atoi(tokens[2]));
	if (ret<0) {
		if(errno==EINVAL){
			display_error("ERROR: Invalid signal specified", "");
			return -1;
		}
		if(errno==ESRCH) {
			display_error("ERROR: The process does not exist", "");
			return -1;
		}  
	}      

	return 0;
}

ssize_t bn_start_server(char **tokens) {
	if(tokens[1]==NULL) {
		display_error("ERROR: No port provided", "");
	}


	return 0;
}

ssize_t bn_close_server(char **tokens){
	return 0;
}

ssize_t bn_send(char **tokens) {
	return 0;
}

ssize_t bn_start_client(char **tokens) {
	return 0;
}
