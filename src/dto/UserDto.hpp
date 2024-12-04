#ifndef USER_DTO_HPP
#define USER_DTO_HPP

#include "oatpp/core/macro/codegen.hpp"
#include "oatpp/core/Types.hpp"

#include OATPP_CODEGEN_BEGIN(DTO)

/**
 * User Data Transfer Object
 */
class UserDto : public oatpp::DTO {

    DTO_INIT(UserDto, DTO)

    DTO_FIELD(Int32, id);
    DTO_FIELD_INFO(id) {
        info->description = "User identifier";
    }

    DTO_FIELD(String, username);
    DTO_FIELD_INFO(username) {
        info->description = "Username";
    }

    DTO_FIELD(String, password);
    DTO_FIELD_INFO(password) {
        info->description = "Password (hashed)";
    }

    DTO_FIELD(Int64, created_at) = std::time(nullptr);
    DTO_FIELD_INFO(created_at) {
        info->description = "Account creation timestamp";
    }
};

#include OATPP_CODEGEN_END(DTO)

#endif /* USER_DTO_HPP */
