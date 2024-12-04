#include "component/AppComponent.hpp"
#include "component/SwaggerComponent.hpp"
#include "controller/AuthController.hpp"
#include "controller/ModbusController.hpp"
#include "controller/SwaggerController.hpp"
#include "util/ErrorHandler.hpp"
#include "oatpp-swagger/Controller.hpp"
#include "oatpp/network/Server.hpp"
#include <iostream>

void run() {
    /* Register Components */
    AppComponent components;
    SwaggerComponent swaggerComponents;

    /* Get router component */
    OATPP_COMPONENT(std::shared_ptr<oatpp::web::server::HttpRouter>, router);
    OATPP_COMPONENT(std::shared_ptr<oatpp::data::mapping::ObjectMapper>, objectMapper);
    OATPP_COMPONENT(std::shared_ptr<AuthService>, authService);
    OATPP_COMPONENT(std::shared_ptr<ModbusService>, modbusService);

    /* Create Controllers */
    auto authController = std::make_shared<AuthController>(objectMapper, authService);
    auto modbusController = std::make_shared<ModbusController>(objectMapper, modbusService);
    auto swaggerController = std::make_shared<SwaggerController>(objectMapper);

    /* Add endpoints to router */
    authController->addEndpointsToRouter(router);
    modbusController->addEndpointsToRouter(router);
    swaggerController->addEndpointsToRouter(router);

    /* Add swagger-ui endpoints */
    auto docEndpoints = oatpp::swagger::Controller::Endpoints::createShared();
    docEndpoints->pushBackAll(authController->getEndpoints());
    docEndpoints->pushBackAll(modbusController->getEndpoints());

    auto swaggerUiController = oatpp::swagger::Controller::createShared(docEndpoints);
    swaggerUiController->addEndpointsToRouter(router);

    /* Get connection handler component */
    OATPP_COMPONENT(std::shared_ptr<oatpp::network::ConnectionHandler>, connectionHandler);

    /* Get connection provider component */
    OATPP_COMPONENT(std::shared_ptr<oatpp::network::ServerConnectionProvider>, connectionProvider);

    /* Create server */
    oatpp::network::Server server(connectionProvider, connectionHandler);

    OATPP_LOGI("Server", "Running on port %s", connectionProvider->getProperty("port").toString()->c_str());

    server.run();
}

/**
 * Main
 */
int main() {
    oatpp::base::Environment::init();

    run();

    /* Print how much objects were created during app running, and what have left-probably leaked */
    /* Disable object counting for release builds using '-DOATPP_DISABLE_ENV_OBJECT_COUNTERS' flag for better performance */
    std::cout << "\nEnvironment:\n";
    std::cout << "objectsCount = " << oatpp::base::Environment::getObjectsCount() << "\n";
    std::cout << "objectsCreated = " << oatpp::base::Environment::getObjectsCreated() << "\n\n";

    oatpp::base::Environment::destroy();

    return 0;
}
