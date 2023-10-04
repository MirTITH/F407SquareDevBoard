#include "main.h"
#include "jy901s/jy901s_device.hpp"

#ifdef __cplusplus
extern "C" {
#endif

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart);
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart);
void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart);

#ifdef __cplusplus
}
#endif

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
    (void)huart;
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    extern uint32_t kUart1RxCpltCount;
    if (huart->Instance == USART1) {
        kUart1RxCpltCount++;
        jy901.RxCpltCallback();
    }
}

void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == USART1) {
        jy901.RxCpltCallback();
    }
}