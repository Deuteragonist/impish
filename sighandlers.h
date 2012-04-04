/* 
 * CMPSC 311, Spring 2012, Project 7
 *
 * Author: Max DeLiso
 * Email: jmd5473@psu.edu, maxdeliso@gmail.com
 * File: sighandlers.h
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

#ifndef IMPISH_SIG_HANDLERS_H
#define IMPISH_SIG_HANDLERS_H

#include <stdbool.h>
#include <signal.h>
#include <unistd.h>

void sigHUPAction  (int sig, siginfo_t * siginfo, void *ctx);
void sigINTAction  (int sig, siginfo_t * siginfo, void *ctx);
void sigQUITAction (int sig, siginfo_t * siginfo, void *ctx);
void sigILLAction  (int sig, siginfo_t * siginfo, void *ctx);
void sigTRAPAction (int sig, siginfo_t * siginfo, void *ctx);
void sigABRTAction (int sig, siginfo_t * siginfo, void *ctx);
void sigFPEAction  (int sig, siginfo_t * siginfo, void *ctx);
void sigKILLAction (int sig, siginfo_t * siginfo, void *ctx);
void sigSEGVAction (int sig, siginfo_t * siginfo, void *ctx);
void sigSYSAction  (int sig, siginfo_t * siginfo, void *ctx);
void sigPIPEAction (int sig, siginfo_t * siginfo, void *ctx);
void sigALRMAction (int sig, siginfo_t * siginfo, void *ctx);
void sigTERMAction (int sig, siginfo_t * siginfo, void *ctx);
void sigCHLDAction (int sig, siginfo_t * siginfo, void *ctx);

#endif /* IMPISG_SIG_HANDLERS_H */
