#ifndef AUTH_SERVICE_HPP
#define AUTH_SERVICE_HPP

#include "db/AuthDb.hpp"
#include "dto/AuthDto.hpp"
#include "dto/StatusDto.hpp"
#include "dto/UserDto.hpp"
#include "util/HashUtils.hpp"

#include <chrono>

class AuthService {
private:
    std::shared_ptr<AuthDb> m_database;
    const int TOKEN_VALIDITY_HOURS = 24;

    void cleanupExpiredTokens() {
        auto currentTime = std::chrono::system_clock::to_time_t(
            std::chrono::system_clock::now()
            );
        m_database->deleteExpiredTokens(currentTime);
    }

public:
    explicit AuthService(const std::shared_ptr<AuthDb>& database)
        : m_database(database)
    {
        cleanupExpiredTokens();
    }

    oatpp::Object<StatusDto> signUp(const oatpp::Object<SignUpDto>& dto) {
        if (!dto) {
            return StatusDto::createError(400, "Invalid signup data");
        }

        if (!dto->username || dto->username->getSize() == 0) {
            return StatusDto::createError(400, "Username is required");
        }

        if (!dto->password || dto->password->getSize() == 0) {
            return StatusDto::createError(400, "Password is required");
        }

        auto existingUser = m_database->getUserByUsername(dto->username);
        if (existingUser->hasMoreToFetch()) {
            return StatusDto::createError(400, "Username already exists");
        }

        auto hashedPassword = HashUtils::hashPassword(dto->password);
        auto dbResult = m_database->createUser(dto->username, hashedPassword);

        if (dbResult->isSuccess()) {
            OATPP_LOGD("AuthService", "User created successfully: %s", dto->username->c_str());
            return StatusDto::createSuccess("User created successfully");
        }

        return StatusDto::createError(500, "Failed to create user");
    }

    oatpp::Object<TokenDto> login(const oatpp::Object<LoginDto>& dto) {
        if (!dto || !dto->username || !dto->password) {
            return nullptr;
        }

        auto hashedPassword = HashUtils::hashPassword(dto->password);
        auto dbResult = m_database->getUserByCredentials(dto->username, hashedPassword);
        auto users = dbResult->fetch<oatpp::Vector<oatpp::Object<UserDto>>>();

        if (users->empty()) {
            return nullptr;
        }

        auto user = users[0];
        auto token = HashUtils::generateToken();
        auto expiresAt = std::chrono::system_clock::now() +
                         std::chrono::hours(TOKEN_VALIDITY_HOURS);
        auto expiresAtTimestamp = std::chrono::system_clock::to_time_t(expiresAt);

        cleanupExpiredTokens();

        auto tokenResult = m_database->createToken(
            user->id,
            token,
            expiresAtTimestamp
            );

        if (!tokenResult->isSuccess()) {
            return nullptr;
        }

        auto tokenDto = TokenDto::createShared();
        tokenDto->user_id = user->id;
        tokenDto->token = token;
        tokenDto->expires_at = expiresAtTimestamp;
        tokenDto->created_at = std::time(nullptr);
        return tokenDto;
    }

    oatpp::Object<UserDto> validateToken(const oatpp::String& authHeader) {
        if (!authHeader || authHeader->getSize() == 0) {
            return nullptr;
        }

        oatpp::String tokenStr = authHeader;
        const char* bearerPrefix = "Bearer ";
        const v_buff_size prefixLength = 7;

        if (authHeader->getSize() > prefixLength) {
            v_char8* data = (v_char8*)authHeader->getData();
            if (std::strncmp((const char*)data, bearerPrefix, prefixLength) == 0) {
                tokenStr = oatpp::String((const char*)data + prefixLength, authHeader->getSize() - prefixLength);
            }
        }

        auto currentTime = std::chrono::system_clock::to_time_t(
            std::chrono::system_clock::now()
            );

        auto dbResult = m_database->getValidToken(tokenStr, currentTime);
        auto tokens = dbResult->fetch<oatpp::Vector<oatpp::Object<TokenDto>>>();

        if (tokens->empty()) {
            return nullptr;
        }

        auto token = tokens[0];
        auto userResult = m_database->getUserById(token->user_id);
        auto users = userResult->fetch<oatpp::Vector<oatpp::Object<UserDto>>>();

        return users->empty() ? nullptr : users[0];
    }
};

#endif /* AUTH_SERVICE_HPP */
