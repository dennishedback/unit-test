/*
 * Umit
 *
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
#define UMIT_VERSION "0.2.0"

/****************************************************************************
 # INCLUDED MODULES
 ****************************************************************************/

/* Need stdio for logging the tests */
#include <stdio.h>
/* Need stdlib for memory management */
#include <stdlib.h>
/* Need string to perform copying of strings while logging */
#include <string.h>


/****************************************************************************
 # CUSTOM TYPES
 ****************************************************************************/

/* Enum to hold the different kinds of tests */
typedef enum {
    TEST_TYPE_ASSERT,
    TEST_TYPE_EXPECT
} TestType;

/* Testcase function pointer type  */
typedef void (*TestCasePtr)();

/* Testcase implemented as singly-linked list */
typedef struct TestCase {
    /* Pointer to the user defined function containing the testcase*/
    TestCasePtr func;
    /* Name of testcase (i.e. the name of the function) */
    char name[32];
    /* Whether or not the name of the testcase has already been written to
       the log */
    int logged;
    struct TestCase *next;
} TestCase;

/* Holds all command line options */
typedef struct Options {
    int color;
    int passed;
    int verbose;
} Options;


/****************************************************************************
 # FORWARD-DECLARATIONS
 ****************************************************************************/

/* User-defined functions. init_tests is called before the actual test begins,
   and cleanup_tests after. */
static void init_tests(void);
static void cleanup_tests(void);


/****************************************************************************
 # VARIABLES AND CONSTANTS
 ****************************************************************************/

/* Static variables are used to keep track of module-wide states. I think this
   is the least painful way to do it. */

/* Whether or not an assertion has failed */
extern int _assert_failed = 0;

/* The number of tests that has been run */
static int _num_run = 0;

/* The number of tests that failed */
static int _num_failed = 0;

/* As the TestCase type is implemented as a linked list, a pointer to its
   current node is needed */
static TestCase *current_testcase;

/* Also need a pointer to the head of the list */
static TestCase *head_testcase;

static Options options;


/****************************************************************************
 # PREPROCESSOR DEFINITIONS AND MACROS
 ****************************************************************************/

/* Evaluates a test, but only if the assert-failed flag is not set */
#define _EVAL(exp, type) if (!_assert_failed) \
    _process_result((int)(exp), #exp, type, __FILE__, __LINE__)

/* Assertion test. If it fails, no more tests will be evaluated */
#define ASSERT_EVAL(exp) _EVAL(exp, TEST_TYPE_ASSERT)

/* Expectation test. Tests continue to be evaluated even if an expectation
   fails */
#define EXPECT_EVAL(exp) _EVAL(exp, TEST_TYPE_EXPECT)

/* Registers testcases */
#define TC_REG(func) _register_testcase(func, #func)


/****************************************************************************
 # FUNCTION DEFINITIONS
 ****************************************************************************/

/*
 * Logs a test result
 */

static void _log(char const *exp, char const *type, char const *file, int line, int eval)
{
    #define RED "\x1b[31m"
    #define GREEN "\x1b[32m"
    #define RESET "\x1b[0m"

    char status[7], color[9], indent[3];

    if (!options.color) {
        status[0] = '\0';
    } else if (eval) {
        strcpy(color, GREEN);
    } else if (!eval) {
        strcpy(color, RED);
    }

    if (eval) {
        strcpy(status, "passed");
    } else {
        strcpy(status, "failed");
    }

    if (!current_testcase->logged && options.verbose) {
        fprintf(stderr, "In testcase '%s':\n", current_testcase->name);
        current_testcase->logged = 1;
        strcpy(indent, "  ");
    } else {
        strcpy(indent, "");
    }

	fprintf(
         stderr,
         "%s%s%s '%s' %s (%s:%i)\n%s",
         indent,
         color,
         type,
         exp,
         status,
         file,
         line,
         RESET
     );
}

/*
 * Processes a test result
 */

extern int _process_result(int passed, char const *exp, TestType type, char const *file, int line)
{ 
    char type_str[12];

    if (!passed) {
        /* The test did not pass, if this is an assertion test, no more tests
           should be evaluated. The assertion-failed flag is therefore
           set to 1 */
        if (type == TEST_TYPE_ASSERT) {
            _assert_failed = 1;
        }
        _num_failed++;
    }

    if ((passed && options.passed) || !passed) {
        switch (type) {
        case TEST_TYPE_ASSERT:
            strcpy(type_str, "Assertion");
            break;
        case TEST_TYPE_EXPECT:
            strcpy(type_str, "Expectation");
            break;
        } 

        _log(exp, type_str, file, line, passed);
    }
    
    _num_run++; 

    return passed;   
}


/*
 * Creates a new testcase node
 */

static TestCase *_tc_new(void)
{
    /* Explicit cast to be able to use this module in C++ projects */
    TestCase *node = (TestCase*)malloc(sizeof(TestCase));
    if (node == NULL) {
        fputs("Could not allocate memory", stderr);
        exit(-1);
    }
    node->func = NULL;
    node->next = NULL;
    node->logged = 0;
    return node;
}

/*
 * Registers a testcase
 */

extern void _register_testcase(TestCasePtr func, char const *testcase_name)
{
    current_testcase->func = func;
    strncpy(current_testcase->name, testcase_name, 31);
    current_testcase->next = _tc_new();
    current_testcase = current_testcase->next;
}

/*
 * Runs all testcases
 */

static void _run_tests(void)
{
    current_testcase = head_testcase;
    while (current_testcase->func != NULL) {
        /* Return prematurely if an assertion failed */
        if (_assert_failed) {
            return;
        }
        /* Execute the user-defined testcase function */
        current_testcase->func();                
        current_testcase = current_testcase->next;
    }
}

/*
 * Outputs usage
 */

void _print_usage(char *executable)
{
    fprintf(stderr, "Usage:  %s [OPTIONS]\n", executable);
}

/*
 * Outputs version information
 */

void _print_version(void)
{
    fputs("umit ", stderr);
    fputs(UMIT_VERSION, stderr);
    fputs("\nCopyright (C) Dennis Hedback 2014\n", stderr);
}

/*
 * Outputs help message
 */

void _print_help(char *executable)
{
    _print_usage(executable);
    fputs("Useful minimal testing-framework\n\n", stderr);
    fputs("  --no-color     No colored output\n", stderr);
    fputs("  --no-passed    Only output failed tests\n", stderr);
    fputs("  --no-verbose   Be less verbose\n", stderr);
    fputs("  --help         Display this help and exit\n", stderr);
    fputs("  --version      Output version information and exit\n", stderr);
}

/*
 * Parses command line arguments
 */

void _parse_arguments(int argc, char *argv[])
{
    int i;

    /* Options variable is global to this translation unit and holds shared
       states */

    /* Default options */
    options.color = 1;
    options.passed = 1;
    options.verbose = 1;

    /* Runtime options */
    for (i = 1; i < argc; i++) {
        if (strcmp("--no-color", argv[i]) == 0) {
            options.color = 0;
        } else if (strcmp("--no-passed", argv[i]) == 0) {
            options.passed = 0;
        } else if (strcmp("--no-verbose", argv[i]) == 0) {
            options.verbose = 0;
            options.passed = 0;
        } else if (strcmp("--help", argv[i]) == 0) {
            _print_help(argv[0]);
            exit(0);
        } else if (strcmp("--version", argv[i]) == 0) {
            _print_version();
            exit(0);
        } else {
            _print_usage(argv[0]);
            exit(-2);
        }
    }
}

int main(int argc, char *argv[])
{
    int num_passed;
    
    _parse_arguments(argc, argv);

    /* Initialize the linked list of testcases */
    head_testcase = _tc_new();
    current_testcase = head_testcase;

    /* This is a user-defined function */
    init_tests();

    /* All tests that the user registered in init_tests() are now evaluated */
    _run_tests();

    if (_assert_failed && options.verbose) {
        fputs("Test aborted due to failed assertion\n", stderr);
    }
 
    /* This is a user-defined function */
    cleanup_tests();

    if (options.verbose) {
        num_passed = _num_run - _num_failed;
        fprintf(stderr, "%i of %i tests passed\n", num_passed, _num_run);
    }

    /* Returns the number of failed tests as exit code*/
    return _num_failed;
}

#endif /*TEST_H*/
