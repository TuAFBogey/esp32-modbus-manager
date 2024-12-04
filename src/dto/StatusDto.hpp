#ifndef STATUS_DTO_HPP
#define STATUS_DTO_HPP

#include "oatpp/core/macro/codegen.hpp"
#include "oatpp/core/Types.hpp"

#include OATPP_CODEGEN_BEGIN(DTO)

/**
 * Data Transfer Object for API response status
 */
class StatusDto : public oatpp::DTO {

    DTO_INIT(StatusDto, DTO)

    /**
     * Status code of the response
     * Example: 200, 400, 401, 403, 404, 500
     */
    DTO_FIELD(Int32, code);
    DTO_FIELD_INFO(code) {
        info->description = "HTTP status code";
    }

    /**
     * Status type
     * Example: "SUCCESS", "ERROR", "WARNING"
     */
    DTO_FIELD(String, status);
    DTO_FIELD_INFO(status) {
        info->description = "Status type (SUCCESS, ERROR, WARNING)";
    }

    /**
     * Detailed message about the status
     * Example: "Operation completed successfully", "Invalid input parameters"
     */
    DTO_FIELD(String, message);
    DTO_FIELD_INFO(message) {
        info->description = "Detailed status message";
    }

    /**
     * Additional error details (optional)
     * Used for providing more specific error information
     */
    DTO_FIELD(Fields<String>, details);
    DTO_FIELD_INFO(details) {
        info->description = "Additional error details";
    }

    /**
     * Timestamp of the response
     * Unix timestamp in seconds
     */
    DTO_FIELD(Int64, timestamp) = std::time(nullptr);
    DTO_FIELD_INFO(timestamp) {
        info->description = "Response timestamp (Unix time)";
    }

public:
    /**
     * Create success response
     */
    static oatpp::Object<StatusDto> createSuccess(const oatpp::String& message = "Success") {
        auto dto = createShared();
        dto->code = 200;
        dto->status = "SUCCESS";
        dto->message = message;
        return dto;
    }

    /**
     * Create error response
     */
    static oatpp::Object<StatusDto> createError(
        Int32 code,
        const oatpp::String& message,
        const oatpp::Fields<String>& details = nullptr
        ) {
        auto dto = createShared();
        dto->code = code;
        dto->status = "ERROR";
        dto->message = message;
        dto->details = details;
        return dto;
    }

    /**
     * Create warning response
     */
    static oatpp::Object<StatusDto> createWarning(
        const oatpp::String& message,
        const oatpp::Fields<String>& details = nullptr
        ) {
        auto dto = createShared();
        dto->code = 200;
        dto->status = "WARNING";
        dto->message = message;
        dto->details = details;
        return dto;
    }

    /**
     * Create not found response
     */
    static oatpp::Object<StatusDto> createNotFound(
        const oatpp::String& message = "Resource not found"
        ) {
        auto dto = createShared();
        dto->code = 404;
        dto->status = "ERROR";
        dto->message = message;
        return dto;
    }

    /**
     * Create unauthorized response
     */
    static oatpp::Object<StatusDto> createUnauthorized(
        const oatpp::String& message = "Unauthorized"
        ) {
        auto dto = createShared();
        dto->code = 401;
        dto->status = "ERROR";
        dto->message = message;
        return dto;
    }

    /**
     * Create forbidden response
     */
    static oatpp::Object<StatusDto> createForbidden(
        const oatpp::String& message = "Access forbidden"
        ) {
        auto dto = createShared();
        dto->code = 403;
        dto->status = "ERROR";
        dto->message = message;
        return dto;
    }

    /**
     * Create bad request response
     */
    static oatpp::Object<StatusDto> createBadRequest(
        const oatpp::String& message,
        const oatpp::Fields<String>& details = nullptr
        ) {
        auto dto = createShared();
        dto->code = 400;
        dto->status = "ERROR";
        dto->message = message;
        dto->details = details;
        return dto;
    }

    /**
     * Create internal server error response
     */
    static oatpp::Object<StatusDto> createInternalError(
        const oatpp::String& message = "Internal server error"
        ) {
        auto dto = createShared();
        dto->code = 500;
        dto->status = "ERROR";
        dto->message = message;
        return dto;
    }
};

#include OATPP_CODEGEN_END(DTO)

#endif /* STATUS_DTO_HPP */
