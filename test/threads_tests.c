#include <stdio.h>
#include <pico/stdlib.h>
#include <stdint.h>
#include <unity.h>
#include "unity_config.h"
#include <threads_funcs.h>

void setUp(void) {}

void tearDown(void) {}

void test_atomic_increment_success()
{
    SemaphoreHandle_t semaphore;
    semaphore = xSemaphoreCreateCounting(1, 1);
    int x = 1;
    int exp_result = 2;
    int act_result;

    TEST_ASSERT_TRUE(atomic_increment(&x, &semaphore));
    act_result = x;
    TEST_ASSERT_TRUE(act_result == exp_result);
}

void test_atomic_increment_starve()
{
    SemaphoreHandle_t semaphore;
    semaphore = xSemaphoreCreateCounting(1, 0);
    int x = 1;
    int exp_result = 1;
    int act_result;
    
    TEST_ASSERT_FALSE(atomic_increment(&x, &semaphore));
    act_result = x;
    TEST_ASSERT_TRUE(act_result == exp_result);
}

void test_multiplication(void)
{
    int x = 30;
    int y = 6;
    int z = x / y;
    TEST_ASSERT_TRUE_MESSAGE(z == 5, "Multiplication of two integers returned incorrect value.");
}

int main (void)
{
    stdio_init_all();
    while (1) {
        sleep_ms(5000); // Give time for TTY to attach.
        printf("Start tests\n");
        UNITY_BEGIN();
        RUN_TEST(test_atomic_increment_success);
        RUN_TEST(test_atomic_increment_starve);
        RUN_TEST(test_multiplication);
        sleep_ms(5000);
        UNITY_END();
    }
}
