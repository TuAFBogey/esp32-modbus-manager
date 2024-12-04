#ifndef DATABASE_COMPONENT_HPP
#define DATABASE_COMPONENT_HPP

#include "db/AuthDb.hpp"
#include "db/ModbusDb.hpp"
#include "oatpp-sqlite/orm.hpp"
#include "oatpp/core/macro/component.hpp"

/**
 * Database component that creates and holds database-related components
 */
class DatabaseComponent {
public:
    /**
     * Create database connection provider
     */
    OATPP_CREATE_COMPONENT(std::shared_ptr<oatpp::provider::Provider<oatpp::sqlite::Connection>>, dbConnectionProvider)([] {
        /* Create database-specific ConnectionProvider */
        auto connectionProvider = std::make_shared<oatpp::sqlite::ConnectionProvider>(DATABASE_FILE);

        /* Create database-specific ConnectionPool */
        return oatpp::sqlite::ConnectionPool::createShared(
            connectionProvider,
            10, /* max-connections */
            std::chrono::seconds(5) /* connection TTL */
            );
    }());

    /**
     * Create AuthDb client
     */
    OATPP_CREATE_COMPONENT(std::shared_ptr<AuthDb>, authDb)([] {
        /* Get database ConnectionProvider component */
        OATPP_COMPONENT(std::shared_ptr<oatpp::provider::Provider<oatpp::sqlite::Connection>>, connectionProvider);

        /* Create database-specific Executor */
        auto executor = std::make_shared<oatpp::sqlite::Executor>(connectionProvider);

        /* Create AuthDb client */
        return std::make_shared<AuthDb>(executor);
    }());

    /**
     * Create ModbusDb client
     */
    OATPP_CREATE_COMPONENT(std::shared_ptr<ModbusDb>, modbusDb)([] {
        /* Get database ConnectionProvider component */
        OATPP_COMPONENT(std::shared_ptr<oatpp::provider::Provider<oatpp::sqlite::Connection>>, connectionProvider);

        /* Create database-specific Executor */
        auto executor = std::make_shared<oatpp::sqlite::Executor>(connectionProvider);

        /* Create ModbusDb client */
        return std::make_shared<ModbusDb>(executor);
    }());
};

#endif /* DATABASE_COMPONENT_HPP */
