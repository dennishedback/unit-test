/*
 * Copyright (c) 2012, Dennis Hedback 
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *   * Redistributions of source code must retain the above copyright notice,
 *     this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef TEST_H
#define TEST_H

#include <stdio.h>

#define ANSI_FORECOLOR_RED "\x1b[31m"
#define ANSI_FORECOLOR_GREEN "\x1b[32m"
#define ANSI_BACKCOLOR_RED "\x1b[41m"
#define ANSI_BACKCOLOR_GREEN "\x1b[42m"
#define ANSI_BOLD_ON "\x1b[1m"
#define ANSI_RESET "\x1b[0m"

#define test(exp) \
	if (exp) \
		_log_succ(#exp, __FILE__, __LINE__); \
	else \
		_log_fail(#exp, __FILE__, __LINE__);

void _log_fail(char const *exp, char *file, int line)
{
	fprintf(
		stderr,
		"%s%sFAILURE!%s%s Test case (%s) at %s:%i failed%s\n",
		ANSI_BACKCOLOR_RED,
		ANSI_BOLD_ON,
		ANSI_RESET,
		ANSI_FORECOLOR_RED,
		exp,
		file,
		line,
		ANSI_RESET
	);
}

void _log_succ(char const *exp, char *file, int line)
{
	printf(
		"%s%sSUCCESS!%s%s Test case (%s) at %s:%i was successful%s\n",
		ANSI_BACKCOLOR_GREEN,
		ANSI_BOLD_ON,
		ANSI_RESET,
		ANSI_FORECOLOR_GREEN,
		exp,
		file,
		line,
		ANSI_RESET
	);
}

#endif /*TEST_H*/

