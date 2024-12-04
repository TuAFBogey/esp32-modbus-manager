#ifndef SWAGGER_CONTROLLER_HPP
#define SWAGGER_CONTROLLER_HPP

#include "oatpp/web/server/api/ApiController.hpp"
#include "oatpp/parser/json/mapping/ObjectMapper.hpp"
#include "oatpp/core/macro/codegen.hpp"
#include "oatpp/core/macro/component.hpp"

#include OATPP_CODEGEN_BEGIN(ApiController) //<- Begin Codegen

class SwaggerController : public oatpp::web::server::api::ApiController {
public:
    SwaggerController(const std::shared_ptr<ObjectMapper>& objectMapper)
        : oatpp::web::server::api::ApiController(objectMapper)
    {}
public:

    static std::shared_ptr<SwaggerController> createShared(
        OATPP_COMPONENT(std::shared_ptr<ObjectMapper>, objectMapper)
        ){
        return std::make_shared<SwaggerController>(objectMapper);
    }

    ENDPOINT("GET", "/", root) {
        const char* html =
            "<html lang='en'>"
            "  <head>"
            "    <meta charset=utf-8/>"
            "  </head>"
            "  <body>"
            "    <a href='swagger/ui'>REST API for ESP32 Modbus Register Management</a>"
            "  </body>"
            "</html>";
        auto response = createResponse(Status::CODE_200, html);
        response->putHeader(Header::CONTENT_TYPE, "text/html");
        return response;
    }

};

#include OATPP_CODEGEN_END(ApiController) //<- End Codegen

#endif //SWAGGER_CONTROLLER_HPP
