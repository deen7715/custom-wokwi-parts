// Wokwi Custom Chip - For docs and examples see:
// https://docs.wokwi.com/chips-api/getting-started
//
// SPDX-License-Identifier: MIT
// Copyright 2023 Deen Core

#include "wokwi-api.h"
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>

#define ATECC608_ADDR 0xC0 >> 1  // 7-bit I2C address

// ATECC608 Commands
#define CMD_RESET 0x00
#define CMD_SLEEP 0x01
#define CMD_IDLE 0x02
#define CMD_COMMAND 0x03
#define CMD_RANDOM 0x1B
#define CMD_NONCE 0x16
#define CMD_GENKEY 0x40
#define CMD_SIGN 0x41
#define CMD_VERIFY 0x45
#define CMD_READ 0x02
#define CMD_WRITE 0x12
#define CMD_LOCK 0x17
#define CMD_INFO 0x30

// Zones
#define ZONE_CONFIG 0x00
#define ZONE_OTP 0x01
#define ZONE_DATA 0x02

// Key types
#define KEY_TYPE_P256 0x04
#define KEY_TYPE_AES 0x06

#define CONFIG_SIZE 128
#define OTP_SIZE 64
#define DATA_SIZE 1024
#define MAX_PACKET_SIZE 128

typedef enum {
    IDLE,
    SLEEP,
    ACTIVE
} DeviceState;

typedef struct {
    DeviceState state;
    uint8_t lastError;
    uint8_t configZone[CONFIG_SIZE];
    uint8_t otpZone[OTP_SIZE];
    uint8_t dataZone[DATA_SIZE];
    uint8_t commandPacket[MAX_PACKET_SIZE];
    uint8_t responsePacket[MAX_PACKET_SIZE];
    uint8_t packetPos;
    uint8_t responsePos;
    uint32_t executionTime;
} ATECC608;

static ATECC608 device;

// Function prototypes
static void generateRandomNumber(uint8_t *random, uint8_t length);
static uint16_t calculateCRC(const uint8_t *data, size_t length);
static void processCommand(void);
static bool sendCommand(uint8_t command, uint8_t p1, uint16_t p2, const uint8_t *data, uint8_t dataLen);
static void simulateSignature(const uint8_t *message, uint8_t *signature);
static bool simulateVerification(const uint8_t *message, const uint8_t *signature);
static bool read(uint8_t zone, uint16_t address, uint8_t *data, uint8_t len);
static bool write(uint8_t zone, uint16_t address, const uint8_t *data, uint8_t len);
static bool lockConfigZone(void);
static bool lockDataAndOTPZones(void);
static bool isConfigLocked(void);
static bool isDataAndOTPLocked(void);
static bool storeKey(uint8_t key_id, const uint8_t *key, uint8_t key_type);
static bool generatePrivateKey(uint8_t key_id, uint8_t key_type);
static bool computeHMAC(uint8_t key_id, const uint8_t *message, uint8_t *hmac);
static bool deriveKey(uint8_t parent_key_id, uint8_t *derived_key);
static void simulateExecutionTime(uint32_t duration);
static void setResponse(uint8_t *data, uint8_t len);

void atecc608_init(void) {
    device.state = IDLE;
    device.lastError = 0;
    device.packetPos = 0;
    device.responsePos = 0;
    device.executionTime = 0;
    memset(device.configZone, 0xFF, CONFIG_SIZE);
    memset(device.otpZone, 0, OTP_SIZE);
    memset(device.dataZone, 0, DATA_SIZE);
    
    // Initialize config zone with some default values
    device.configZone[0] = 0x01; // I2C address
    device.configZone[1] = 0x23; // Chip mode
    // More config initialization can be added here

    // Seed the random number generator
    srand(time(NULL));
}

static void generateRandomNumber(uint8_t *random, uint8_t length) {
    for (uint8_t i = 0; i < length; i++) {
        random[i] = rand() & 0xFF;
    }
}

static uint16_t calculateCRC(const uint8_t *data, size_t length) {
    uint16_t crc = 0;
    uint8_t counter;
    uint16_t crc_register;
    uint16_t polynom = 0x8005;
    
    for (counter = 0; counter < length; counter++) {
        crc_register = (uint8_t)(crc >> 8) | (crc << 8);
        crc_register ^= data[counter];
        crc_register ^= (uint8_t)(crc_register & 0xff) >> 4;
        crc_register ^= (crc_register << 8) << 4;
        crc_register ^= ((crc_register & 0xff) << 4) << 1;
        crc = crc_register;
    }
    
    return crc;
}

static void processCommand(void) {
    uint8_t command = device.commandPacket[2];
    uint8_t p1 = device.commandPacket[3];
    uint16_t p2 = (device.commandPacket[5] << 8) | device.commandPacket[4];
    
    switch (command) {
        case CMD_RANDOM:
            generateRandomNumber(device.responsePacket + 1, 32);
            setResponse(device.responsePacket, 33);
            simulateExecutionTime(23);  // Typical execution time in ms
            break;
        case CMD_NONCE:
            // Implement nonce generation
            simulateExecutionTime(7);  // Typical execution time in ms
            break;
        case CMD_GENKEY:
            // Implement key generation
            simulateExecutionTime(115);  // Typical execution time in ms
            break;
        case CMD_SIGN:
            // Implement signing
            simulateExecutionTime(60);  // Typical execution time in ms
            break;
        case CMD_VERIFY:
            // Implement verification
            simulateExecutionTime(72);  // Typical execution time in ms
            break;
        case CMD_READ:
            // Implement read operation
            simulateExecutionTime(1);  // Typical execution time in ms
            break;
        case CMD_WRITE:
            // Implement write operation
            simulateExecutionTime(26);  // Typical execution time in ms
            break;
        case CMD_LOCK:
            // Implement lock operation
            simulateExecutionTime(32);  // Typical execution time in ms
            break;
        case CMD_INFO:
            // Implement info command
            simulateExecutionTime(1);  // Typical execution time in ms
            break;
        default:
            device.lastError = 7;
            break;
    }
}

uint8_t atecc608_read_byte(void) {
    if (device.responsePos < MAX_PACKET_SIZE) {
        return device.responsePacket[device.responsePos++];
    }
    return 0;
}

void atecc608_write_byte(uint8_t byte) {
    if (device.packetPos < MAX_PACKET_SIZE) {
        device.commandPacket[device.packetPos++] = byte;
    }
    if (device.packetPos == 1) {  // Word Address
        if (byte == 0x03) {  // Command packet
            device.packetPos = 0;  // Reset for actual command
        }
    } else if (device.packetPos > 1 && device.commandPacket[0] == CMD_COMMAND) {
        if (device.packetPos == device.commandPacket[1]) {  // Received all bytes
            processCommand();
            device.packetPos = 0;
        }
    }
}

static bool sendCommand(uint8_t command, uint8_t p1, uint16_t p2, const uint8_t *data, uint8_t dataLen) {
    uint8_t count = 7 + dataLen;
    device.commandPacket[0] = CMD_COMMAND;
    device.commandPacket[1] = count;
    device.commandPacket[2] = command;
    device.commandPacket[3] = p1;
    device.commandPacket[4] = p2 & 0xFF;
    device.commandPacket[5] = p2 >> 8;
    if (data && dataLen > 0) {
        memcpy(&device.commandPacket[6], data, dataLen);
    }
    device.commandPacket[count - 1] = calculateCRC(&device.commandPacket[1], count - 2);
    
    processCommand();
    return true;
}

static void simulateSignature(const uint8_t *message, uint8_t *signature) {
    for (uint8_t i = 0; i < 64; i++) {
        signature[i] = message[i % 32] ^ (rand() & 0xFF);
    }
}

static bool simulateVerification(const uint8_t *message, const uint8_t *signature) {
    return true;
}

static bool read(uint8_t zone, uint16_t address, uint8_t *data, uint8_t len) {
    if (len > 32) {
        device.lastError = 1;
        return false;
    }
    uint8_t *source;
    uint16_t max_len;
    switch (zone & 0x03) {
        case ZONE_CONFIG: source = device.configZone; max_len = CONFIG_SIZE; break;
        case ZONE_OTP: source = device.otpZone; max_len = OTP_SIZE; break;
        case ZONE_DATA: source = device.dataZone; max_len = DATA_SIZE; break;
        default: device.lastError = 2; return false;
    }
    if (address + len > max_len) {
        device.lastError = 3;
        return false;
    }
    memcpy(data, source + address, len);
    return true;
}

static bool write(uint8_t zone, uint16_t address, const uint8_t *data, uint8_t len) {
    if (len > 32) {
        device.lastError = 1;
        return false;
    }
    uint8_t *dest;
    uint16_t max_len;
    switch (zone & 0x03) {
        case ZONE_CONFIG: dest = device.configZone; max_len = CONFIG_SIZE; break;
        case ZONE_OTP: dest = device.otpZone; max_len = OTP_SIZE; break;
        case ZONE_DATA: dest = device.dataZone; max_len = DATA_SIZE; break;
        default: device.lastError = 2; return false;
    }
    if (address + len > max_len) {
        device.lastError = 3;
        return false;
    }
    memcpy(dest + address, data, len);
    return true;
}

static bool lockConfigZone(void) {
    device.configZone[87] = 0x00;  // Set the lock byte
    return true;
}

static bool lockDataAndOTPZones(void) {
    device.configZone[86] = 0x00;  // Set the lock byte
    return true;
}

static bool isConfigLocked(void) {
    return device.configZone[87] == 0x00;
}

static bool isDataAndOTPLocked(void) {
    return device.configZone[86] == 0x00;
}

static bool storeKey(uint8_t key_id, const uint8_t *key, uint8_t key_type) {
    if (key_id >= 16) {
        device.lastError = 8;
        return false;
    }
    uint16_t address = 32 * key_id;
    return write(ZONE_DATA, address, key, 32);
}

static bool generatePrivateKey(uint8_t key_id, uint8_t key_type) {
    uint8_t private_key[32];
    generateRandomNumber(private_key, 32);
    return storeKey(key_id, private_key, key_type);
}

static bool computeHMAC(uint8_t key_id, const uint8_t *message, uint8_t *hmac) {
    uint8_t key[32];
    if (!read(ZONE_DATA, 32 * key_id, key, 32)) {
        return false;
    }
    for (uint8_t i = 0; i < 32; i++) {
        hmac[i] = key[i] ^ message[i];
    }
    return true;
}

static bool deriveKey(uint8_t parent_key_id, uint8_t *derived_key) {
    uint8_t parent_key[32];
    if (!read(ZONE_DATA, 32 * parent_key_id, parent_key, 32)) {
        return false;
    }
    for (uint8_t i = 0; i < 32; i++) {
        derived_key[i] = parent_key[i] ^ (i + 1);
    }
    return true;
}

static void simulateExecutionTime(uint32_t duration) {
    device.executionTime = duration;
    // In a real simulation, you might want to actually delay here
    // For Wokwi, we'll just store the duration
}

static void setResponse(uint8_t *data, uint8_t len) {
    memcpy(device.responsePacket, data, len);
    device.responsePos = 0;
}

void atecc608_reset(void) {
    atecc608_init();
}

// Wokwi API integration
static i2c_dev_t atecc608_dev = {
    .address = ATECC608_ADDR,
    .write_func = atecc608_write_byte,
    .read_func = atecc608_read_byte
};

void chip_init() {
    atecc608_init();
    i2c_register_device(&atecc608_dev);
}