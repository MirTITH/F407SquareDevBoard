#pragma once

#include "main.h"
#include <cstring>
#include <array>

class JY901S
{
public:
    // 统计信息
    struct
    {
        size_t valid_frame_count;
        size_t check_sum_fail_count;
        size_t finding_head_fail_count;
    } stat_;

    JY901S(UART_HandleTypeDef *huart)
        : huart_(huart){};

    void StartReceive()
    {
        state_ = State_t::FindingHead;
        ReceiveNonBlocking((uint8_t *)&rx_buffer_);
    }

    std::array<float, 3> GetAccel() const
    {
        std::array<float, 3> result;
        for (size_t i = 0; i < result.size(); i++) {
            result[i] = raw_data_.accel[i] / (32768.0f / 16.0f);
        }
        return result;
    }

    float GetTempearture() const
    {
        return raw_data_.temperature / 100.0f;
    }

    std::array<float, 3> GetGyro() const
    {
        std::array<float, 3> result;
        for (size_t i = 0; i < result.size(); i++) {
            result[i] = raw_data_.gyro[i] / (32768.0f / 2000.0f);
        }
        return result;
    }

    std::array<float, 3> GetEuler() const
    {
        std::array<float, 3> result;
        for (size_t i = 0; i < result.size(); i++) {
            result[i] = raw_data_.euler[i] / (32768.0f / 180.0f);
        }
        return result;
    }

    std::array<float, 3> GetMag() const
    {
        std::array<float, 3> result;
        for (size_t i = 0; i < result.size(); i++) {
            result[i] = raw_data_.mag[i];
        }
        return result;
    }

    std::array<float, 4> GetQuat() const
    {
        std::array<float, 4> result;
        for (size_t i = 0; i < result.size(); i++) {
            result[i] = raw_data_.quat[i] / (32768.0f);
        }
        return result;
    }

    void RxCpltCallback()
    {
        switch (state_) {
            case State_t::FindingHead: // 正在寻找协议头
                // 判断是否找到协议头
                if (rx_buffer_.head == head_byte_) {
                    // 找到协议头
                    state_ = State_t::FoundHead;
                    // 继续接收剩下的数据
                    ReceiveNonBlocking((uint8_t *)&rx_buffer_ + 1, sizeof(rx_buffer_) - 1);
                } else {
                    // 未找到协议头，再次接收下一个字节
                    ReceiveNonBlocking((uint8_t *)&rx_buffer_);
                    stat_.finding_head_fail_count++;
                }
                break;
            case State_t::FoundHead: // 已经找到协议头，rx_buffer_ 里包含完整数据
                if (CheckSumCrc() == true) {
                    DecodeData();                                                   // 解码数据
                    ReceiveNonBlocking((uint8_t *)&rx_buffer_, sizeof(rx_buffer_)); // 继续接收下一个数据帧
                    stat_.valid_frame_count++;
                } else {
                    state_ = State_t::FindingHead;
                    ReceiveNonBlocking((uint8_t *)&rx_buffer_);
                    stat_.check_sum_fail_count++;
                }
                break;

            default:
                break;
        }
    }

    void RxErrCallback()
    {
        state_ = State_t::FindingHead;
        ReceiveNonBlocking((uint8_t *)&rx_buffer_);
    }

private:
    UART_HandleTypeDef *huart_;
    struct __attribute__((packed)) {
        uint8_t head;
        uint8_t data_type;
        int16_t data[4]; // 只适用于小端序平台
        uint8_t sumcrc;
    } rx_buffer_;

    const uint8_t *const rx_buffer_end = (uint8_t *)&rx_buffer_ + (sizeof(rx_buffer_) - 1);

    struct
    {
        int16_t accel[3];    // 加速度
        int16_t gyro[3];     // 角速度
        int16_t euler[3];    // 欧拉角
        int16_t mag[3];      // 磁场强度
        int16_t quat[4];     // 四元数
        int16_t temperature; // 温度
    } raw_data_;

    static const uint8_t head_byte_ = 0x55;

    enum class State_t {
        FindingHead,
        FoundHead
    } state_;

    /**
     * @brief 非阻塞式接收一个字节
     *
     */
    void ReceiveNonBlocking(uint8_t *buffer)
    {
        HAL_UART_Receive_DMA(huart_, buffer, 1);
    }

    /**
     * @brief 非阻塞式接收多个字节
     *
     */
    void ReceiveNonBlocking(uint8_t *buffer, uint16_t size)
    {
        HAL_UART_Receive_DMA(huart_, buffer, size);
    }

    bool CheckSumCrc()
    {
        const uint8_t *data = (uint8_t *)&rx_buffer_;
        uint8_t checksum    = 0;
        while (data < rx_buffer_end) {
            checksum += *data;
            data++;
        }

        return checksum == *data;
    }

    void DecodeData()
    {
        switch (rx_buffer_.data_type) {
            case 0x51:
                memcpy(raw_data_.accel, rx_buffer_.data, sizeof(raw_data_.accel));
                raw_data_.temperature = rx_buffer_.data[3];
                break;
            case 0x52:
                memcpy(raw_data_.gyro, rx_buffer_.data, sizeof(raw_data_.gyro));
                break;
            case 0x53:
                memcpy(raw_data_.euler, rx_buffer_.data, sizeof(raw_data_.euler));
                break;
            case 0x59:
                memcpy(raw_data_.quat, rx_buffer_.data, sizeof(raw_data_.quat));
                break;

            default:
                break;
        }
    }
};
