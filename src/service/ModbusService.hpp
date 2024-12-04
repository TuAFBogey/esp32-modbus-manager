#ifndef MODBUS_SERVICE_HPP
#define MODBUS_SERVICE_HPP

#include "db/ModbusDb.hpp"
#include "dto/DeviceDto.hpp"
#include "dto/ModbusDto.hpp"
#include "dto/StatusDto.hpp"
#include "service/AuthService.hpp"
#include "oatpp/core/macro/component.hpp"
#include "oatpp/web/protocol/http/Http.hpp"

class ModbusService {
private:
    typedef oatpp::web::protocol::http::Status Status;
    std::shared_ptr<ModbusDb> m_database;
    std::shared_ptr<AuthService> m_authService;

    void validateDevice(const oatpp::Int32& deviceId) {
        auto result = m_database->getDeviceById(deviceId);
        if (!result->hasMoreToFetch()) {
            OATPP_LOGE("ModbusService", "Device not found: %d", deviceId);
            throw Status::CODE_404;
        }
    }

    void validateDeviceId(const oatpp::String& deviceId) {
        auto result = m_database->getDeviceByDeviceId(deviceId);
        if (result->hasMoreToFetch()) {
            OATPP_LOGE("ModbusService", "Device ID already exists: %s", deviceId->c_str());
            throw Status::CODE_409;
        }
    }

    void validateRegisterAccess(
        const oatpp::Int32& deviceId,
        const oatpp::String& registerType,
        const oatpp::Int32& address,
        bool isWrite = false) {

        auto result = m_database->getRegister(deviceId, registerType, address);
        if (!result->hasMoreToFetch()) {
            OATPP_LOGE("ModbusService", "Register not found: device=%d, type=%s, address=%d",
                       deviceId, registerType->c_str(), address);
            throw Status::CODE_404;
        }

        auto reg = result->fetch<oatpp::Object<RegisterDto>>();

        if (isWrite) {
            if (reg->read_only) {
                OATPP_LOGE("ModbusService", "Attempt to write to read-only register: %d", address);
                throw Status::CODE_403;
            }

            if (registerType == "INPUT" || registerType == "DISCRETE_INPUT") {
                OATPP_LOGE("ModbusService", "Cannot write to input register: type=%s", registerType->c_str());
                throw Status::CODE_403;
            }
        }
    }

    void logRegisterAccess(
        const oatpp::Int32& registerId,
        const oatpp::Int32& userId,
        const oatpp::Int32& oldValue,
        const oatpp::Int32& newValue,
        const oatpp::String& action) {

        try {
            m_database->createHistory(registerId, userId, oldValue, newValue, action);
            OATPP_LOGD("ModbusService", "Access logged: reg=%d, user=%d, old=%d, new=%d, action=%s",
                       registerId, userId, oldValue, newValue, action->c_str());
        } catch (const std::exception& e) {
            OATPP_LOGE("ModbusService", "Failed to log register access: %s", e.what());
        }
    }

public:
    ModbusService(const std::shared_ptr<AuthService>& authService,
                  const std::shared_ptr<ModbusDb>& database)
        : m_database(database)
        , m_authService(authService)
    {
        checkDatabaseContent();
    }

    void checkDatabaseContent() {
        OATPP_LOGD("ModbusService", "Checking database content...");

        // Check devices
        auto deviceResult = m_database->getAllDevices();
        auto devices = deviceResult->fetch<oatpp::Vector<oatpp::Object<DeviceDto>>>();
        OATPP_LOGD("ModbusService", "Found %d devices", devices->size());

        for(auto& device : *devices) {
            OATPP_LOGD("ModbusService", "Device: id=%d, device_id=%s, slave_id=%d",
                       device->id,
                       device->device_id->c_str(),
                       device->slave_id);
        }

        // Check registers
        auto regResult = m_database->getAllRegisters();
        auto registers = regResult->fetch<oatpp::Vector<oatpp::Object<RegisterDto>>>();
        OATPP_LOGD("ModbusService", "Found %d registers", registers->size());

        for(auto& reg : *registers) {
            OATPP_LOGD("ModbusService", "Register: id=%d, device=%d, type=%s, address=%d, value=%d",
                       reg->id,
                       reg->device_id,
                       reg->register_type->c_str(),
                       reg->address,
                       reg->value);
        }
    }

    // Device operations
    oatpp::Object<DeviceDto> createDevice(
        const oatpp::Object<CreateDeviceDto>& createDto,
        const oatpp::String& authHeader) {

        if (!createDto) {
            OATPP_LOGE("ModbusService", "Invalid device creation data - createDto is null");
            throw Status::CODE_400;
        }

        OATPP_LOGD("ModbusService", "Processing device creation request");
        OATPP_LOGD("ModbusService", "  device_id: %s",
                   createDto->device_id ? createDto->device_id->c_str() : "null");
        OATPP_LOGD("ModbusService", "  slave_id: %s",
                   createDto->slave_id ? createDto->slave_id->c_str() : "null");
        OATPP_LOGD("ModbusService", "  device_name: %s",
                   createDto->device_name ? createDto->device_name->c_str() : "null");
        OATPP_LOGD("ModbusService", "  model: %s",
                   createDto->model ? createDto->model->c_str() : "null");

        auto user = m_authService->validateToken(authHeader);
        if (!user) {
            OATPP_LOGE("ModbusService", "Invalid token for device creation");
            throw Status::CODE_401;
        }

        if (!createDto->device_id || !createDto->slave_id || !createDto->device_name) {
            OATPP_LOGE("ModbusService", "Missing required fields");
            throw Status::CODE_400;
        }

        int slaveId;
        try {
            slaveId = std::stoi(createDto->slave_id->std_str());
            if (slaveId < 1 || slaveId > 247) {
                OATPP_LOGE("ModbusService", "Invalid slave ID: %d (out of range 1-247)", slaveId);
                throw Status::CODE_400;
            }
        } catch (const std::exception& e) {
            OATPP_LOGE("ModbusService", "Invalid slave ID format: %s",
                       createDto->slave_id->c_str());
            throw Status::CODE_400;
        }

        try {
            auto existingDevice = m_database->getDeviceByDeviceId(createDto->device_id);
            if (existingDevice->hasMoreToFetch()) {
                OATPP_LOGE("ModbusService", "Device ID already exists: %s", createDto->device_id->c_str());
                throw Status::CODE_409;
            }

            OATPP_LOGD("ModbusService", "Creating device with parameters:");
            OATPP_LOGD("ModbusService", "  device_id: %s", createDto->device_id->c_str());
            OATPP_LOGD("ModbusService", "  slave_id: %d", slaveId);
            OATPP_LOGD("ModbusService", "  device_name: %s", createDto->device_name->c_str());
            OATPP_LOGD("ModbusService", "  model: %s", createDto->model ? createDto->model->c_str() : "null");

            auto result = m_database->createDevice(
                createDto->device_id,
                slaveId,
                createDto->device_name,
                createDto->model ? createDto->model : ""
                );

            if (!result->isSuccess()) {
                OATPP_LOGE("ModbusService", "Database error during device creation: %s",
                           result->getErrorMessage() ? result->getErrorMessage()->c_str() : "Unknown error");
                throw Status::CODE_500;
            }

            auto deviceResult = m_database->getDeviceByDeviceId(createDto->device_id);
            if (!deviceResult->hasMoreToFetch()) {
                OATPP_LOGE("ModbusService", "Device created but not found: %s", createDto->device_id->c_str());
                throw Status::CODE_500;
            }

            auto device = deviceResult->fetch<oatpp::Object<DeviceDto>>();
            OATPP_LOGD("ModbusService", "Device created successfully: id=%d, device_id=%s, slave_id=%d",
                       device->id, device->device_id->c_str(), device->slave_id);

            return device;

        } catch (const std::exception& e) {
            OATPP_LOGE("ModbusService", "Exception during device creation: %s", e.what());
            throw Status::CODE_500;
        }
    }

    // Register operations
    oatpp::Object<RegisterDto> readRegister(
        const oatpp::Int32& deviceId,
        const oatpp::String& registerType,
        const oatpp::Int32& address,
        const oatpp::String& authHeader) {

        OATPP_LOGD("ModbusService", "Reading register: device=%d, type=%s, address=%d",
                   deviceId, registerType->c_str(), address);

        auto user = m_authService->validateToken(authHeader);
        if (!user) {
            OATPP_LOGE("ModbusService", "Invalid token for register read");
            throw Status::CODE_401;
        }

        validateDevice(deviceId);
        validateRegisterAccess(deviceId, registerType, address, false);

        auto result = m_database->getRegister(deviceId, registerType, address);
        if (!result->hasMoreToFetch()) {
            OATPP_LOGE("ModbusService", "Register not found");
            throw Status::CODE_404;
        }

        auto reg = result->fetch<oatpp::Object<RegisterDto>>();
        if (!reg) {
            OATPP_LOGE("ModbusService", "Failed to fetch register data");
            throw Status::CODE_500;
        }

        logRegisterAccess(reg->id, user->id, reg->value, reg->value, "READ");
        return reg;
    }

    oatpp::Vector<oatpp::Object<RegisterDto>> readRegisters(
        const oatpp::Object<RegisterBatchReadDto>& readDto,
        const oatpp::String& authHeader) {

        auto user = m_authService->validateToken(authHeader);
        if (!user) {
            OATPP_LOGE("ModbusService", "Invalid token for batch read");
            throw Status::CODE_401;
        }

        if (!readDto || !readDto->addresses) {
            OATPP_LOGE("ModbusService", "Invalid batch read request data");
            throw Status::CODE_400;
        }

        validateDevice(readDto->device_id);

        auto registers = oatpp::Vector<oatpp::Object<RegisterDto>>::createShared();

        for(auto& address : *readDto->addresses) {
            try {
                auto reg = readRegister(readDto->device_id, readDto->register_type, address, authHeader);
                registers->push_back(reg);
            } catch (const Status& status) {
                if (status.code == 404) {
                    OATPP_LOGW("ModbusService", "Register not found in batch: device=%d, type=%s, address=%d",
                               readDto->device_id, readDto->register_type->c_str(), address);
                    continue;
                }
                throw;
            }
        }

        if (registers->empty()) {
            OATPP_LOGE("ModbusService", "No valid registers found in batch");
            throw Status::CODE_404;
        }

        return registers;
    }

    oatpp::Object<StatusDto> writeRegister(
        const oatpp::Object<RegisterWriteDto>& writeDto,
        const oatpp::String& authHeader) {

        if (!writeDto) {
            OATPP_LOGE("ModbusService", "Invalid write request data");
            throw Status::CODE_400;
        }

        OATPP_LOGD("ModbusService", "Writing register: device=%d, type=%s, address=%d, value=%d",
                   writeDto->device_id, writeDto->register_type->c_str(),
                   writeDto->address, writeDto->value);

        auto user = m_authService->validateToken(authHeader);
        if (!user) {
            OATPP_LOGE("ModbusService", "Invalid token for register write");
            throw Status::CODE_401;
        }

        validateDevice(writeDto->device_id);
        validateRegisterAccess(writeDto->device_id, writeDto->register_type, writeDto->address, true);

        auto result = m_database->getRegister(writeDto->device_id, writeDto->register_type, writeDto->address);
        auto reg = result->fetch<oatpp::Object<RegisterDto>>();

        if (writeDto->value < reg->min_value || writeDto->value > reg->max_value) {
            OATPP_LOGE("ModbusService", "Value out of range: %d (min=%d, max=%d)",
                       writeDto->value, reg->min_value, reg->max_value);
            throw Status::CODE_400;
        }

        try {
            // TODO: Implement actual Modbus communication here
            auto oldValue = reg->value;
            m_database->updateRegister(writeDto->device_id, writeDto->register_type,
                                       writeDto->address, writeDto->value);

            logRegisterAccess(reg->id, user->id, oldValue, writeDto->value, "WRITE");
        } catch (const std::exception& e) {
            OATPP_LOGE("ModbusService", "Failed to write register: %s", e.what());
            throw Status::CODE_500;
        }

        return StatusDto::createSuccess("Register updated successfully");
    }

    oatpp::Object<StatusDto> writeRegisters(
        const oatpp::Object<RegisterBatchWriteDto>& writeDto,
        const oatpp::String& authHeader) {

        auto user = m_authService->validateToken(authHeader);
        if (!user) {
            OATPP_LOGE("ModbusService", "Invalid token for batch write");
            throw Status::CODE_401;
        }

        if (!writeDto || !writeDto->registers) {
            OATPP_LOGE("ModbusService", "Invalid batch write data");
            throw Status::CODE_400;
        }

        for(auto& reg : *writeDto->registers) {
            try {
                writeRegister(reg, authHeader);
            } catch (const Status& status) {
                if (status.code == 404) {
                    OATPP_LOGW("ModbusService", "Register not found in batch write: device=%d, type=%s, address=%d",
                               reg->device_id, reg->register_type->c_str(), reg->address);
                    continue;
                }
                throw;
            }
        }

        return StatusDto::createSuccess("Registers updated successfully");
    }

    oatpp::Object<StatusDto> getDeviceStatus(
        const oatpp::String& deviceId,
        const oatpp::String& authHeader) {

        OATPP_LOGD("ModbusService", "Checking device status: %s", deviceId->c_str());

        auto user = m_authService->validateToken(authHeader);
        if (!user) {
            OATPP_LOGE("ModbusService", "Invalid token for device status check");
            throw Status::CODE_401;
        }

        auto deviceResult = m_database->getDeviceByDeviceId(deviceId);
        if (!deviceResult->hasMoreToFetch()) {
            OATPP_LOGE("ModbusService", "Device not found: %s", deviceId->c_str());
            throw Status::CODE_404;
        }

        auto device = deviceResult->fetch<oatpp::Object<DeviceDto>>();
        auto now = std::time(nullptr);
        auto timeDiff = now - device->last_seen;

        if (device->status != "ONLINE") {
            std::string msg = "Device is " + device->status->std_str();
            OATPP_LOGW("ModbusService", "Device not online: %s, status: %s",
                       deviceId->c_str(), device->status->c_str());
            return StatusDto::createWarning(msg.c_str());
        }

        if (timeDiff > 300) { // 5 minutes
            std::string msg = "Device has been inactive for ";
            msg += std::to_string(timeDiff);
            msg += " seconds";
            OATPP_LOGW("ModbusService", "Device inactive: %s, time diff: %d",
                       deviceId->c_str(), timeDiff);
            return StatusDto::createWarning(msg.c_str());
        }

        OATPP_LOGD("ModbusService", "Device online and active: %s", deviceId->c_str());
        return StatusDto::createSuccess("Device is online and active");
    }



    oatpp::Vector<oatpp::Object<DeviceDto>> getAllDevices(
        const oatpp::String& authHeader) {

        auto user = m_authService->validateToken(authHeader);
        if (!user) {
            OATPP_LOGE("ModbusService", "Invalid token for device list request");
            throw Status::CODE_401;
        }

        auto result = m_database->getAllDevices();
        return result->fetch<oatpp::Vector<oatpp::Object<DeviceDto>>>();
    }

    oatpp::Vector<oatpp::Object<DeviceDto>> getDevice(
        const oatpp::String& deviceId,
        const oatpp::String& authHeader) {

        auto user = m_authService->validateToken(authHeader);
        if (!user) {
            OATPP_LOGE("ModbusService", "Invalid token for device info request");
            throw Status::CODE_401;
        }

        auto result = m_database->getDeviceByDeviceId(deviceId);
        if (!result->hasMoreToFetch()) {
            OATPP_LOGE("ModbusService", "Device not found: %s", deviceId->c_str());
            throw Status::CODE_404;
        }

        return result->fetch<oatpp::Vector<oatpp::Object<DeviceDto>>>();
    }

    oatpp::Object<StatusDto> updateDeviceStatus(
        const oatpp::String& deviceId,
        const oatpp::String& status,
        const oatpp::String& authHeader) {

        auto user = m_authService->validateToken(authHeader);
        if (!user) {
            OATPP_LOGE("ModbusService", "Invalid token for device status update");
            throw Status::CODE_401;
        }

        auto deviceResult = m_database->getDeviceByDeviceId(deviceId);
        if (!deviceResult->hasMoreToFetch()) {
            OATPP_LOGE("ModbusService", "Device not found: %s", deviceId->c_str());
            throw Status::CODE_404;
        }

        auto device = deviceResult->fetch<oatpp::Object<DeviceDto>>();

        if (status != "ONLINE" && status != "OFFLINE" && status != "ERROR") {
            OATPP_LOGE("ModbusService", "Invalid status value: %s", status->c_str());
            throw Status::CODE_400;
        }

        try {
            m_database->updateDeviceStatus(device->id, status);
            if (status == "ONLINE") {
                m_database->updateDeviceLastSeen(device->id);
            }
            return StatusDto::createSuccess("Device status updated successfully");
        } catch (const std::exception& e) {
            OATPP_LOGE("ModbusService", "Failed to update device status: %s", e.what());
            throw Status::CODE_500;
        }
    }

    oatpp::Object<StatusDto> updateDeviceIpAndPort(
        const oatpp::String& deviceId,
        const oatpp::String& ipAddress,
        const oatpp::Int32& port,
        const oatpp::String& authHeader) {

        auto user = m_authService->validateToken(authHeader);
        if (!user) {
            OATPP_LOGE("ModbusService", "Invalid token for device network update");
            throw Status::CODE_401;
        }

        if (port < 1 || port > 65535) {
            OATPP_LOGE("ModbusService", "Invalid port number: %d", port);
            throw Status::CODE_400;
        }

        auto deviceResult = m_database->getDeviceByDeviceId(deviceId);
        if (!deviceResult->hasMoreToFetch()) {
            OATPP_LOGE("ModbusService", "Device not found: %s", deviceId->c_str());
            throw Status::CODE_404;
        }

        auto device = deviceResult->fetch<oatpp::Object<DeviceDto>>();

        try {
            m_database->updateDeviceIpAndPort(device->id, ipAddress, port);
            return StatusDto::createSuccess("Device network settings updated successfully");
        } catch (const std::exception& e) {
            OATPP_LOGE("ModbusService", "Failed to update device network settings: %s", e.what());
            throw Status::CODE_500;
        }
    }
};

#endif /* MODBUS_SERVICE_HPP */
