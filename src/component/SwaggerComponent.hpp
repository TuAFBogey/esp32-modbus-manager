#ifndef SWAGGER_COMPONENT_HPP
#define SWAGGER_COMPONENT_HPP

#include "oatpp-swagger/Model.hpp"
#include "oatpp-swagger/Resources.hpp"
#include "oatpp/core/macro/component.hpp"

class SwaggerComponent {
public:
    /**
     *  General API docs info
     */
    OATPP_CREATE_COMPONENT(std::shared_ptr<oatpp::swagger::DocumentInfo>, swaggerDocumentInfo)([] {
        oatpp::swagger::DocumentInfo::Builder builder;

        // Create security scheme
        auto authSchemes = std::make_shared<oatpp::swagger::SecurityScheme>();
        authSchemes->name = "Authorization";
        authSchemes->type = "apiKey";
        authSchemes->in = "header";
        authSchemes->description = "Bearer Authentication";

        builder
            .setTitle("ESP32 Modbus API")
            .setDescription(
                "REST API for ESP32 Modbus Register Management.\n\n"
                "Authentication:\n"
                "1. First get a token using /api/auth/login\n"
                "2. Click the 'Authorize' button at the top\n"
                "3. Enter your token with Bearer prefix\n"
                "4. Click 'Authorize' to save\n"
                )
            .setVersion("1.0.0")
            .setContactName("TuAFBogey")
            .setContactEmail("xxxx@gmail.com")
            .addServer("http://localhost:8000", "Local API")
            .addSecurityScheme("Authorization", authSchemes);

        return builder.build();
    }());

    /**
     *  Swagger-UI Resources
     */
    OATPP_CREATE_COMPONENT(std::shared_ptr<oatpp::swagger::Resources>, swaggerResources)([] {
        return oatpp::swagger::Resources::loadResources(OATPP_SWAGGER_RES_PATH);
    }());
};

#endif /* SWAGGER_COMPONENT_HPP */
