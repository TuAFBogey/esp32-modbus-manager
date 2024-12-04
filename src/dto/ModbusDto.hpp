#ifndef MODBUS_DTO_HPP
#define MODBUS_DTO_HPP

#include "oatpp/core/macro/codegen.hpp"
#include "oatpp/core/Types.hpp"
#include "dto/DeviceDto.hpp"

#include OATPP_CODEGEN_BEGIN(DTO)

class RegisterDto : public oatpp::DTO {
    DTO_INIT(RegisterDto, DTO)

    DTO_FIELD(Int32, id);
    DTO_FIELD_INFO(id) {
        info->description = "Register database ID";
    }

    DTO_FIELD(Int32, device_id, "device_id");
    DTO_FIELD_INFO(device_id) {
        info->description = "Device ID reference";
    }

    DTO_FIELD(String, register_type, "register_type");
    DTO_FIELD_INFO(register_type) {
        info->description = "Register type (COIL/DISCRETE_INPUT/INPUT/HOLDING)";
    }

    DTO_FIELD(Int32, address);
    DTO_FIELD_INFO(address) {
        info->description = "Register address";
    }

    DTO_FIELD(Int32, value);
    DTO_FIELD_INFO(value) {
        info->description = "Current value";
    }

    DTO_FIELD(Int32, min_value, "min_value");
    DTO_FIELD_INFO(min_value) {
        info->description = "Minimum allowed value";
    }

    DTO_FIELD(Int32, max_value, "max_value");
    DTO_FIELD_INFO(max_value) {
        info->description = "Maximum allowed value";
    }

    DTO_FIELD(Int32, default_value, "default_value");
    DTO_FIELD_INFO(default_value) {
        info->description = "Default value";
    }

    DTO_FIELD(Boolean, read_only, "read_only");
    DTO_FIELD_INFO(read_only) {
        info->description = "Read-only flag";
    }

    DTO_FIELD(String, description);
    DTO_FIELD_INFO(description) {
        info->description = "Register description";
    }

    DTO_FIELD(Int64, updated_at, "updated_at");
    DTO_FIELD_INFO(updated_at) {
        info->description = "Last update timestamp";
    }

    DTO_FIELD(Int64, created_at, "created_at");
    DTO_FIELD_INFO(created_at) {
        info->description = "Creation timestamp";
    }
};

class RegisterWriteDto : public oatpp::DTO {
    DTO_INIT(RegisterWriteDto, DTO)

    DTO_FIELD(Int32, device_id, "device_id");
    DTO_FIELD_INFO(device_id) {
        info->description = "Device ID";
    }

    DTO_FIELD(String, register_type, "register_type");
    DTO_FIELD_INFO(register_type) {
        info->description = "Register type (COIL/HOLDING)";
    }

    DTO_FIELD(Int32, address);
    DTO_FIELD_INFO(address) {
        info->description = "Register address";
    }

    DTO_FIELD(Int32, value);
    DTO_FIELD_INFO(value) {
        info->description = "Value to write";
    }
};

class RegisterBatchReadDto : public oatpp::DTO {
    DTO_INIT(RegisterBatchReadDto, DTO)

    DTO_FIELD(Int32, device_id, "device_id");
    DTO_FIELD_INFO(device_id) {
        info->description = "Device ID";
    }

    DTO_FIELD(String, register_type, "register_type");
    DTO_FIELD_INFO(register_type) {
        info->description = "Register type";
    }

    DTO_FIELD(Vector<Int32>, addresses);
    DTO_FIELD_INFO(addresses) {
        info->description = "List of register addresses to read";
    }
};

class RegisterBatchWriteDto : public oatpp::DTO {
    DTO_INIT(RegisterBatchWriteDto, DTO)

    DTO_FIELD(Vector<Object<RegisterWriteDto>>, registers);
    DTO_FIELD_INFO(registers) {
        info->description = "List of register write operations";
    }
};

#include OATPP_CODEGEN_END(DTO)

#endif /* MODBUS_DTO_HPP */
