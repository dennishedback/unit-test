#include "test.h"

void testcase_1(void)
{
    UMIT_EXPECT(4 > 10); /* Fails */
    UMIT_EXPECT(5 == 5); /* Passes */
}

void testcase_2(void)
{
    UMIT_ASSERT(5 == 5); /* Passes, evaluations of tests continue */
    UMIT_ASSERT(5 != 5); /* Fails, no more evaluations of tests */
    UMIT_EXPECT(5 != 5); /* Never executed */
}

/* This function will never execute due to a failed assertion */
void testcase_3(void)
{
    UMIT_EXPECT(4 == 4);
}

void init_tests(void)
{
    /* Register the testcases for evaluation */
    UMIT_REG(testcase_1);
    UMIT_REG(testcase_2);
    UMIT_REG(testcase_3);
}

void cleanup_tests(void)
{
    /* Use tis one to free memory etc */
}
