/* 
 * CMPSC 311, Spring 2012, Project 7
 *
 * Author: Max DeLiso
 * Email: jmd5473@psu.edu, maxdeliso@gmail.com
 * File: sighandlers.c
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
#include <stdbool.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>

#include "impish.h"
#include "util.h"
#include "sighandlers.h"

void sigHUPAction(int sig, siginfo_t * siginfo, void *ctx)
{
   (void)sig;
   (void)siginfo;
   (void)ctx;
}

void sigINTAction(int sig, siginfo_t * siginfo, void *ctx)
{
   (void)sig;
   (void)siginfo;
   (void)ctx;
}

void sigQUITAction(int sig, siginfo_t * siginfo, void *ctx)
{
   (void)sig;
   (void)siginfo;
   (void)ctx;
}

void sigILLAction(int sig, siginfo_t * siginfo, void *ctx)
{
   (void)sig;
   (void)siginfo;
   (void)ctx;
}

void sigTRAPAction(int sig, siginfo_t * siginfo, void *ctx)
{
   (void)sig;
   (void)siginfo;
   (void)ctx;
}

void sigABRTAction(int sig, siginfo_t * siginfo, void *ctx)
{
   (void)sig;
   (void)siginfo;
   (void)ctx;
}

void sigFPEAction(int sig, siginfo_t * siginfo, void *ctx)
{
   (void)sig;
   (void)siginfo;
   (void)ctx;
}

void sigKILLAction(int sig, siginfo_t * siginfo, void *ctx)
{
   (void)sig;
   (void)siginfo;
   (void)ctx;
}

void sigSEGVAction(int sig, siginfo_t * siginfo, void *ctx)
{
   (void)sig;
   (void)siginfo;
   (void)ctx;
}

void sigSYSAction(int sig, siginfo_t * siginfo, void *ctx)
{
   (void)sig;
   (void)siginfo;
   (void)ctx;
}

void sigPIPEAction(int sig, siginfo_t * siginfo, void *ctx)
{
   (void)sig;
   (void)siginfo;
   (void)ctx;
}

void sigALRMAction(int sig, siginfo_t * siginfo, void *ctx)
{
   (void)sig;
   (void)siginfo;
   (void)ctx;
}

void sigTERMAction(int sig, siginfo_t * siginfo, void *ctx)
{
   (void)sig;
   (void)siginfo;
   (void)ctx;

   fflush(stdout);
   printf("caught sigterm (%i). ignoring...\n", sig);
}

void sigCHLDAction(int sig, siginfo_t * siginfo, void *ctx)
{
   (void)sig;
   (void)siginfo;
   (void)ctx;
}
