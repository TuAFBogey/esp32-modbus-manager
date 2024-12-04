#ifndef AUTH_DB_HPP
#define AUTH_DB_HPP

#include "dto/AuthDto.hpp"
#include "dto/UserDto.hpp"
#include "oatpp-sqlite/orm.hpp"

#include OATPP_CODEGEN_BEGIN(DbClient)

class AuthDb : public oatpp::orm::DbClient {
public:
    AuthDb(const std::shared_ptr<oatpp::orm::Executor>& executor)
        : oatpp::orm::DbClient(executor)
    {
        oatpp::orm::SchemaMigration migration(executor);
        migration.addFile(1, DATABASE_MIGRATIONS "/init.sql");
        migration.migrate();

        auto version = executor->getSchemaVersion();
        OATPP_LOGD("AuthDb", "Migration - OK. Version=%d.", version);
    }

    QUERY(createUser,
          "INSERT INTO User (username, password) VALUES (:username, :password);",
          PARAM(oatpp::String, username),
          PARAM(oatpp::String, password))

    QUERY(getUserByUsername,
          "SELECT * FROM User WHERE username = :username;",
          PARAM(oatpp::String, username))

    QUERY(getUserByCredentials,
          "SELECT * FROM User WHERE username = :username AND password = :password;",
          PARAM(oatpp::String, username),
          PARAM(oatpp::String, password))

    QUERY(getUserById,
          "SELECT * FROM User WHERE id = :id;",
          PARAM(oatpp::Int32, id))

    QUERY(createToken,
          "INSERT INTO AuthToken (user_id, token, expires_at) VALUES (:userId, :token, :expiresAt);",
          PARAM(oatpp::Int32, userId),
          PARAM(oatpp::String, token),
          PARAM(oatpp::Int64, expiresAt))

    QUERY(getValidToken,
          "SELECT * FROM AuthToken WHERE token = :token AND expires_at > :currentTime;",
          PARAM(oatpp::String, token),
          PARAM(oatpp::Int64, currentTime))

    QUERY(deleteExpiredTokens,
          "DELETE FROM AuthToken WHERE expires_at <= :currentTime;",
          PARAM(oatpp::Int64, currentTime))

    QUERY(getAllUsers,
          "SELECT * FROM User")

    QUERY(getAllTokens,
          "SELECT * FROM AuthToken")
};

#include OATPP_CODEGEN_END(DbClient)

#endif /* AUTH_DB_HPP */
