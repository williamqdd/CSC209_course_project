#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <dirent.h>
#include <signal.h>
#include <sys/wait.h>

#include "builtins.h"
#include "io_helpers.h"
#include "variables.h"
#include "other_helpers.h"
#include "commands.h"



void handler(int sig) {
	display_message("\n");
}


int main(int argc, char* argv[]) {
	int pid;
	extern int proc_num_g;
	char *prompt = "mysh$ "; // TODO Step 1, Uncomment this.

	char input_buf[MAX_STR_LEN + 1];
	input_buf[MAX_STR_LEN] = '\0';
	char input_buf_copy[MAX_STR_LEN+1];
	char input_buf_copy_s[MAX_STR_LEN+1];
	char *token_arr[MAX_STR_LEN] = {NULL};

	while (1) {

		struct sigaction act;
		act.sa_handler=handler;
		act.sa_flags=0;
		sigemptyset(&act.sa_mask);
		sigaddset(&act.sa_mask, SIGINT);

		sigaction(SIGINT, &act, NULL);
		//int pid=waitpid(-1, NULL, WNOHANG);

		while((pid = waitpid(-1, NULL, WNOHANG))>0) {
			char pid_s[10];
			sprintf(pid_s, "%d", pid);
            Pid_table *curr_tmp=frontp;
			while (curr_tmp!=NULL&&strcmp(curr_tmp->pid_value, pid_s)!=0) {
				curr_tmp = curr_tmp->next;
			}
			if (curr_tmp==NULL) {
				continue;
			}
			display_message("[");
			display_message(curr_tmp->proc_num);
			display_message("]+");
            display_message("  ");
            display_message("Done  ");
			display_message(curr_tmp->name);
			display_message("\n");

			delete_pidtableNode(pid, &frontp);
		}

		// Prompt and input tokenization

		// TODO Step 2:
		// Display the prompt via the display_message function.
		display_message(prompt);


		int ret = get_input(input_buf);
		strncpy(input_buf_copy, input_buf, MAX_STR_LEN+1);
		strncpy(input_buf_copy_s, input_buf, MAX_STR_LEN+1);
		size_t token_count = tokenize_input(input_buf, token_arr);



		// background process
		if(ret!=-1&&token_count!=0&&token_arr[token_count-1][strlen(token_arr[token_count-1])-1]=='&'){
			int ret = fork();
			if (ret<0){
				perror("fork");
				exit(-1);
			}
			if (ret==0) {
				ampersand_exec(input_buf_copy, token_arr, token_count);
				exit(1);
			}
			char pnums[10];
			sprintf(pnums, "%d", proc_num_g);

			display_message("[");
			display_message(pnums);
			display_message("]");
			display_message(" ");
			char pid[10];
			sprintf(pid, "%d", ret);
			display_message(pid);
			display_message("\n");
			pid_table_insert(&frontp, input_buf_copy_s, token_arr[0], ret);
			continue;
		}
		// pipe excution
		if (strchr(input_buf_copy, '|') !=NULL) {
			int retp = pipe_exec(input_buf_copy);
			if(retp >0){
				continue;
			}
			break;
		}


		// Clean exit
		if ((ret != -1 && token_count != 0 && strncmp("exit", token_arr[0], 4) == 0 && strlen(token_arr[0]) == 4) || ret == 0) {
			Node *curr = front;
			while (curr != NULL){
				Node *tmp = curr;
				curr= curr->next;
				free(tmp->variable);
				free(tmp->value);
				free(tmp);
			}

			Pid_table *curr2=frontp;
			while(curr2!=NULL) {
				Pid_table *tmp2=curr2;
				curr2=curr2->next;
				free(tmp2->proc_num);
				free(tmp2->command);
				free(tmp2->pid_value);
				free(tmp2->name);
				free(tmp2);
			}
			break;
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

			else {
				display_error("ERROR: Unrecognized command: ", token_arr[0]);
			}
		}

	}

	return 0;
}
