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

#ifndef UMIT_TEST_H
#define UMIT_TEST_H
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
} UmitTestType;

/* Testcase function pointer type  */
typedef void (*UmitTestCasePtr)();

/* Testcase implemented as singly-linked list */
typedef struct UmitTestCase {
    /* Pointer to the user defined function containing the testcase*/
    UmitTestCasePtr func;
    /* Name of testcase (i.e. the name of the function) */
    char name[32];
    /* Whether or not the name of the testcase has already been written to
       the log */
    int logged;
    struct UmitTestCase *next;
} UmitTestCase;

/* Holds all command line umit_options */
typedef struct UmitOptions {
    int color;
    int passed;
    int verbose;
} UmitOptions;


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
extern int umit_assert_failed = 0;

/* The number of tests that has been run */
static int umit_num_run = 0;

/* The number of tests that failed */
static int umit_num_failed = 0;

/* As the UmitTestCase type is implemented as a linked list, a pointer to its
   current node is needed */
static UmitTestCase *umit_current_testcase;

/* Also need a pointer to the head of the list */
static UmitTestCase *umit_head_testcase;

static UmitOptions umit_options;


/****************************************************************************
 # PREPROCESSOR DEFINITIONS AND MACROS
 ****************************************************************************/

/* Evaluates a test, but only if the assert-failed flag is not set */
#define _EVAL(exp, type) if (!umit_assert_failed) \
    umit_process_result((int)(exp), #exp, type, __FILE__, __LINE__)

/* Assertion test. If it fails, no more tests will be evaluated */
#define UMIT_ASSERT(exp) _EVAL(exp, TEST_TYPE_ASSERT)

/* Expectation test. Tests continue to be evaluated even if an expectation
   fails */
#define UMIT_EXPECT(exp) _EVAL(exp, TEST_TYPE_EXPECT)

/* Registers testcases */
#define UMIT_REG(func) umit_register_testcase(func, #func)


/****************************************************************************
 # FUNCTION DEFINITIONS
 ****************************************************************************/

/*
 * Logs a test result
 */

static void umit_log(char const *exp, char const *type, char const *file, int line, int passed)
{
    #define RED "\x1b[31m"
    #define GREEN "\x1b[32m"
    #define RESET "\x1b[0m"

    char status[7], color[9], indent[3];

    if (!umit_options.color) {
        status[0] = '\0';
    } else if (passed) {
        strcpy(color, GREEN);
    } else if (!passed) {
        strcpy(color, RED);
    }

    if (passed) {
        strcpy(status, "passed");
    } else {
        strcpy(status, "failed");
    }

    if (!umit_current_testcase->logged && umit_options.verbose) {
        fprintf(stderr, "In testcase '%s':\n", umit_current_testcase->name);
        umit_current_testcase->logged = 1;
    }

    if (umit_options.verbose) {
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

extern int umit_process_result(int passed, char const *exp, UmitTestType type, char const *file, int line)
{ 
    char type_str[12];

    if (!passed) {
        /* The test did not pass, if this is an assertion test, no more tests
           should be evaluated. The assertion-failed flag is therefore
           set to 1 */
        if (type == TEST_TYPE_ASSERT) {
            umit_assert_failed = 1;
        }
        umit_num_failed++;
    }

    if ((passed && umit_options.passed) || !passed) {
        switch (type) {
        case TEST_TYPE_ASSERT:
            strcpy(type_str, "Assertion");
            break;
        case TEST_TYPE_EXPECT:
            strcpy(type_str, "Expectation");
            break;
        } 

        umit_log(exp, type_str, file, line, passed);
    }
    
    umit_num_run++; 

    return passed;   
}


/*
 * Creates a new testcase node
 */

static UmitTestCase *umit_create_testcase(void)
{
    /* Explicit cast to be able to use this module in C++ projects */
    UmitTestCase *node = (UmitTestCase*)malloc(sizeof(UmitTestCase));
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

extern void umit_register_testcase(UmitTestCasePtr func, char const *testcase_name)
{
    umit_current_testcase->func = func;
    strncpy(umit_current_testcase->name, testcase_name, 31);
    umit_current_testcase->next = umit_create_testcase();
    umit_current_testcase = umit_current_testcase->next;
}

/*
 * Runs all testcases
 */

static void umit_run_tests(void)
{
    umit_current_testcase = umit_head_testcase;
    while (umit_current_testcase->func != NULL) {
        /* Return prematurely if an assertion failed */
        if (umit_assert_failed) {
            return;
        }
        /* Execute the user-defined testcase function */
        umit_current_testcase->func();                
        umit_current_testcase = umit_current_testcase->next;
    }
}

/*
 * Outputs usage
 */

static void umit_print_usage(char *executable)
{
    fprintf(stderr, "Usage:  %s [OPTIONS]\n", executable);
}

/*
 * Outputs version information
 */

static void umit_print_version(void)
{
    fputs("umit ", stderr);
    fputs(UMIT_VERSION, stderr);
    fputs("\nCopyright (C) Dennis Hedback 2014\n", stderr);
}

/*
 * Outputs help message
 */

static void umit_print_help(char *executable)
{
    umit_print_usage(executable);
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

static void umit_parse_arguments(int argc, char *argv[])
{
    int i;

    /* 'umit_options' variable is global to this translation unit and holds shared
       states */

    /* Default umit_options */
    umit_options.color = 1;
    umit_options.passed = 1;
    umit_options.verbose = 1;

    /* Runtime umit_options */
    for (i = 1; i < argc; i++) {
        if (strcmp("--no-color", argv[i]) == 0) {
            umit_options.color = 0;
        } else if (strcmp("--no-passed", argv[i]) == 0) {
            umit_options.passed = 0;
        } else if (strcmp("--no-verbose", argv[i]) == 0) {
            umit_options.verbose = 0;
        } else if (strcmp("--help", argv[i]) == 0) {
            umit_print_help(argv[0]);
            exit(0);
        } else if (strcmp("--version", argv[i]) == 0) {
            umit_print_version();
            exit(0);
        } else {
            umit_print_usage(argv[0]);
            exit(-2);
        }
    }
}

int main(int argc, char *argv[])
{
    int num_passed;
    
    umit_parse_arguments(argc, argv);

    /* Initialize the linked list of testcases */
    umit_head_testcase = umit_create_testcase();
    umit_current_testcase = umit_head_testcase;

    /* This is a user-defined function */
    init_tests();

    /* All tests that the user registered in init_tests() are now evaluated */
    umit_run_tests();

    if (umit_assert_failed && umit_options.verbose) {
        fputs("Test aborted due to failed assertion\n", stderr);
    }
 
    /* This is a user-defined function */
    cleanup_tests();

    if (umit_options.verbose) {
        num_passed = umit_num_run - umit_num_failed;
        fprintf(stderr, "%i of %i tests passed\n", num_passed, umit_num_run);
    }

    /* Returns the number of failed tests as exit code*/
    return umit_num_failed;
}

#endif /*TEST_H*/
