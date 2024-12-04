#ifndef MODBUS_CONTROLLER_HPP
#define MODBUS_CONTROLLER_HPP

#include "service/ModbusService.hpp"
#include "dto/ModbusDto.hpp"
#include "dto/StatusDto.hpp"
#include "oatpp/web/server/api/ApiController.hpp"
#include "oatpp/core/macro/codegen.hpp"
#include "oatpp/core/macro/component.hpp"

#include OATPP_CODEGEN_BEGIN(ApiController)

class ModbusController : public oatpp::web::server::api::ApiController {
private:
    std::shared_ptr<ModbusService> m_modbusService;

public:
    ModbusController(const std::shared_ptr<ObjectMapper>& objectMapper,
                     const std::shared_ptr<ModbusService>& modbusService)
        : oatpp::web::server::api::ApiController(objectMapper)
        , m_modbusService(modbusService)
    {}

    static std::shared_ptr<ModbusController> createShared(
        OATPP_COMPONENT(std::shared_ptr<ObjectMapper>, objectMapper),
        OATPP_COMPONENT(std::shared_ptr<ModbusService>, modbusService))
    {
        return std::make_shared<ModbusController>(objectMapper, modbusService);
    }

    ENDPOINT_INFO(createDevice) {
        info->summary = "Create new modbus device";
        info->addConsumes<Object<CreateDeviceDto>>("application/json");
        info->addResponse<Object<DeviceDto>>(Status::CODE_200, "application/json");
        info->addResponse<Object<StatusDto>>(Status::CODE_400, "application/json");
        info->addResponse<Object<StatusDto>>(Status::CODE_401, "application/json");
        info->addSecurityRequirement("Authorization");
        info->authorization = "Authorization";
    }
    ENDPOINT("POST", "api/device", createDevice,
             BODY_DTO(Object<CreateDeviceDto>, deviceDto),
             HEADER(String, authorization, "Authorization"))
    {
        return createDtoResponse(Status::CODE_200, m_modbusService->createDevice(deviceDto, authorization));
    }

    ENDPOINT_INFO(getAllDevices) {
        info->summary = "Get all devices";
        info->addResponse<oatpp::Vector<Object<DeviceDto>>>(Status::CODE_200, "application/json");
        info->addResponse<Object<StatusDto>>(Status::CODE_401, "application/json");
        info->addSecurityRequirement("Authorization");
        info->authorization = "Authorization";
    }
    ENDPOINT("GET", "api/device", getAllDevices,
             HEADER(String, authorization, "Authorization"))
    {
        return createDtoResponse(Status::CODE_200, m_modbusService->getAllDevices(authorization));
    }

    ENDPOINT_INFO(getDevice) {
        info->summary = "Get device by ID";
        info->addResponse<Object<DeviceDto>>(Status::CODE_200, "application/json");
        info->addResponse<Object<StatusDto>>(Status::CODE_401, "application/json");
        info->addResponse<Object<StatusDto>>(Status::CODE_404, "application/json");
        info->addSecurityRequirement("Authorization");
        info->authorization = "Authorization";
    }
    ENDPOINT("GET", "api/device/{deviceId}", getDevice,
             PATH(String, deviceId),
             HEADER(String, authorization, "Authorization"))
    {
        return createDtoResponse(Status::CODE_200, m_modbusService->getDevice(deviceId, authorization));
    }

    ENDPOINT_INFO(updateDeviceStatus) {
        info->summary = "Update device status";
        info->addResponse<Object<StatusDto>>(Status::CODE_200, "application/json");
        info->addResponse<Object<StatusDto>>(Status::CODE_400, "application/json");
        info->addResponse<Object<StatusDto>>(Status::CODE_401, "application/json");
        info->addResponse<Object<StatusDto>>(Status::CODE_404, "application/json");
        info->addSecurityRequirement("Authorization");
        info->authorization = "Authorization";
    }
    ENDPOINT("PUT", "api/device/{deviceId}/status/{status}", updateDeviceStatus,
             PATH(String, deviceId),
             PATH(String, status),
             HEADER(String, authorization, "Authorization"))
    {
        return createDtoResponse(Status::CODE_200, m_modbusService->updateDeviceStatus(deviceId, status, authorization));
    }

    ENDPOINT_INFO(getDeviceStatus) {
        info->summary = "Get device status";
        info->addResponse<Object<StatusDto>>(Status::CODE_200, "application/json");
        info->addResponse<Object<StatusDto>>(Status::CODE_401, "application/json");
        info->addResponse<Object<StatusDto>>(Status::CODE_404, "application/json");
        info->addSecurityRequirement("Authorization");
        info->authorization = "Authorization";
    }
    ENDPOINT("GET", "api/device/{deviceId}/status", getDeviceStatus,
             PATH(String, deviceId),
             HEADER(String, authorization, "Authorization"))
    {
        return createDtoResponse(Status::CODE_200, m_modbusService->getDeviceStatus(deviceId, authorization));
    }

    ENDPOINT_INFO(readRegister) {
        info->summary = "Read register value";
        info->addResponse<Object<RegisterDto>>(Status::CODE_200, "application/json");
        info->addResponse<Object<StatusDto>>(Status::CODE_401, "application/json");
        info->addResponse<Object<StatusDto>>(Status::CODE_404, "application/json");
        info->addSecurityRequirement("Authorization");
        info->authorization = "Authorization";
    }
    ENDPOINT("GET", "api/register/{deviceId}/{type}/{address}", readRegister,
             PATH(Int32, deviceId),
             PATH(String, type),
             PATH(Int32, address),
             HEADER(String, authorization, "Authorization"))
    {
        return createDtoResponse(Status::CODE_200, m_modbusService->readRegister(deviceId, type, address, authorization));
    }

    ENDPOINT_INFO(readRegisters) {
        info->summary = "Read multiple registers";
        info->addConsumes<Object<RegisterBatchReadDto>>("application/json");
        info->addResponse<oatpp::Vector<Object<RegisterDto>>>(Status::CODE_200, "application/json");
        info->addResponse<Object<StatusDto>>(Status::CODE_401, "application/json");
        info->addResponse<Object<StatusDto>>(Status::CODE_404, "application/json");
        info->addSecurityRequirement("Authorization");
        info->authorization = "Authorization";
    }
    ENDPOINT("POST", "api/register/read/batch", readRegisters,
             BODY_DTO(Object<RegisterBatchReadDto>, readDto),
             HEADER(String, authorization, "Authorization"))
    {
        return createDtoResponse(Status::CODE_200, m_modbusService->readRegisters(readDto, authorization));
    }

    ENDPOINT_INFO(writeRegister) {
        info->summary = "Write register value";
        info->addConsumes<Object<RegisterWriteDto>>("application/json");
        info->addResponse<Object<StatusDto>>(Status::CODE_200, "application/json");
        info->addResponse<Object<StatusDto>>(Status::CODE_400, "application/json");
        info->addResponse<Object<StatusDto>>(Status::CODE_401, "application/json");
        info->addResponse<Object<StatusDto>>(Status::CODE_403, "application/json");
        info->addResponse<Object<StatusDto>>(Status::CODE_404, "application/json");
        info->addSecurityRequirement("Authorization");
        info->authorization = "Authorization";
    }
    ENDPOINT("PUT", "api/register", writeRegister,
             BODY_DTO(Object<RegisterWriteDto>, writeDto),
             HEADER(String, authorization, "Authorization"))
    {
        return createDtoResponse(Status::CODE_200, m_modbusService->writeRegister(writeDto, authorization));
    }

    ENDPOINT_INFO(writeRegisters) {
        info->summary = "Write multiple registers";
        info->addConsumes<Object<RegisterBatchWriteDto>>("application/json");
        info->addResponse<Object<StatusDto>>(Status::CODE_200, "application/json");
        info->addResponse<Object<StatusDto>>(Status::CODE_400, "application/json");
        info->addResponse<Object<StatusDto>>(Status::CODE_401, "application/json");
        info->addResponse<Object<StatusDto>>(Status::CODE_403, "application/json");
        info->addResponse<Object<StatusDto>>(Status::CODE_404, "application/json");
        info->addSecurityRequirement("Authorization");
        info->authorization = "Authorization";
    }
    ENDPOINT("POST", "api/register/write/batch", writeRegisters,
             BODY_DTO(Object<RegisterBatchWriteDto>, writeDto),
             HEADER(String, authorization, "Authorization"))
    {
        return createDtoResponse(Status::CODE_200, m_modbusService->writeRegisters(writeDto, authorization));
    }
};

#include OATPP_CODEGEN_END(ApiController)

#endif /* MODBUS_CONTROLLER_HPP */
