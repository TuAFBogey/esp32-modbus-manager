#ifndef MODBUS_DB_HPP
#define MODBUS_DB_HPP

#include "dto/DeviceDto.hpp"
#include "dto/ModbusDto.hpp"
#include "oatpp-sqlite/orm.hpp"

#include OATPP_CODEGEN_BEGIN(DbClient)

class ModbusDb : public oatpp::orm::DbClient {
public:
    ModbusDb(const std::shared_ptr<oatpp::orm::Executor>& executor)
        : oatpp::orm::DbClient(executor)
    {
        oatpp::orm::SchemaMigration migration(executor);
        migration.addFile(1, DATABASE_MIGRATIONS "/init.sql");
        migration.migrate();

        auto version = executor->getSchemaVersion();
        OATPP_LOGD("ModbusDb", "Migration - OK. Version=%d.", version);
    }

    // Device operations
    QUERY(createDevice,
          "INSERT INTO ModbusDevice "
          "(device_id, slave_id, device_name, model) "
          "VALUES (:deviceId, :slaveId, :deviceName, :model); "
          "SELECT changes() as CHANGES",
          PARAM(oatpp::String, deviceId),
          PARAM(oatpp::Int32, slaveId),
          PARAM(oatpp::String, deviceName),
          PARAM(oatpp::String, model))

    QUERY(getDeviceById,
          "SELECT "
          "  id as \"id\", "
          "  device_id as \"device_id\", "
          "  slave_id as \"slave_id\", "
          "  ip_address as \"ip_address\", "
          "  port as \"port\", "
          "  device_name as \"device_name\", "
          "  model as \"model\", "
          "  last_seen as \"last_seen\", "
          "  status as \"status\", "
          "  created_at as \"created_at\" "
          "FROM ModbusDevice WHERE id=:id",
          PARAM(oatpp::Int32, id))

    QUERY(getDeviceByDeviceId,
          "SELECT "
          "  id as \"id\", "
          "  device_id as \"device_id\", "
          "  slave_id as \"slave_id\", "
          "  ip_address as \"ip_address\", "
          "  port as \"port\", "
          "  device_name as \"device_name\", "
          "  model as \"model\", "
          "  last_seen as \"last_seen\", "
          "  status as \"status\", "
          "  created_at as \"created_at\" "
          "FROM ModbusDevice WHERE device_id=:deviceId",
          PARAM(oatpp::String, deviceId))

    QUERY(updateDeviceLastSeen,
          "UPDATE ModbusDevice SET "
          "last_seen=strftime('%s','now') "
          "WHERE id=:id",
          PARAM(oatpp::Int32, id))

    QUERY(updateDeviceStatus,
          "UPDATE ModbusDevice SET "
          "status=:status "
          "WHERE id=:id",
          PARAM(oatpp::Int32, id),
          PARAM(oatpp::String, status))

    QUERY(updateDeviceIpAndPort,
          "UPDATE ModbusDevice SET "
          "ip_address=:ipAddress, port=:port "
          "WHERE id=:id",
          PARAM(oatpp::Int32, id),
          PARAM(oatpp::String, ipAddress),
          PARAM(oatpp::Int32, port))

    QUERY(getAllDevices,
          "SELECT "
          "  id as \"id\", "
          "  device_id as \"device_id\", "
          "  slave_id as \"slave_id\", "
          "  ip_address as \"ip_address\", "
          "  port as \"port\", "
          "  device_name as \"device_name\", "
          "  model as \"model\", "
          "  last_seen as \"last_seen\", "
          "  status as \"status\", "
          "  created_at as \"created_at\" "
          "FROM ModbusDevice")

    // Register operations
    QUERY(createRegister,
          "INSERT INTO ModbusRegister "
          "(device_id, register_type, address, value, min_value, max_value, default_value, read_only, description) "
          "VALUES (:deviceId, :registerType, :address, :value, :minValue, :maxValue, :defaultValue, :readOnly, :description)",
          PARAM(oatpp::Int32, deviceId),
          PARAM(oatpp::String, registerType),
          PARAM(oatpp::Int32, address),
          PARAM(oatpp::Int32, value),
          PARAM(oatpp::Int32, minValue),
          PARAM(oatpp::Int32, maxValue),
          PARAM(oatpp::Int32, defaultValue),
          PARAM(oatpp::Boolean, readOnly),
          PARAM(oatpp::String, description))

    QUERY(getRegister,
          "SELECT "
          "  id as \"id\", "
          "  device_id as \"device_id\", "
          "  register_type as \"register_type\", "
          "  address as \"address\", "
          "  value as \"value\", "
          "  min_value as \"min_value\", "
          "  max_value as \"max_value\", "
          "  default_value as \"default_value\", "
          "  read_only as \"read_only\", "
          "  description as \"description\", "
          "  updated_at as \"updated_at\", "
          "  created_at as \"created_at\" "
          "FROM ModbusRegister "
          "WHERE device_id=:deviceId AND register_type=:registerType AND address=:address",
          PARAM(oatpp::Int32, deviceId),
          PARAM(oatpp::String, registerType),
          PARAM(oatpp::Int32, address))

    QUERY(updateRegister,
          "UPDATE ModbusRegister SET "
          "value=:value, updated_at=strftime('%s','now') "
          "WHERE device_id=:deviceId AND register_type=:registerType AND address=:address",
          PARAM(oatpp::Int32, deviceId),
          PARAM(oatpp::String, registerType),
          PARAM(oatpp::Int32, address),
          PARAM(oatpp::Int32, value))

    QUERY(getRegistersForDevice,
          "SELECT "
          "  id as \"id\", "
          "  device_id as \"device_id\", "
          "  register_type as \"register_type\", "
          "  address as \"address\", "
          "  value as \"value\", "
          "  min_value as \"min_value\", "
          "  max_value as \"max_value\", "
          "  default_value as \"default_value\", "
          "  read_only as \"read_only\", "
          "  description as \"description\", "
          "  updated_at as \"updated_at\", "
          "  created_at as \"created_at\" "
          "FROM ModbusRegister WHERE device_id=:deviceId",
          PARAM(oatpp::Int32, deviceId))

    QUERY(getAllRegisters,
          "SELECT "
          "  id as \"id\", "
          "  device_id as \"device_id\", "
          "  register_type as \"register_type\", "
          "  address as \"address\", "
          "  value as \"value\", "
          "  min_value as \"min_value\", "
          "  max_value as \"max_value\", "
          "  default_value as \"default_value\", "
          "  read_only as \"read_only\", "
          "  description as \"description\", "
          "  updated_at as \"updated_at\", "
          "  created_at as \"created_at\" "
          "FROM ModbusRegister")

    QUERY(createHistory,
          "INSERT INTO RegisterHistory "
          "(register_id, user_id, old_value, new_value, action) "
          "VALUES (:registerId, :userId, :oldValue, :newValue, :action)",
          PARAM(oatpp::Int32, registerId),
          PARAM(oatpp::Int32, userId),
          PARAM(oatpp::Int32, oldValue),
          PARAM(oatpp::Int32, newValue),
          PARAM(oatpp::String, action))

    QUERY(getRegisterHistory,
          "SELECT * FROM RegisterHistory "
          "WHERE register_id=:registerId "
          "ORDER BY timestamp DESC LIMIT :limit",
          PARAM(oatpp::Int32, registerId),
          PARAM(oatpp::UInt32, limit))
};

#include OATPP_CODEGEN_END(DbClient)

#endif /* MODBUS_DB_HPP */
