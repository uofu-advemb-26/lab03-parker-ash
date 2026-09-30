#include <FreeRTOS.h>
#include <pico/cyw43_arch.h>
#include <pico/multicore.h>
#include <pico/stdlib.h>
#include <semphr.h>
#include <stdio.h>
#include <task.h>
#include <threads_funcs.h>
#include <threads_deadlock.h>

#define MAIN_TASK_PRIORITY (tskIDLE_PRIORITY + 1UL)
#define MAIN_TASK_STACK_SIZE configMINIMAL_STACK_SIZE

#define SIDE_TASK_PRIORITY (tskIDLE_PRIORITY + 1UL)
#define SIDE_TASK_STACK_SIZE configMINIMAL_STACK_SIZE

// semaphore! Shared resource AND locking mechanism.
SemaphoreHandle_t semaphore_A;
SemaphoreHandle_t semaphore_B;

// Shared resources in global memory space.
int on;

/**
 * Only job is to deadlock with main thread
 */
void side_thread(void *params) {
  while (1) {
    vTaskDelay(100);

    bool deadlocked = is_deadlocked_takeAthenB(&semaphore_A, &semaphore_B);

    printf("hello world from %s! Deadlocked? %s\n", "thread", deadlocked ? "true" : "false");
  }
}

/**
 * Toggles LED once every 100 ticks. When deadlocked, toggles LED once every second instead.
 */
void main_thread(void *params) {
  while (1) {
    cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, on);
    vTaskDelay(100);

    bool deadlocked = is_deadlocked_takeBthenA(&semaphore_A, &semaphore_B);

    printf("hello world from %s! Deadlocked? %s\n", "main", deadlocked ? "true" : "false");
    on = !on;
  }
}

int main(void) {
  stdio_init_all();
  hard_assert(cyw43_arch_init() == PICO_OK);
  on = false;
  TaskHandle_t main, side;
  // SemaphoreHandle_t xSemaphoreCreateCounting(UBaseType_t uxMaxCount,
  //                                            UBaseType_t uxInitialCount );
  semaphore_A = xSemaphoreCreateCounting(1, 1);
  semaphore_B = xSemaphoreCreateCounting(1, 1);
  xTaskCreate(main_thread, "MainThread", MAIN_TASK_STACK_SIZE, NULL,
              MAIN_TASK_PRIORITY, &main);
  xTaskCreate(side_thread, "SideThread1", SIDE_TASK_STACK_SIZE, NULL,
              SIDE_TASK_PRIORITY, &side);     
  vTaskStartScheduler();
  return 0;
}
