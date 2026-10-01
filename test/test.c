#include "FreeRTOSConfig_examples_common.h"
#include <pico/stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <threads_funcs.h>
#include <unity.h>

#define TAKER_STACK_SIZE configMINIMAL_STACK_SIZE
#define TAKER_PRIORITY tskIDLE_PRIORITY + 1UL

void setUp(void) {}

void tearDown(void) {}

void test_atomic_increment_success() {
  SemaphoreHandle_t semaphore;
  semaphore = xSemaphoreCreateCounting(1, 1);
  int x = 1;
  int exp_result = 2;
  int act_result;

  TEST_ASSERT_TRUE(atomic_increment(&x, &semaphore));
  act_result = x;
  TEST_ASSERT_TRUE(act_result == exp_result);
}

void test_atomic_increment_starve() {
  SemaphoreHandle_t semaphore;
  semaphore = xSemaphoreCreateCounting(1, 0);
  int x = 1;
  int exp_result = 1;
  int act_result;

  TEST_ASSERT_FALSE(atomic_increment(&x, &semaphore));
  act_result = x;
  TEST_ASSERT_TRUE(act_result == exp_result);
}

SemaphoreHandle_t a, b;

void takeAthenB(void *params) {
  bool res = is_deadlocked_takeAthenB(&a, &b);
  TEST_ASSERT_TRUE(res);
}

void takeBthenA(void *params) {
  bool res = is_deadlocked_takeBthenA(&a, &b);
  TEST_ASSERT_TRUE(res);
}

void test_direct_deadlock() {
  a = xSemaphoreCreateCounting(1, 1);
  b = xSemaphoreCreateCounting(1, 1);

  TaskHandle_t handleAthenB, handleBthenA;

  xTaskCreate(takeAthenB, "taking A then B thread", TAKER_STACK_SIZE, NULL,
              TAKER_PRIORITY, &handleAthenB);
  xTaskCreate(takeBthenA, "taking B then A thread", TAKER_STACK_SIZE, NULL,
              TAKER_PRIORITY, &handleBthenA);

  sleep_ms(2);
  vTaskSuspend(handleAthenB);
  vTaskDelete(handleAthenB);
  vTaskSuspend(handleBthenA);
  vTaskDelete(handleBthenA);
}

int main(void) {
  stdio_init_all();
  while (1) {
    sleep_ms(5000); // Give time for TTY to attach.
    printf("Start tests\n");
    UNITY_BEGIN();
    RUN_TEST(test_atomic_increment_success);
    RUN_TEST(test_atomic_increment_starve);
    RUN_TEST(test_direct_deadlock);
    sleep_ms(5000);
    UNITY_END();
  }
}
