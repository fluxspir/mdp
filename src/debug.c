/*
 * Copyright (c) 2012-2015 Bertrand Janin <b@janin.com>
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

#include <unistd.h>
#include <time.h>
#include <stdio.h>
#include <err.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdbool.h>

#include "debug.h"
#include "xmalloc.h"


bool		 debug_enabled = false;


/*
 * Print to stderr if debug flag is on.
 */
int
debug(const char *fmt, ...)
{
	va_list	ap;
	time_t tvec;
	struct tm *timeptr;
	char *pfmt;
	static char tbuf[20];
	int i;
	pid_t pid;

	if (!debug_enabled)
		return (0);

	pid = getpid();

	time(&tvec);
	timeptr = localtime(&tvec);
	i = strftime(tbuf, sizeof(tbuf), "%Y-%m-%d %H:%M:%S", timeptr);
	if (i == 0) {
		errx(EXIT_FAILURE, "strftime failed");
	}

	xasprintf(&pfmt, "[%s] [pid:%d] %s\n", tbuf, pid, fmt);

	va_start(ap, fmt);
	i = vfprintf(stderr, pfmt, ap);
	va_end(ap);

	xfree(pfmt);

	return (i);
}
