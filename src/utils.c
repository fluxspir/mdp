/*
 * Copyright (c) 2012-2013 Bertrand Janin <b@janin.com>
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include <sys/param.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <unistd.h>
#include <time.h>
#include <stdio.h>
#include <wchar.h>
#include <wctype.h>
#include <errno.h>
#include <err.h>
#include <stdarg.h>
#include <stdlib.h>
#include <signal.h>

#include "utils.h"
#include "xmalloc.h"


#define WHITESPACE	L" \t\r\n"


extern int	 cfg_debug;
pid_t		 watcher_pid = 0;


/*
 * Print to stderr if debug flag is on.
 */
int
debug(const char *fmt, ...)
{
	va_list	ap;
	time_t tvec;
	struct tm *timeptr;
	char pfmt[256], tbuf[20];
	int i;
	pid_t pid;

	if (cfg_debug != 1)
		return 0;

	pid = getpid();

	time(&tvec);
	timeptr = localtime(&tvec);
	i = strftime(tbuf, sizeof(tbuf), "%Y-%m-%d %H:%M:%S", timeptr);
	if (i == 0)
		errx(0, "strftime failed");

	snprintf(pfmt, sizeof(pfmt), "[%s] [pid:%5d] %s\n", tbuf, pid, fmt);

	va_start(ap, fmt);
	i = vfprintf(stderr, pfmt, ap);
	va_end(ap);

	return i;
}


/*
 * Strip trailing whitespace.
 */
void
strip_trailing_whitespaces(wchar_t *s)
{
	int len;

	for (len = wcslen(s) - 1; len >= 0; len--) {
		if (wcschr(WHITESPACE, s[len]) == NULL)
			break;
		s[len] = '\0';
	}
}


/*
 * Same as wcsstr but case-insensitive.
 */
wchar_t *
wcscasestr(const wchar_t *s, const wchar_t *find)
{
	wchar_t c, sc;
	size_t len;

	if ((c = *find++) != 0) {
		c = (wchar_t)towlower((wchar_t)c);
		len = wcslen(find);
		do {
			do {
				if ((sc = *s++) == 0)
					return (NULL);
			} while ((wchar_t)towlower((wchar_t)sc) != c);
		} while (wcsncasecmp(s, find, len) != 0);
		s--;
	}

	/*
	 * Ignore the warning for this, it's better to accept const explicitly
	 * and have a warning than removing const everywhere.
	 */
	return ((wchar_t *)s);
}


/*
 * Duplicate a wide-char string as a multi-byte strings.
 *
 * This is essentially a wrapper around wcstombs with proper memory allocation.
 * You are responsible for free'ing the returned pointer's data. Any
 * encoding/decoding error will cause an immediate exit (e.g. one of the
 * wide-char can't be converted according to the current locale).
 */
char *
wcs_duplicate_as_mbs(const wchar_t *str)
{
	size_t bytelen, clen;
	char *output;

	/* Find out how much space we need to store the multi-byte string. */
	bytelen = wcstombs(NULL, str, 0);

	output = xmalloc(bytelen);

	clen = wcstombs(output, str, bytelen);
	if (clen == (size_t)-1) {
		err(100, "encoding error (invalid locale?)");
	}

	return output;
}


/*
 * Stop the process watch timeout.
 */
void
cancel_pid_timeout()
{
	if (watcher_pid == 0)
		return;

	debug("cancel_pid_timeout on %d", watcher_pid);

	if (kill(watcher_pid, SIGINT) != 0) {
		if (errno != ESRCH) {
			err(1, "cancel_pid_timeout");
		}
	}

	watcher_pid = 0;
}


/*
 * Automatically kill a process after X seconds.
 *
 * Sets the global pid_t for the watcher process, you need to run
 * cancel_pid_timeout once the child process is known to have completed.
 */
void
set_pid_timeout(pid_t pid, int timeout)
{
	watcher_pid = fork();

	switch (watcher_pid) {
	case -1:
		err(1, "set_pid_timeout fork()");
		break;

	case 0: /* Child process */
		/* Reset signals since the parent uses them to cleanup. */
		signal(SIGINT, SIG_DFL);
		signal(SIGKILL, SIG_DFL);

		debug("set_pid_timeout sleep(%d)", timeout);
		sleep(timeout);

		debug("set_pid_timeout kill: %d", pid);
		fprintf(stderr, "gpg timed out after %d seconds, aborting\n",
				timeout);
		if (kill(pid, SIGINT) != 0)
			err(1, "set_pid_timeout child kill");
		/* Avoid atexit() to run on the child. */
		_Exit(0);

		/* NOTREACHED */

	default:
		/* Parent process. NOP'ing. */
		break;
	}

	debug("set_pid_timeout parent pid=%d, watcher pid=%d", getpid(),
			watcher_pid);
}


/*
 * Check if a file exists.
 */
int
file_exists(char *filepath)
{
	struct stat sb;

	if (stat(filepath, &sb) != 0) {
		if (errno == ENOENT) {
			return 0;
		}
		err(1, "file_exists stat()");
	}

	return 1;
}

