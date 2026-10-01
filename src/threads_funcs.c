#include <FreeRTOS.h>
#include <pico/cyw43_arch.h>
#include <pico/multicore.h>
#include <pico/stdlib.h>
#include <semphr.h>
#include <stdio.h>
#include <task.h>
/**
 * Helper functions for threads.c
 */

/**
 * Increments int x by one atomically by taking semaphore s.
 */
bool atomic_increment(int *x, SemaphoreHandle_t *s) {
  if (xSemaphoreTake(*s, 1 * portTICK_PERIOD_MS) == pdTRUE) {
    *x += 1;
  } else {
    printf("{atomic_increment}: Semaphore not available\n");
    return false;
  }
  xSemaphoreGive(*s);

  return true;
}

/**
 * Takes semaphore A, waits a moment, then takes semaphore B. Returns true if
 * the request to take semaphore B times out, indicating the thread is
 * deadlocked. Returns false and gives back semaphores otherwise.
 */
bool is_deadlocked_takeAthenB(SemaphoreHandle_t *semaphore_A,
                              SemaphoreHandle_t *semaphore_B) {
  xSemaphoreTake(*semaphore_A, 1000 * portTICK_PERIOD_MS);
  vTaskDelay(10);
  if (xSemaphoreTake(*semaphore_B, 1000 * portTICK_PERIOD_MS) == pdFALSE) {
    xSemaphoreGive(*semaphore_A);
    return true;
  }

  xSemaphoreGive(*semaphore_A);
  xSemaphoreGive(*semaphore_B);
  return false;
}

/**
 * Takes semaphore B, waits a moment, then takes semaphore A. Returns true if
 * the request to take semaphore A times out, indicating the thread is
 * deadlocked. Returns false and gives back semaphores otherwise.
 */
bool is_deadlocked_takeBthenA(SemaphoreHandle_t *semaphore_A,
                              SemaphoreHandle_t *semaphore_B) {
  xSemaphoreTake(*semaphore_B, 1000 * portTICK_PERIOD_MS);
  vTaskDelay(10);
  if (xSemaphoreTake(*semaphore_A, 1000 * portTICK_PERIOD_MS) == pdFALSE) {
    xSemaphoreGive(*semaphore_B);
    return true;
  }

  bool deadlocked = 0;
  xSemaphoreGive(*semaphore_B);
  xSemaphoreGive(*semaphore_A);
  return false;
}
