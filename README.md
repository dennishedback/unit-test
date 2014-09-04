umit
====

Useful Minimal Testing-framework
--------------------------------

umit is a minimal unit testing framework consisting of one C header file which
can be dropped into projects. It will compile as both ANSI-C and C++98. 

If your output (for example xterm) is capable of understanding ANSI color
codes, the output will also be colorized in green or red, allowing you to see
more clearly which tests failed and which ones didn't.

Usage
-----

### Interface

The public interface consists of three preprocessor macros and two function
declarations. The user shall define the following functions:

    void init_tests(void);
    void cleanup_tests(void);

The user may initialize the testcases and register them with umit within
`init_tests` using the `TC_REG(func)` macro, where `func` is a function
pointer to a testcase. A testcase takes the following form:

    void testcase(void);

The user may then perform tests inside testcases. There are currently two
types of tests:

    EXPECT(exp)
    ASSERT(exp)

The difference between them is that all further evalutation is aborted if
`ASSERT` fails. This is not the case of a failure within `EXPECT`.

### Command-line options
    
    --no-color     No colored output
    --no-passed    Only output failed tests
    --no-verbose   Be less verbose
    --help         Display this help and exit
    --version      Output version information and exit

### Example

The following code should contain _all_ aspects of using umit.

    #include "test.h"
    
    void testcase_1(void)
    {
        EXPECT(4 > 10); /* Fails */
        EXPECT(5 == 5); /* Passes */
    }
    
    void testcase_2(void)
    {
        ASSERT(5 == 5); /* Passes, evaluations of tests continue */
        ASSERT(5 != 5); /* Fails, no more evaluations of tests */
        EXPECT(5 != 5); /* Never executed */
    }
    
    /* This function will never execute due to a failed assertion */
    void testcase_3(void)
    {
        EXPECT(4 == 4);
    }
    
    void init_tests(void)
    {
        /* Register the testcases for evaluation */
        TC_REG(testcase_1);
        TC_REG(testcase_2);
        TC_REG(testcase_3);
    }

    void cleanup_tests(void)
    {
        /* Use tis one to free memory etc */
    }

Copying
-------

Copyright (c) 2014, Dennis Hedback 
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
