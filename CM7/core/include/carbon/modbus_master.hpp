/**
 ******************************************************************************
 * @file           modbus_master.hpp
 * @author         Michele Viti <micheleviti78@gmail.com>
 * @date           Jan. 2025
 * @brief          Modbus master interface
 ******************************************************************************
 * @attention
 * Copyright (c) 2022 Michele Viti.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
#pragma once

#include <carbon/result.hpp>
#include <lwip/api.h>

class ModbusMaster {
public:
    // Modbus constants
    static constexpr uint16_t TCP_PORT = 502;
    static constexpr uint16_t MAX_PDU_SIZE = 256;

    // Modbus Function Codes
    static constexpr uint8_t FUNC_READ_HOLDING_REGISTERS = 0x03;
    static constexpr uint8_t FUNC_WRITE_SINGLE_HOLDING_REGISTER = 0x06;
    static constexpr uint8_t FUNC_READ_INPUT_REGISTERS = 0x04;

    // Structure to hold slave IP and ID
    struct ModbusSlave {
        ip_addr_t slaveIp;
        uint8_t slaveId;
    };

    // Modbus Message Structure
    struct ModbusMessage {
        uint8_t slaveId;
        uint8_t functionCode;
        uint16_t startAddress;
        uint8_t byteCount;
        uint8_t data[2]; // Only one register (2 bytes)
    };

    ModbusMaster() = default;

    Result<uint16_t> readHoldingRegister(const ModbusSlave &slave,
                                         uint16_t startAddress);
    Result<uint16_t> readInputRegister(const ModbusSlave &slave,
                                       uint16_t startAddress);
    Error writeHoldingRegister(const ModbusSlave &slave, uint16_t startAddress,
                               uint16_t value);

private:
    bool isValidIp(const ModbusSlave &slave) const;
    void constructReadRegister(ModbusMessage &request, const ModbusSlave &slave,
                               uint16_t startAddress, uint8_t functionCode);
    void constructWriteRegister(ModbusMessage &request,
                                const ModbusSlave &slave, uint16_t startAddress,
                                uint16_t value);
    Result<uint16_t> processResponse(uint8_t *response, size_t len);
};
