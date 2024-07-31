#include <stdio.h>
#include <stdlib.h>

#include <sys/wait.h>
#include <sysexits.h>
#include <err.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>

#include "command.h"
#include "executor.h"

static void print_tree(struct tree *t) {
   if (t != NULL) {
      print_tree(t->left);

      if (t->conjunction == NONE) {
         printf("NONE: %s, ", t->argv[0]);
      } else {
         printf("%s, ", conj[t->conjunction]);
      }
      printf("IR: %s, ", t->input);
      printf("OR: %s\n", t->output);

      print_tree(t->right);
   }
}

int execute(struct tree *t) {   
   
   pid_t pid;
   int fd1, fd2, pipe_fd[2], status;

   
   if (t->conjunction == NONE) {

      /* These are the basic commands that we have to implement */
      if (strstr(t->argv[0], "cd")){
         chdir(t->argv[1]);
      }
      else if (strstr(t->argv[0], "exit")) {
         exit(0);
      }
      /* All other commands have to fork() and run using exec */
      else {
         if ((pid = fork()) < 0) {
            printf("Fork Error!\n");
         }
         /* Parent */
         if (pid) {
            wait(&status);
            return status; 
         }
         else {
            /* If the command has input or output, it will ass it to the file descriptor */
            if (t->input) {
               fd1 = open(t->input, O_RDONLY | O_CREAT, 0664);
               dup2(fd1, STDIN_FILENO);
               close(fd1);
            }
            if (t->output) {
               fd2 = open(t->output, O_WRONLY | O_CREAT, 0664);
               dup2(fd2, STDOUT_FILENO);
               close(fd2);
            }
            /* Runs the command, exits with -1 if command fails*/
            execvp(t->argv[0], t->argv);
            exit(-1);
         }
      }
   }

   /* If conjunction is AND, it will run left node and run right if th
      the left node is succesful*/
   else if (t->conjunction == AND) {
      if (execute(t->left) == 0){
         execute(t->right);
      }
   }

   /* If conjunction is PIPE, it will pipe output from left node to right node */
   else if (t->conjunction == PIPE) {

      if (t->left->output) {
         printf("Ambiguous output redirect.\n");
         return -1;
      }
      if (t->right->input){
         printf("Ambiguous input redirect.\n");
         return -1;
      }
      if ((pipe(pipe_fd) < 0) || (pid = fork()) < 0) {
         printf("Error Forking or Piping!\n");
      }
      /* Parent and child share PIPE, close [1] to share PIPE like from parent -> child */
      if (pid) {
         close(pipe_fd[1]);
         dup2(pipe_fd[0], STDIN_FILENO);
         execute(t->right);
         wait(NULL);
      /* child, close [0] necessary to share PIPE like from parent -> child */
      }else {
         close(pipe_fd[0]);
         dup2(pipe_fd[1], STDOUT_FILENO);
         execute(t->left);
      }
   }

   /*If conjunction is a subshell it will only run left node */
   else if(t->conjunction == SUBSHELL) {

      if ((pid = fork()) < 0) {
         printf("Fork Error!\n");
      }
      /* Parent waits for child */
      if (pid) {
         wait(NULL);
      }
      /*Executes only left node, if exec fails, exit -1*/
      else {
         execute(t->left);
         exit(-1);
      }
   }
   
   
   return 0;
}
