#ifndef ERROR_HANDLER_HPP
#define ERROR_HANDLER_HPP

#include "dto/StatusDto.hpp"
#include "oatpp/web/server/handler/ErrorHandler.hpp"
#include "oatpp/web/protocol/http/outgoing/ResponseFactory.hpp"

/**
 * Custom error handler for the application
 */
class ErrorHandler : public oatpp::web::server::handler::ErrorHandler {
private:
    typedef oatpp::web::protocol::http::outgoing::Response OutgoingResponse;
    typedef oatpp::web::protocol::http::Status Status;
    typedef oatpp::web::protocol::http::outgoing::ResponseFactory ResponseFactory;
    
    std::shared_ptr<oatpp::data::mapping::ObjectMapper> m_objectMapper;
    
public:
    ErrorHandler(const std::shared_ptr<oatpp::data::mapping::ObjectMapper>& objectMapper)
        : m_objectMapper(objectMapper)
    {}
    
    std::shared_ptr<OutgoingResponse>
    handleError(const Status& status, const oatpp::String& message, const Headers& headers) override {
        auto error = StatusDto::createShared();
        error->status = "ERROR";
        error->code = status.code;
        error->message = message;
        
        auto response = ResponseFactory::createResponse(status, error, m_objectMapper);
        
        for(const auto& pair : headers.getAll()) {
            response->putHeader(pair.first.toString(), pair.second.toString());
        }
        
        return response;
    }
};

#endif /* ERROR_HANDLER_HPP */
