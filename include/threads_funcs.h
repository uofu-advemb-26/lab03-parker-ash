#include <FreeRTOS.h>
#include <pico/cyw43_arch.h>
#include <pico/multicore.h>
#include <pico/stdlib.h>
#include <semphr.h>
#include <task.h>

int atomic_increment(int *x, SemaphoreHandle_t *s);
bool is_deadlocked_takeAthenB(SemaphoreHandle_t *semaphore_A,
                              SemaphoreHandle_t *semaphore_B);
bool is_deadlocked_takeBthenA(SemaphoreHandle_t *semaphore_A,
                              SemaphoreHandle_t *semaphore_B);
