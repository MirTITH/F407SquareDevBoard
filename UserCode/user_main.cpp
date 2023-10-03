#include "user_main.hpp"
#include "main.h"
#include "FreeRTOS.h"
#include "task.h"

void ThreadEntry(void *argument)
{
    (void)argument;

    while (1) {
        HAL_GPIO_TogglePin(Led3_GPIO_Port, Led3_Pin);
        vTaskDelay(300);
    }
}

void StartDefaultTask(void const *argument)
{
    (void)argument;

    xTaskCreate(ThreadEntry, "ThreadName", 512, nullptr, 3, nullptr);

    while (1) {
        HAL_GPIO_TogglePin(Led2_GPIO_Port, Led2_Pin);
        vTaskDelay(500);
    }
}