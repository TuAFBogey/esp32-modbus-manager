#ifndef APP_COMPONENT_HPP
#define APP_COMPONENT_HPP

#include "db/AuthDb.hpp"
#include "db/ModbusDb.hpp"
#include "service/AuthService.hpp"
#include "service/ModbusService.hpp"

#include "oatpp/web/server/HttpConnectionHandler.hpp"
#include "oatpp/network/tcp/server/ConnectionProvider.hpp"
#include "oatpp/parser/json/mapping/ObjectMapper.hpp"
#include "oatpp/core/macro/component.hpp"

class AppComponent {
public:
    /**
     * Create ObjectMapper component to serialize/deserialize DTOs
     */
    OATPP_CREATE_COMPONENT(std::shared_ptr<oatpp::data::mapping::ObjectMapper>, apiObjectMapper)([] {
        return oatpp::parser::json::mapping::ObjectMapper::createShared();
    }());

    /**
     * Create database components
     */
    OATPP_CREATE_COMPONENT(std::shared_ptr<oatpp::sqlite::ConnectionProvider>, dbConnectionProvider)([] {
        return std::make_shared<oatpp::sqlite::ConnectionProvider>("db.sqlite");
    }());

    OATPP_CREATE_COMPONENT(std::shared_ptr<oatpp::sqlite::Executor>, dbExecutor)([] {
        OATPP_COMPONENT(std::shared_ptr<oatpp::sqlite::ConnectionProvider>, connectionProvider);
        return std::make_shared<oatpp::sqlite::Executor>(connectionProvider);
    }());

    /**
     * Create AuthDb component
     */
    OATPP_CREATE_COMPONENT(std::shared_ptr<AuthDb>, authDb)([] {
        OATPP_COMPONENT(std::shared_ptr<oatpp::sqlite::Executor>, dbExecutor);
        return std::make_shared<AuthDb>(dbExecutor);
    }());

    /**
     * Create ModbusDb component
     */
    OATPP_CREATE_COMPONENT(std::shared_ptr<ModbusDb>, modbusDb)([] {
        OATPP_COMPONENT(std::shared_ptr<oatpp::sqlite::Executor>, dbExecutor);
        return std::make_shared<ModbusDb>(dbExecutor);
    }());

    /**
     * Create AuthService component
     */
    OATPP_CREATE_COMPONENT(std::shared_ptr<AuthService>, authService)([] {
        OATPP_COMPONENT(std::shared_ptr<AuthDb>, authDb);
        return std::make_shared<AuthService>(authDb);
    }());

    /**
     * Create ModbusService component
     */
    OATPP_CREATE_COMPONENT(std::shared_ptr<ModbusService>, modbusService)([] {
        OATPP_COMPONENT(std::shared_ptr<AuthService>, authService);
        OATPP_COMPONENT(std::shared_ptr<ModbusDb>, modbusDb);
        return std::make_shared<ModbusService>(authService, modbusDb);
    }());

    /**
     * Create ConnectionProvider component
     */
    OATPP_CREATE_COMPONENT(std::shared_ptr<oatpp::network::ServerConnectionProvider>, serverConnectionProvider)([] {
        return oatpp::network::tcp::server::ConnectionProvider::createShared({"0.0.0.0", 8000, oatpp::network::Address::IP_4});
    }());

    /**
     * Create Router component
     */
    OATPP_CREATE_COMPONENT(std::shared_ptr<oatpp::web::server::HttpRouter>, httpRouter)([] {
        return oatpp::web::server::HttpRouter::createShared();
    }());

    /**
     * Create ConnectionHandler component
     */
    OATPP_CREATE_COMPONENT(std::shared_ptr<oatpp::network::ConnectionHandler>, serverConnectionHandler)([] {
        OATPP_COMPONENT(std::shared_ptr<oatpp::web::server::HttpRouter>, router);
        return oatpp::web::server::HttpConnectionHandler::createShared(router);
    }());
};

#endif /* APP_COMPONENT_HPP */
