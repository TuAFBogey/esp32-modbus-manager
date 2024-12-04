#ifndef HASH_UTILS_HPP
#define HASH_UTILS_HPP

#include "oatpp/core/Types.hpp"
#include "oatpp/core/base/StrBuffer.hpp"
#include <string>
#include <sstream>
#include <iomanip>
#include <functional>
#include <vector>
#include <map>
#include <random>

class HashUtils {
public:
    static oatpp::String hashPassword(const oatpp::String& password) {
        if (!password) {
            OATPP_LOGD("HashUtils", "Password is null");
            return nullptr;
        }

        // Get password as string
        std::string pwd = password->std_str();
        OATPP_LOGD("HashUtils", "Input password: %s", pwd.c_str());

        // Calculate hash
        std::hash<std::string> hasher;
        auto hashValue = hasher(pwd);

        // Convert to hex string
        std::stringstream ss;
        ss << std::hex << std::setfill('0') << std::setw(16) << hashValue;
        auto hashStr = ss.str();

        OATPP_LOGD("HashUtils", "Generated hash: %s", hashStr.c_str());

        return oatpp::String(hashStr.c_str());
    }

    static oatpp::String generateToken(size_t length = 32) {
        const std::string chars =
            "0123456789"
            "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
            "abcdefghijklmnopqrstuvwxyz";

        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(0, chars.length() - 1);

        std::string token;
        token.reserve(length);
        for (size_t i = 0; i < length; ++i) {
            token += chars[dis(gen)];
        }

        return oatpp::String(token.c_str());
    }
};

#endif /* HASH_UTILS_HPP */
