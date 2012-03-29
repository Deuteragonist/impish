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

/* If first arg is a builtin command, run it and return true */
int builtin_command(char **argv)
{
   if (!strcmp(argv[0], "quit"))        /* quit command */
      exit(0);
   if (!strcmp(argv[0], "&"))   /* Ignore singleton & */
      return 1;
   return 0;                    /* Not a builtin command */
}

/* $end eval */

/* $begin parseline */
/* parseline - Parse the command line and build the argv array */
int parseline(char *buf, char **argv)
{
   char *delim;                 /* Points to first space delimiter */
   int argc;                    /* Number of args */
   int bg;                      /* Background job? */

   buf[strlen(buf) - 1] = ' ';  /* Replace trailing '\n' with space */
   while (*buf && (*buf == ' '))        /* Ignore leading spaces */
      buf++;

   /* Build the argv list */
   argc = 0;
   while ((delim = strchr(buf, ' '))) {
      argv[argc++] = buf;
      *delim = '\0';
      buf = delim + 1;
      while (*buf && (*buf == ' '))     /* Ignore spaces */
         buf++;
   }
   argv[argc] = NULL;

   if (argc == 0)               /* Ignore blank line */
      return 1;

   /* Should the job run in the background? */
   if ((bg = (*argv[argc - 1] == '&')) != 0)
      argv[--argc] = NULL;

   return bg;
}

/* $end parseline */
