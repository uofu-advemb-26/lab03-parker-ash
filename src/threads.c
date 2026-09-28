#include <FreeRTOS.h>
#include <pico/cyw43_arch.h>
#include <pico/multicore.h>
#include <pico/stdlib.h>
#include <semphr.h>
#include <stdio.h>
#include <task.h>

#define MAIN_TASK_PRIORITY (tskIDLE_PRIORITY + 1UL)
#define MAIN_TASK_STACK_SIZE configMINIMAL_STACK_SIZE

#define SIDE_TASK_PRIORITY (tskIDLE_PRIORITY + 1UL)
#define SIDE_TASK_STACK_SIZE configMINIMAL_STACK_SIZE

// semaphore! Shared resource AND locking mechanism.
SemaphoreHandle_t semaphore;

// Shared resources in global memory space.
int counter;
int on;

void atomic_increment(int *x, SemaphoreHandle_t *s) {
  // lock
  counter += 1;
  // unlock
}

// Execution context waits and increments counter forever
// after incrementing, prints value
void side_thread(void *params) {
  while (1) {
    vTaskDelay(100);

    atomic_increment(&counter, &semaphore);

    printf("hello world from %s! Count %d\n", "thread", counter);
  }
}

// Execution context turns on the LED, increments and prints the counter,
// and toggles `on` forever
void main_thread(void *params) {
  while (1) {
    cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, on);
    vTaskDelay(100);

    atomic_increment(&counter, &semaphore);

    printf("hello world from %s! Count %d\n", "main", counter);
    on = !on;
  }
}

int main(void) {
  stdio_init_all();
  hard_assert(cyw43_arch_init() == PICO_OK);
  on = false;
  counter = 0;
  TaskHandle_t main, side;
  // SemaphoreHandle_t xSemaphoreCreateCounting(UBaseType_t uxMaxCount,
  //                                            UBaseType_t uxInitialCount );
  semaphore = xSemaphoreCreateCounting(1, 1);
  xTaskCreate(main_thread, "MainThread", MAIN_TASK_STACK_SIZE, NULL,
              MAIN_TASK_PRIORITY, &main);
  xTaskCreate(side_thread, "SideThread", SIDE_TASK_STACK_SIZE, NULL,
              SIDE_TASK_PRIORITY, &side);
  vTaskStartScheduler();
  return 0;
}
