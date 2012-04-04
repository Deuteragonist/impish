/* 
 * CMPSC 311, Spring 2012, Project 7
 *
 * Author: Max DeLiso
 * Email: jmd5473@psu.edu, maxdeliso@gmail.com
 * File: util.h
 * Repository: https://github.com/maxdeliso/impish
 * License:
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

#ifndef IMPISH_UTIL_H
#define IMPISH_UTIL_H

#include <stdint.h>
#include <stdarg.h>

#include "impish.h"

/* macro utility functions */
#define impishVerify(tf, msg) \
   _impishVerify(tf, msg,    __func__, __LINE__)
#define impishMalloc(size) \
   _impishMalloc(size,       __func__, __LINE__)
#define impishRealloc(ptr, size) \
   _impishRealloc(ptr, size, __func__, __LINE__)
#define impishStrdup(s) \
   _impishStrdup(s, __func__, __LINE__)
#define impishStrndup(s,n) \
   _impishStrndup(s, n, __func__, __LINE__)
#define impishFree(s) \
   _impishFree(s, __func__, __LINE__)

/* utility functions/wrappers */
void _impishVerify(
      const bool tf,
      const char *msg,
      const char *func,
      const int line);

void *_impishMalloc(
      const size_t size,
      const char *func,
      const int line);

void *_impishRealloc(
      void *ptr,
      const size_t size,
      const char *func,
      const int line);

char *_impishStrdup(
      const char *s,
      const char *func,
      const int line);

char *_impishStrndup(
      const char *s,
      const size_t n,
      const char *func,
      const int line);

void _impishFree(
      void *s,
      const char *func,
      const int line);

void impishMessage(const char *fmt, ...);

#endif /* IMPISH_UTIL_H */
