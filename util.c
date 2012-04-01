/* 
 * CMPSC 311, Spring 2012, Project 7
 *
 * Author: Max DeLiso
 * Email: jmd5473@psu.edu, maxdeliso@gmail.com
 * File: util.c
 * License:
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING 
 * BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND 
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, 
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, 
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include <unistd.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdbool.h>
#include <string.h>
#include <errno.h>

#include "impish.h"
#include "util.h"

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
