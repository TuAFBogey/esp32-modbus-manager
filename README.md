# ESP32 Modbus Manager

ESP32 Modbus Manager is a REST API service designed to manage Modbus devices, specifically ESP32 devices, through a web interface. It provides endpoints for device management, register operations, and real-time monitoring.

## Features

- **Device Management**
  - Register new devices with unique IDs
  - Configure Modbus slave IDs
  - Monitor device status
  - Manage WiFi settings for devices

- **Register Operations**
  - Read/Write individual registers
  - Batch operations for multiple registers
  - Support for different register types (Holding, Input, Coil, Discrete Input)
  - Register value validation
  - Historical tracking of register changes

- **Security**
  - Token-based authentication
  - Secure device registration
  - Request validation
  - Permission checks for register access

## Getting Started

### Prerequisites

- CMake 3.1+
- C++11 compiler
- SQLite3
- oatpp-1.2.5, oatpp-swagger-1.2.5, oatpp-sqlite-1.2.5

## API Documentation

### Device Endpoints

- `POST /api/device` - Create new device
- `GET /api/device` - List all devices
- `GET /api/device/{deviceId}` - Get device details
- `PUT /api/device/{deviceId}/status` - Update device status

### Register Endpoints

- `GET /api/register/{deviceId}/{type}/{address}` - Read register value
- `PUT /api/register` - Write register value
- `POST /api/register/write/batch` - Batch write registers
- `POST /api/register/read/batch` - Batch read registers

### Authentication Endpoints

- `POST /api/auth/login` - Get authentication token
- `POST /api/auth/signup` - Register new device
- `GET /api/auth/validate` - Validate authentication token
