#include "user_main.hpp"
#include "main.h"
#include "FreeRTOS.h"
#include "task.h"
#include "stdio_retarget2usb/stdio_retarget2usb.h"
#include <cstring>
#include <cstdio>
#include "FreeRtosSys/thread_priority_def.h"

void BlinkLedEntry(void *argument)
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

    StdioRetarget2Usb_Init();

    xTaskCreate(BlinkLedEntry, "BlinkLed", 512, nullptr, PriorityNormal, nullptr);

    while (1) {
        HAL_GPIO_TogglePin(Led2_GPIO_Port, Led2_Pin);
        printf("Hello %lu\n", xTaskGetTickCount());
        vTaskDelay(500);
    }
}
