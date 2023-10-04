#include "user_main.hpp"
#include "main.h"
#include "FreeRTOS.h"
#include "task.h"
#include "stdio_retarget2usb/stdio_retarget2usb.h"
#include <cstring>
#include <cstdio>
#include "FreeRtosSys/thread_priority_def.h"
#include "usart.h"
#include "jy901s/jy901s_device.hpp"

void BlinkLedEntry(void *argument)
{
    (void)argument;

    while (1) {
        HAL_GPIO_TogglePin(Led3_GPIO_Port, Led3_Pin);
        vTaskDelay(300);
    }
}

uint32_t kUart1RxCpltCount = 0;

void StartDefaultTask(void const *argument)
{
    (void)argument;

    StdioRetarget2Usb_Init();
    jy901.StartReceive();

    xTaskCreate(BlinkLedEntry, "BlinkLed", 512, nullptr, PriorityNormal, nullptr);

    while (1) {
        HAL_GPIO_TogglePin(Led2_GPIO_Port, Led2_Pin);
        // taskENTER_CRITICAL();
        // memcpy(rx_buffer, jy901.rx_buffer_cplt, sizeof(rx_buffer));
        // taskEXIT_CRITICAL();
        // for (size_t i = 0; i < sizeof(rx_buffer); i++) {
        //     printf("%02x ", rx_buffer[i]);
        // }

        auto accel = jy901.GetAccel();
        auto gyro  = jy901.GetGyro();
        // auto mag   = jy901.GetMag();
        auto euler = jy901.GetEuler();
        auto quat  = jy901.GetQuat();

        // for (auto &var : accel) {
        //     printf("%6.3g,", var);
        // }
        // for (auto &var : gyro) {
        //     printf("%6.3g,", var);
        // }
        // for (auto &var : mag) {
        //     printf("%6.3g,", var);
        // }
        for (auto &var : euler) {
            printf("%10.4g,", var);
        }
        for (auto &var : quat) {
            printf("%10.4g,", var);
        }

        printf("%.1f", jy901.GetTempearture());

        printf(",%u,%u,%u\n",
               jy901.stat_.valid_frame_count,
               jy901.stat_.check_sum_fail_count,
               jy901.stat_.finding_head_fail_count);
        vTaskDelay(10);
    }
}
