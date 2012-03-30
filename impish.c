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

#include <errno.h>
#include <signal.h>
#include <assert.h>

#include <readline/readline.h>
#include <readline/history.h>

extern char **environ;

static const char DEFAULT_PROMPT[] = ">> ";

void eval(const char *const cmdline);
int parseLine(const char *const buf, int *argcPtr, char ***argvPtr);
bool builtinCommand(const char *const *const argv);

int main()
{
   char *cmdline;

   do {
      cmdline = readline(DEFAULT_PROMPT);

      if (cmdline) {
         eval(cmdline);
         free(cmdline);
      }
   } while (cmdline && !feof(stdin));

   return 0;
}

void eval(const char *const cmdline)
{
   pid_t pid;
   int ret;
   int argc = 0;
   char **argv = NULL;

   /* TODO: maybe remove leading trailing whitespace in place */

   ret = parseLine(cmdline, &argc, &argv);

   if (argv == NULL || argv[0] == NULL) {
      free(argv);
      return;
   }

   if (builtinCommand((const char **)argv)) {
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
            exit(0);
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
      free(argv[i]);
   }
   free(argv);

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

   /* initialize local variables */
   int argc = 0;
   int argvCap = ARGV_INIT_CAPACITY;
   /* cpos's address can be changed, but not the character it points to */
   char const *cpos = NULL;
   /* TODO: write a malloc/realloc wrapper */
   char **argv = malloc(argvCap * sizeof(void *));
   assert(argv);

   /* extract first token into argv[0] */
   if ((cpos = strchr(buf, ' '))) {
      printf("length of first string: %i\n", (int)(cpos - buf));
      argv[0] = strndup(buf, cpos - buf);
   } else {
      argv[0] = strdup(buf);
   }
   assert(argv[0]);

   /* extract the rest of the tokens into argv[i] for i >= 1 */
   argc = 1;
   cpos = buf;
   while ((cpos = strchr(cpos, ' '))) {

      /* compute the length of the current token */
      const char const *const npos = strchr(cpos + 1, ' ');
      const int clen =
          (npos) ? (npos - (cpos + 1)) : ((int)strlen(buf) -
                                          ((cpos + 1) - buf));

      /* make sure argv is large enough */
      /* TODO: wrap this this into a function */
      if (argc >= argvCap) {
         argvCap *= 2;
         argv = realloc(argv, argvCap * sizeof(void *));
         assert(argv);
      }

      /* copy the current token into the argument vector */
      argv[argc] = strndup(cpos + 1, (size_t) clen);
      assert(argv[argc]);
      ++argc;

      /* advance to the next character in buf */
      ++cpos;
   }

   /* NULL terminate argv */
   /* TODO: wrap this this into a function */
   if (argc >= argvCap) {
      argvCap *= 2;
      argv = realloc(argv, argvCap * sizeof(void *));
      assert(argv);
   }
   argv[argc] = NULL;

   /* copy argc and argv back to caller (pass by value-return) */
   *argcPtr = argc;
   *argvPtr = argv;

   return 0;
}
