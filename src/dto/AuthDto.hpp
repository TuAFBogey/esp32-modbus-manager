#ifndef AUTH_DTO_HPP
#define AUTH_DTO_HPP

#include "oatpp/core/macro/codegen.hpp"
#include "oatpp/core/Types.hpp"

#include OATPP_CODEGEN_BEGIN(DTO)

/**
 * SignUp Data Transfer Object
 */
class SignUpDto : public oatpp::DTO {

    DTO_INIT(SignUpDto, DTO)

    DTO_FIELD(String, username);
    DTO_FIELD_INFO(username) {
        info->description = "Username for registration";
    }

    DTO_FIELD(String, password);
    DTO_FIELD_INFO(password) {
        info->description = "Password for registration";
    }
};

/**
 * Login Data Transfer Object
 */
class LoginDto : public oatpp::DTO {

    DTO_INIT(LoginDto, DTO)

    DTO_FIELD(String, username);
    DTO_FIELD_INFO(username) {
        info->description = "Username for login";
    }

    DTO_FIELD(String, password);
    DTO_FIELD_INFO(password) {
        info->description = "Password for login";
    }
};

/**
 * Token Data Transfer Object
 */
class TokenDto : public oatpp::DTO {

    DTO_INIT(TokenDto, DTO)

    DTO_FIELD(Int32, id);
    DTO_FIELD_INFO(id) {
        info->description = "Token ID";
    }

    DTO_FIELD(Int32, user_id, "user_id");
    DTO_FIELD_INFO(user_id) {
        info->description = "User ID associated with token";
    }

    DTO_FIELD(String, token);
    DTO_FIELD_INFO(token) {
        info->description = "Authentication token - Use with 'Bearer ' prefix in requests";
    }

    DTO_FIELD(Int64, expires_at, "expires_at");
    DTO_FIELD_INFO(expires_at) {
        info->description = "Token expiration timestamp";
    }

    DTO_FIELD(Int64, created_at, "created_at");
    DTO_FIELD_INFO(created_at) {
        info->description = "Token creation timestamp";
    }
};

#include OATPP_CODEGEN_END(DTO)

#endif /* AUTH_DTO_HPP */
