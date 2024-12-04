#ifndef DEVICE_DTO_HPP
#define DEVICE_DTO_HPP

#include "oatpp/core/macro/codegen.hpp"
#include "oatpp/core/Types.hpp"

#include OATPP_CODEGEN_BEGIN(DTO)

class CreateDeviceDto : public oatpp::DTO {
    DTO_INIT(CreateDeviceDto, DTO)

    DTO_FIELD(String, device_id, "device_id");
    DTO_FIELD_INFO(device_id) {
        info->description = "Unique device identifier";
    }

    DTO_FIELD(String, slave_id, "slave_id");
    DTO_FIELD_INFO(slave_id) {
        info->description = "Modbus slave ID (1-247)";
        info->pattern = "^([1-9]|[1-9][0-9]|1[0-9]{2}|2[0-3][0-9]|24[0-7])$";
    }

    DTO_FIELD(String, device_name, "device_name");
    DTO_FIELD_INFO(device_name) {
        info->description = "Device name";
    }

    DTO_FIELD(String, model);
    DTO_FIELD_INFO(model) {
        info->description = "Device model";
    }
};

class DeviceDto : public oatpp::DTO {
    DTO_INIT(DeviceDto, DTO)

    DTO_FIELD(Int32, id);
    DTO_FIELD_INFO(id) {
        info->description = "Device database ID";
    }

    DTO_FIELD(String, device_id, "device_id");
    DTO_FIELD_INFO(device_id) {
        info->description = "Unique device identifier";
    }

    DTO_FIELD(Int32, slave_id, "slave_id");
    DTO_FIELD_INFO(slave_id) {
        info->description = "Modbus slave ID (1-247)";
    }

    DTO_FIELD(String, ip_address, "ip_address");
    DTO_FIELD_INFO(ip_address) {
        info->description = "Device IP address";
    }

    DTO_FIELD(Int32, port);
    DTO_FIELD_INFO(port) {
        info->description = "Modbus TCP port";
    }

    DTO_FIELD(String, device_name, "device_name");
    DTO_FIELD_INFO(device_name) {
        info->description = "Device name";
    }

    DTO_FIELD(String, model);
    DTO_FIELD_INFO(model) {
        info->description = "Device model";
    }

    DTO_FIELD(Int64, last_seen, "last_seen");
    DTO_FIELD_INFO(last_seen) {
        info->description = "Last seen timestamp";
    }

    DTO_FIELD(String, status);
    DTO_FIELD_INFO(status) {
        info->description = "Device status (ONLINE/OFFLINE/ERROR)";
    }

    DTO_FIELD(Int64, created_at, "created_at");
    DTO_FIELD_INFO(created_at) {
        info->description = "Creation timestamp";
    }
};

#include OATPP_CODEGEN_END(DTO)

#endif /* DEVICE_DTO_HPP */
