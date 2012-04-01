/* 
 * CMPSC 311, Spring 2012, Project 7
 *
 * Author: Max DeLiso
 * Email: jmd5473@psu.edu, maxdeliso@gmail.com
 * File: Makefile
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

#include <errno.h>
#include <signal.h>
#include <assert.h>

#include <readline/readline.h>
#include <readline/history.h>

/* linker directives for library globals */
extern char **environ;
extern char *optarg;
extern int optind, opterr, optopt;

/* read-only module-only globals */
static const char DEFAULT_PROMPT[] = ">> ";
static const char OPT_STRING[] = "hvies:";
static const char USAGE_STRING[] =
    "Usage:  impish [-h] [-v] [-i] [-e] [-s f] [file]\n"
    " -h     help\n"
    " -v     verbose mode\n"
    " -i     interactive mode\n"
    " -e     echo commands before execution\n"
    " -s f   use startup file f, default impish.init\n"
    " Shell commands:\n" "  help \n";

/* read/write globals */
bool verbose;

/* macro utility functions */
#define impishVerify(tf, msg)      _impishVerify(tf, msg,    __func__, __LINE__)
#define impishMalloc(size)         _impishMalloc(size,       __func__, __LINE__)
#define impishRealloc(ptr, size)   _impishRealloc(ptr, size, __func__, __LINE__)
#define impishStrdup(s)            _impishStrdup(s,          __func__, __LINE__)
#define impishStrndup(s,n)         _impishStrndup(s, n,      __func__, __LINE__)
#define impishFree(s)              _impishFree(s,            __func__, __LINE__)

/* TODO: split these definitions into multiple files */

/* main routines */
void processArgs(int argc, char *const argv[]);
void eval(const char *const cmdline);
int parseLine(const char *const buf, int *argcPtr, char ***argvPtr);
bool builtinCommand(const char *const *const argv);

/* utility functions/wrappers */
void _impishVerify(const bool tf,
                   const char *msg, const char *func, const int line);

void *_impishMalloc(const size_t size, const char *func, const int line);

void *_impishRealloc(void *ptr,
                     const size_t size, const char *func, const int line);

char *_impishStrdup(const char *s, const char *func, const int line);
char *_impishStrndup(const char *s, const size_t n, const char *func,
                     const int line);
void _impishFree(void *s, const char *func, const int line);

void impishMessage(const char *fmt, ...);

/* entry point */
int main(int argc, char *argv[])
{
   processArgs(argc, argv);

   char *cmdline;
   do {
      cmdline = readline(DEFAULT_PROMPT);

      if (cmdline) {
         eval(cmdline);
         impishFree(cmdline);
      }
   } while (cmdline && !feof(stdin));

   return EXIT_SUCCESS;
}

void processArgs(int argc, char *const argv[])
{
   int opt;

   /* TODO: set flags */
   while ((opt = getopt(argc, argv, OPT_STRING)) != -1) {
      switch (opt) {
      case 'h':
         printf("%s", USAGE_STRING);
         exit(EXIT_SUCCESS);

      case 'v':
         verbose = true;
         break;

      case 'i':
         /* TODO: set a flag */
         break;

      case 'e':
         /* TODO: set a flag */
         break;

      case 's':
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
   int ret;
   int argc = 0;
   char **argv = NULL;

   ret = parseLine(cmdline, &argc, &argv);

   if (argv == NULL || argv[0] == NULL) {
      impishFree(argv);
      return;
   }

   if (builtinCommand((const char *const *const)argv)) {
      return;
   }

   /* if we were able to find a command */
   /* TODO: devise a more transparent return machanism for parseLine */
   if (ret != 1)
      switch (pid = fork()) {
      case 0:
         if (execvp(argv[0], argv) == -1) {
            fprintf(stderr,
                    "%s: Command execution failed: %s\n",
                    argv[0], strerror(errno));

            /* TODO: return something different here */
            exit(EXIT_SUCCESS);
         }

      case -1:
         perror("fork");
         exit(EXIT_FAILURE);

      default:
         if (!ret) {
            int status;
            if (waitpid(pid, &status, 0) < 0) {
               perror("waitpid");
            }
         } else {
            printf("%d %s", pid, cmdline);
         }
         break;
      }

   /* free all of the memory allocated by parseLine */
   for (int i = 0; i < argc; ++i) {
      impishFree(argv[i]);
   }
   impishFree(argv);

}

bool builtinCommand(const char *const *const argv)
{
   if (!strcmp(argv[0], "quit")) {
      exit(0);
   }

   if (!strcmp(argv[0], "&")) {
      return true;
   }

   return false;
}

int parseLine(const char *const buf, int *argcPtr, char ***argvPtr)
{
   static const int ARGV_INIT_CAPACITY = 16;

   int argc = 0;
   int argvCap = ARGV_INIT_CAPACITY;
   char const *cpos = NULL;     /* chars that cpos points to are read only */
   char **argv = impishMalloc(argvCap * sizeof(void *));
   int bufStart;

   /* skip leading whitespace */
   for (bufStart = 0; isspace(buf[bufStart]); ++bufStart) ;

   /* bind offset plus start buffer to form effective buffer,
    * this is needed due to the fact that buf is const qualified */
   const char *const ebuf = buf + bufStart;
   const size_t ebufSize = strlen(ebuf);

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
         impishMessage("skipping space at offset: %i\n", cpos - ebuf);
         ++cpos;
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

   /* NULL terminate argv */
   if (argc >= argvCap) {
      argvCap *= 2;
      argv = impishRealloc(argv, argvCap * sizeof(void *));
   }
   argv[argc] = NULL;

   /* copy argc and argv back to caller (pass by value-return) */
   *argcPtr = argc;
   *argvPtr = argv;

   return 0;
}

/* taken from heller c2html example */
void _impishVerify(const bool tf, const char *msg, const char *func,
                   const int line)
{
   if (tf == false) {
      fprintf(stderr, "%s() at line %d failed: %s\n", func, line, msg);
      exit(EXIT_FAILURE);
   }
}

void *_impishMalloc(const size_t size, const char *func, const int line)
{
   void *p = malloc(size);
   if (p == NULL) {
      fprintf(stderr, "%s() at line %d failed: malloc(): %s\n", func,
              line, strerror(errno));
      exit(EXIT_FAILURE);
   }

   if (verbose) {
      fprintf(stderr, "malloc(%zd) at %p from %s line %d\n", size, p,
              func, line);
   }

   return p;
}

void *_impishRealloc(void *ptr, const size_t size, const char *func,
                     const int line)
{
   void *p = realloc(ptr, size);
   if (p == NULL) {
      fprintf(stderr, "%s() at line %d failed: realloc(): %s\n", func,
              line, strerror(errno));
      exit(EXIT_FAILURE);
   }

   if (verbose) {
      fprintf(stderr, "realloc(%p, %zd) at %p from %s line %d\n", ptr, size, p,
              func, line);
   }

   return p;
}

char *_impishStrdup(const char *s, const char *func, const int line)
{
   char *p = strdup(s);
   if (p == NULL) {
      fprintf(stderr, "%s() at line %d failed: strdup(): %s\n", func,
              line, strerror(errno));
      exit(EXIT_FAILURE);
   }

   if (verbose) {
      fprintf(stderr, "strdup(%zd) at %p from %s line %d\n",
              strlen(s) + 1, p, func, line);
   }

   return p;
}

char *_impishStrndup(const char *s, const size_t n, const char *func,
                     const int line)
{
   char *p = strndup(s, n);
   if (p == NULL) {
      fprintf(stderr, "%s() at line %d failed: strndup(): %s\n", func,
              line, strerror(errno));
      exit(EXIT_FAILURE);
   }

   if (verbose) {
      fprintf(stderr, "strndup(%zd, %zd) at %p from %s line %d\n",
              strlen(s) + 1, n, p, func, line);
   }

   return p;
}

void _impishFree(void *s, const char *func, const int line)
{
   if (verbose) {
      fprintf(stderr, "free(%p) from %s line %d\n", s, func, line);
   }

   free(s);
}

void impishMessage(const char *fmt, ...)
{
   if (verbose) {
      va_list argp;

      va_start(argp, fmt);
      vprintf(fmt, argp);
      va_end(argp);
   }
}
