Readme
======

What is this?
-------------

This is a minimal unit testing framework consisting of one C header file. The
header file contains an assert-like macro and two output functions. Unlike
the standard library assert macro, this macro will print to stdout in cases
of both success and failure. It will also not terminate your program,
allowing you to see all test results at once.

If your output (for example xterm) is capable of understanding ANSI color
codes, the output will also be colorized in green or red, allowing you to see
more clearly which tests failed and which ones didn't.

Usage
---

### Call the macro

    test(expression)

### Filtering output

As the output will be printed to stderr on failure and stdout on success, it
is possible to filter the output of your compiled tests. You may choose to
only see tests that failed, like so:

    $ ./test > /dev/null

Or tests that were successful, like so:

    $ ./test 2> /dev/null

### Example

    #include <string.h>
    #include "unittest.h"
    
    int strlen_testcase(void)
    {
        char *s = "Dummy data";
        return strlen(s);
	}

    size_t sizeof_testcase(void)
    {
        return sizeof(int);
    }

    int main(void)
    {
        test(strlen_testcase() == 10);
        test(sizeof_testcase() == 4);
        return 0;
    }

Copying
-------

Copyright (c) 2012, Dennis Hedback 
All rights reserved.
 
Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright notice,
  this list of conditions and the following disclaimer.
* Redistributions in binary form must reproduce the above copyright
  notice, this list of conditions and the following disclaimer in the
  documentation and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
POSSIBILITY OF SUCH DAMAGE.
