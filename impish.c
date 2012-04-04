/* 
 * CMPSC 311, Spring 2012, Project 7
 *
 * Author: Max DeLiso
 * Email: jmd5473@psu.edu, maxdeliso@gmail.com
 * File: impish.c
 * Repository: https://github.com/maxdeliso/impish
 * License:
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING 
 * BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND 
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, 
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, 
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include <stdarg.h>
#include <ctype.h>

#include <errno.h>
#include <signal.h>
#include <assert.h>

#include <readline/readline.h>
#include <readline/history.h>

#include "impish.h"
#include "util.h"
#include "sighandlers.h"

/* read-only module-only globals */
static const char DEFAULT_PROMPT[] = ">> ";
static const char OPT_STRING[] = "hvVies:";
static const char USAGE_STRING[] =
      "Usage:  impish [-h] [-v] [-V] [-i] [-e] [-s f] [file]\n"
            " -h     help\n"
            " -v     verbose mode\n"
            " -V     print version information and exit"
            " -i     interactive mode\n"
            " -e     echo commands before execution\n"
            " -s f   use startup file f, default impish.init\n"
            " Shell commands:\n" "  help \n";

/* TODO: rework the above declaration to omit whitespace
 * and line continuation, making the appropriate changes
 * where it gets passed to printf */

/* read/write globals */bool verbose;

/* read-only globals */
const int IMPISH_VERSION_NUMS[] = { 0, 0, 4 };

/* functions prototyped in impish.h */
void printVersion() {
   printf("impish v%d.%d.%d\ncompiled on %s at %s\n",
         IMPISH_VERSION_NUMS[0],
         IMPISH_VERSION_NUMS[1],
         IMPISH_VERSION_NUMS[2], __DATE__, __TIME__);

   printf("C standard supported:  %zd\n", __STDC_VERSION__);
}

/* functions defined in this file */
void processArgs(int argc, char * const argv[]);
void eval(const char * const cmdline);
void parseLine(const char * const buf, int *argcPtr, char ***argvPtr);
bool builtinCommand(const char * const * const argv);
void installSignalHandlers();
void exitHandler();

/* entry point */
int main(int argc, char *argv[])
{
   /* initialization */
   processArgs(argc, argv);
   installSignalHandlers();

   if (atexit(exitHandler) != 0) {
      printf("warning: failed to install exit handler\n");
   }

   /* main loop: read - eval - print */
   char *cmdline;
   do {
      cmdline = readline(DEFAULT_PROMPT);

      if (cmdline) {
         eval(cmdline);
         impishFree(cmdline);
      }
   } while (cmdline && !feof(stdin));

   /* TODO: cleanup? */

   return EXIT_SUCCESS;
}

void processArgs(int argc, char *const argv[])
{
   int opt;

   /* Option parsing loop */
   while ((opt = getopt(argc, argv, OPT_STRING)) != -1) {
      switch (opt) {
      case 'h':
         printf("%s", USAGE_STRING);
         exit(EXIT_SUCCESS);

      case 'v':
         verbose = true;
         break;

      case 'V':
         printVersion();
         exit(EXIT_SUCCESS);

      case 'i':
      case 'e':
      case 's':
         fprintf(stderr, "warning: flag unimplemented");
         /* TODO: set a flag */
         break;

      default:
         fprintf(stderr, "%s", USAGE_STRING);
         exit(EXIT_FAILURE);
      }
   }

   while (optind < argc) {
      printf("warning: ignoring additional argument: %s ", argv[optind++]);
   }
}

void eval(const char *const cmdline)
{
   pid_t pid;
   int argc = 0;
   char **argv = NULL;
   int status;

   parseLine(cmdline, &argc, &argv);

   if (argv == NULL || argv[0] == NULL) {
      impishFree(argv);
      return;
   }

   if (builtinCommand((const char * const * const ) argv)) {
      return;
   }

   /* TODO: handle forking to the background */
   switch (pid = fork()) {
   case -1:
      perror("fork");
      exit(EXIT_FAILURE);

   case 0:
      if (execvp(argv[0], argv) == -1) {
         fprintf(stderr, "%s: Command execution failed: %s\n", argv[0],
               strerror(errno));

         /* TODO: return something different here */
         exit(EXIT_SUCCESS);
      } else {
         fprintf(stderr, "warning: strange execvp() behavior");
         break;
      }

   default:
      /* Interact with the child via signals */
      do {
         int w = waitpid(pid, &status, WUNTRACED | WCONTINUED);

         /* Check for error condition */
         if (w == -1) {
            perror("waitpid");
            exit(EXIT_FAILURE);
         }

         /* Print messages back based on the status */
         if (WIFEXITED(status) && status != EXIT_SUCCESS) {
            printf("warning: child exited abnormally with status = %d\n",
                  WEXITSTATUS(status));

         } else if (WIFSIGNALED(status)) {
            printf("warning: child killed by signal = %d\n", WTERMSIG(status));

         } else if (WIFSTOPPED(status)) {
            printf("warning: child stopped by signal = %d\n", WSTOPSIG(status));

         } else if (WIFCONTINUED(status)) {
            printf("child continued\n");

         }
      } while (!WIFEXITED(status) && !WIFSIGNALED(status));

      break;
   }

   /* free all of the memory allocated by parseLine */
   for (int i = 0; i < argc; ++i) {
      impishFree(argv[i]);
   }

   impishFree(argv);
}

/* builtinCommand(..) returns true if the command was built in,
 * but should also be communicating some other data back to the
 * caller, i.e. whether the process should be wait()ed for or not */

/* TODO: encode data which needs to be communicated to
 * the fork-exec stanza (see above comment) */
bool builtinCommand(const char *const *const argv)
{
   if (!strcmp(argv[0], "quit") || !strcmp(argv[0],"exit")) {
      exit(EXIT_SUCCESS);
   }

   if (!strcmp(argv[0], "&")) {
      return true;
   }

   return false;
}

void parseLine(const char *const buf, int *argcPtr, char ***argvPtr)
{
   static const int ARGV_INIT_CAPACITY = 16;

   int argc = 0;
   int argvCap = ARGV_INIT_CAPACITY;
   char const *cpos = NULL;     /* chars that cpos point to are read only */
   char **argv = impishMalloc(argvCap * sizeof(void *));
   int bufStart;

   /* skip leading whitespace */
   for (bufStart = 0; isspace(buf[bufStart]); ++bufStart) ;

   /* bind offset plus start buffer to form effective buffer,
    * this is needed due to the fact that buf is const qualified */
   const char *const ebuf = buf + bufStart;
   const int ebufSize = strlen(ebuf);

   /* extract first token into argv[0] */
   if ((cpos = strchr(ebuf, ' '))) {
      impishMessage("calculated length of argv[0]: %i\n", (int)(cpos - ebuf));
      argv[0] = impishStrndup(ebuf, cpos - ebuf);
      impishMessage("actual length of argv[0]: %i\n", strlen(argv[0]));
   } else {
      argv[0] = impishStrdup(ebuf);
      impishMessage("length of argv[0]: %i\n", strlen(argv[0]));
   }

   /* extract the rest of the tokens into argv[i] for i >= 1 */
   argc = 1;
   cpos = ebuf;
   while ((cpos = strchr(cpos, ' '))) {

      /* skip to the next non-space */
      while (isspace(*cpos)) {
         ++cpos;
      }

      /* check to see if we've skipped to the end */
      if (cpos - ebuf == ebufSize) {
         impishMessage("detected trailing whitespace... continuing\n");
         continue;
      }

      /* compute the length of the current token */
      /*   find the next space after the current position */
      const char const *const npos = strchr(cpos, ' ');

      /*   if there is a next space, the length is equal to the difference
       *   between the next space and the current space,
       *   otherwise this is the last argument so it's length is the 
       *   size of the buffer minus the difference between the current
       *   position and the start */
      const int clen = (npos) ? (npos - cpos) : ((int)ebufSize - (cpos - ebuf));

      /* make sure argv is large enough */
      if (argc >= argvCap) {
         argvCap *= 2;
         argv = impishRealloc(argv, argvCap * sizeof(void *));
      }

      /* copy the current token into the argument vector */
      impishMessage("the length of the current token is %i and contains %s\n",
            clen, cpos);
      argv[argc++] = impishStrndup(cpos, (size_t) clen);
      impishMessage("argv[%i] = %s\n", argc - 1, argv[argc - 1]);

      /* advance to the next character in buf */
      ++cpos;
   }

   /* NULL terminate the argument vector, first ensuring that there
    * exists sufficient size for the null pointer entry */
   if (argc >= argvCap) {
      argvCap *= 2;
      argv = impishRealloc(argv, argvCap * sizeof(void *));
   }
   argv[argc] = NULL;

   /* copy argc and argv back to caller (pass by value-return) */
   *argcPtr = argc;
   *argvPtr = argv;
}

void installSignalHandlers()
{
   /* initialize an array of sigaction structs coupled with their intended signal,
    * list global params are specified below */
   static const int IMPISH_SA_FLAGS = SA_SIGINFO | SA_RESTART;
   static struct {
      int sig;
      struct sigaction handler;
   } sigStructs[] = {
         { .sig = SIGHUP,  .handler = { .sa_sigaction = sigHUPAction  } },
         { .sig = SIGINT,  .handler = { .sa_sigaction = sigINTAction  } },
         { .sig = SIGQUIT, .handler = { .sa_sigaction = sigQUITAction } },
         { .sig = SIGILL,  .handler = { .sa_sigaction = sigILLAction  } },
         { .sig = SIGTRAP, .handler = { .sa_sigaction = sigTRAPAction } },
         { .sig = SIGABRT, .handler = { .sa_sigaction = sigABRTAction } },
         { .sig = SIGFPE,  .handler = { .sa_sigaction = sigFPEAction  } },
         { .sig = SIGSEGV, .handler = { .sa_sigaction = sigSEGVAction } },
         { .sig = SIGPIPE, .handler = { .sa_sigaction = sigPIPEAction } },
         { .sig = SIGALRM, .handler = { .sa_sigaction = sigALRMAction } },
         { .sig = SIGTERM, .handler = { .sa_sigaction = sigTERMAction } },
         { .sig = SIGCHLD, .handler = { .sa_sigaction = sigCHLDAction } }
   };

   /* allocate the loop invariant */
   sigset_t eSet;
   sigemptyset(&eSet);

   /* call sigaction() on each of the values defined in the struct above */
   for (size_t i = 0; i < ARRAY_SIZE(sigStructs); i++) {
      /* copy the current signal struct into cStruct */
      struct sigaction cStruct = sigStructs[i].handler;

      /* write the list-global params */
      cStruct.sa_mask = eSet;
      cStruct.sa_flags = IMPISH_SA_FLAGS;

      /* call sigaction(), warning on failure */
      if (sigaction(sigStructs[i].sig,
            (const struct sigaction * restrict)& cStruct,
            NULL) < 0) {
         fprintf(stderr,
               "warning: failed to install signal handler for signal: %i: %s\n",
               sigStructs[i].sig, strerror(errno));
      } else {
         impishMessage("installed handler for signal: %i\n", sigStructs[i].sig);
      }
   }
}

void exitHandler() {
   /* TODO: do some post-execution checks */
   printf("\n\nbye.\n");
}
