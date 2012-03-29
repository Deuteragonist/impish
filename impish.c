#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <assert.h>

#define MAXARGS   128
#define MAXLINE   4096

extern char **environ;

void eval(char *cmdline);
int parseline(char *buf, char **argv);
int builtin_command(char **argv);

int main()
{
   char cmdline[MAXLINE];
   bool alive = true;

   while (alive) {
      printf("> ");

      if (fgets(cmdline, MAXLINE, stdin) == NULL) {
         if (errno) {
            int fgetsErrno = errno;
            fprintf(stderr, "fgets: %s\n", strerror(fgetsErrno));
         }
      }

      if (feof(stdin)) {
         alive = false;
      } else {
         eval(cmdline);
      }
   }
}

void eval(char *cmdline)
{
   char *argv[MAXARGS];
   char buf[MAXLINE];
   int bg;
   pid_t pid;

   strcpy(buf, cmdline);
   bg = parseline(buf, argv);

   if (argv[0] == NULL) {
      return;
   }

   if (!builtin_command(argv)) {
      switch (pid = fork()) {
      case 0:                  /* child */
         if (execve(argv[0], argv, environ) < 0) {
            printf("%s: Command not found: %s\n", argv[0], strerror(errno));

            exit(0);
         }
         break;

      case -1:
         perror("fork");
         exit(EXIT_FAILURE);
         break;

      default:
         if (!bg) {
            int status;
            if (waitpid(pid, &status, 0) < 0) {
               perror("waitpid");
            }
         } else {
            printf("%d %s", pid, cmdline);
         }
         break;
      }

   }
   return;
}

int builtin_command(char **argv)
{
   if (!strcmp(argv[0], "quit")) { 
      exit(0);
   }

   if (!strcmp(argv[0], "&")) {
      return 1;
   }

   return 0;                  
}

int parseline(char *buf, char **argv)
{
   char *delim;               
   int argc;                   
   int bg;                     

   buf[strlen(buf) - 1] = ' ';  
   while (*buf && (*buf == ' ')) {
      buf++;
   }

   argc = 0;
   while ((delim = strchr(buf, ' '))) {
      argv[argc++] = buf;
      *delim = '\0';
      buf = delim + 1;

      while (*buf && (*buf == ' '))     
         buf++;
   }
   argv[argc] = NULL;

   if (argc == 0) { 
      return 1;
   }

   if ((bg = (*argv[argc - 1] == '&')) != 0)
      argv[--argc] = NULL;

   return bg;
}
