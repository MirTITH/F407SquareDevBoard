#include <errno.h>
#include <unistd.h>
#include <assert.h>
#include "usb_device.h"
#include "usbd_cdc_if.h"
#include "main.h"

static void StdioRetarget2Usb_ResetUsbPort(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    __HAL_RCC_GPIOA_CLK_ENABLE();
    GPIO_InitStruct.Pin   = GPIO_PIN_12;
    GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull  = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_12, GPIO_PIN_RESET);
    HAL_Delay(100);
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_12, GPIO_PIN_SET);
}

void StdioRetarget2Usb_Init()
{
    StdioRetarget2Usb_ResetUsbPort();
    MX_USB_DEVICE_Init();
}

int _read(int file, char *ptr, int len)
{
    (void)file;

    assert(0); // TODO

    return 0;
}

int _write(int file, char *ptr, int len)
{
    switch (file) {
        case STDOUT_FILENO: // 标准输出流
            CDC_Transmit_FS((uint8_t *)ptr, len);
            break;
        case STDERR_FILENO: // 标准错误流
            CDC_Transmit_FS((uint8_t *)ptr, len);
            break;
        default:
            // EBADF, which means the file descriptor is invalid or the file isn't opened for writing;
            errno = EBADF;
            return -1;
            break;
    }
    return len;
}