/**
 ******************************************************************************
 * @file           modbus_master.cpp
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
#include <carbon/modbus_master.hpp>
#include <string.h>

// Check if IP is valid
bool ModbusMaster::isValidIp(const ModbusSlave &slave) const {
    return slave.slaveIp.addr != IPADDR_NONE;
}

// Construct Modbus read register request
void ModbusMaster::constructReadRegister(ModbusMessage &request,
                                         const ModbusSlave &slave,
                                         uint16_t startAddress,
                                         uint8_t functionCode) {
    request.slaveId = slave.slaveId;
    request.functionCode = functionCode;
    request.startAddress = startAddress;
    request.byteCount = 2; // Always one register
}

// Construct Modbus write register request
void ModbusMaster::constructWriteRegister(ModbusMessage &request,
                                          const ModbusSlave &slave,
                                          uint16_t startAddress,
                                          uint16_t value) {
    request.slaveId = slave.slaveId;
    request.functionCode = FUNC_WRITE_SINGLE_HOLDING_REGISTER;
    request.startAddress = startAddress;
    request.byteCount = 2;
    request.data[0] = (value >> 8) & 0xFF;
    request.data[1] = value & 0xFF;
}

// Process Modbus TCP response
Result<uint16_t> ModbusMaster::processResponse(uint8_t *response, size_t len) {
    uint16_t value;
    if (len < 5)
        return ModbusResponseFailed;
    uint8_t functionCode = response[1];
    uint8_t byteCount = response[2];

    if ((functionCode == FUNC_READ_HOLDING_REGISTERS ||
         functionCode == FUNC_READ_INPUT_REGISTERS) &&
        byteCount == 2) {
        value = (response[3] << 8) | response[4];
        return value;
    }
    return ModbusResponseFailed;
}

// Read holding register (1 register)
Result<uint16_t> ModbusMaster::readHoldingRegister(const ModbusSlave &slave,
                                                   uint16_t startAddress) {
    if (!isValidIp(slave))
        return NetworkInvalidIP;

    struct netconn *netConn = netconn_new(NETCONN_TCP);
    if (!netConn)
        return NetworkConnectionFailed;

    if (netconn_connect(netConn, &slave.slaveIp, TCP_PORT) != ERR_OK) {
        netconn_close(netConn);
        netconn_delete(netConn);
        return NetworkConnectionFailed;
    }

    ModbusMessage request;
    constructReadRegister(request, slave, startAddress,
                          FUNC_READ_HOLDING_REGISTERS);

    if (netconn_write(netConn, &request, sizeof(request), NETCONN_COPY) !=
        ERR_OK) {
        netconn_close(netConn);
        netconn_delete(netConn);
        return ModbusRequestFailed;
    }

    struct netbuf *buf;
    uint8_t response[MAX_PDU_SIZE];
    if (netconn_recv(netConn, &buf) == ERR_OK) {
        netbuf_copy(buf, response, MAX_PDU_SIZE);
        netbuf_delete(buf);
        auto res = processResponse(response, MAX_PDU_SIZE);
        netconn_close(netConn);
        netconn_delete(netConn);
        return res;
    }

    netconn_close(netConn);
    netconn_delete(netConn);
    return ModbusResponseFailed;
}

// Read input register (1 register)
Result<uint16_t> ModbusMaster::readInputRegister(const ModbusSlave &slave,
                                                 uint16_t startAddress) {
    if (!isValidIp(slave))
        return NetworkInvalidIP;

    struct netconn *netConn = netconn_new(NETCONN_TCP);
    if (!netConn)
        return NetworkConnectionFailed;

    if (netconn_connect(netConn, &slave.slaveIp, TCP_PORT) != ERR_OK) {
        netconn_close(netConn);
        netconn_delete(netConn);
        return NetworkConnectionFailed;
    }

    ModbusMessage request;
    constructReadRegister(request, slave, startAddress,
                          FUNC_READ_INPUT_REGISTERS);

    if (netconn_write(netConn, &request, sizeof(request), NETCONN_COPY) !=
        ERR_OK) {
        netconn_close(netConn);
        netconn_delete(netConn);
        return ModbusRequestFailed;
    }

    struct netbuf *buf;
    uint8_t response[MAX_PDU_SIZE];
    if (netconn_recv(netConn, &buf) == ERR_OK) {
        netbuf_copy(buf, response, MAX_PDU_SIZE);
        netbuf_delete(buf);
        auto res = processResponse(response, MAX_PDU_SIZE);
        netconn_close(netConn);
        netconn_delete(netConn);
        return res;
    }

    netconn_close(netConn);
    netconn_delete(netConn);
    return ModbusResponseFailed;
}

// Write holding register (1 register)
Error ModbusMaster::writeHoldingRegister(const ModbusSlave &slave,
                                         uint16_t startAddress,
                                         uint16_t value) {
    if (!isValidIp(slave))
        return NetworkInvalidIP;

    struct netconn *netConn = netconn_new(NETCONN_TCP);
    if (!netConn)
        return NetworkConnectionFailed;

    if (netconn_connect(netConn, &slave.slaveIp, TCP_PORT) != ERR_OK) {
        netconn_close(netConn);
        netconn_delete(netConn);
        return NetworkConnectionFailed;
    }

    ModbusMessage request;
    constructWriteRegister(request, slave, startAddress, value);

    if (netconn_write(netConn, &request, sizeof(request), NETCONN_COPY) !=
        ERR_OK) {
        netconn_close(netConn);
        netconn_delete(netConn);
        return ModbusRequestFailed;
    }

    struct netbuf *buf;
    uint8_t response[MAX_PDU_SIZE];
    if (netconn_recv(netConn, &buf) == ERR_OK) {
        netbuf_copy(buf, response, MAX_PDU_SIZE);
        netbuf_delete(buf);
        netconn_close(netConn);
        netconn_delete(netConn);
        return Success;
    }

    netconn_close(netConn);
    netconn_delete(netConn);
    return ModbusResponseFailed;
}
