#ifndef AUTH_CONTROLLER_HPP
#define AUTH_CONTROLLER_HPP

#include "service/AuthService.hpp"
#include "dto/AuthDto.hpp"
#include "dto/StatusDto.hpp"

#include "oatpp/web/server/api/ApiController.hpp"
#include "oatpp/core/macro/codegen.hpp"
#include "oatpp/core/macro/component.hpp"

#include OATPP_CODEGEN_BEGIN(ApiController)

class AuthController : public oatpp::web::server::api::ApiController {
private:
    std::shared_ptr<AuthService> m_authService;

public:
    AuthController(const std::shared_ptr<ObjectMapper>& objectMapper,
                   const std::shared_ptr<AuthService>& authService)
        : oatpp::web::server::api::ApiController(objectMapper)
        , m_authService(authService)
    {}

    static std::shared_ptr<AuthController> createShared(
        OATPP_COMPONENT(std::shared_ptr<ObjectMapper>, objectMapper),
        OATPP_COMPONENT(std::shared_ptr<AuthService>, authService))
    {
        return std::make_shared<AuthController>(objectMapper, authService);
    }

    ENDPOINT_INFO(signUp) {
        info->summary = "Create new user account";
        info->addConsumes<Object<SignUpDto>>("application/json");
        info->addResponse<Object<StatusDto>>(Status::CODE_200, "application/json");
    }
    ENDPOINT("POST", "api/auth/signup", signUp,
             BODY_DTO(Object<SignUpDto>, signUpDto)) {
        return createDtoResponse(Status::CODE_200, m_authService->signUp(signUpDto));
    }

    ENDPOINT_INFO(login) {
        info->summary = "Authenticate user and receive auth token";
        info->addConsumes<Object<LoginDto>>("application/json");
        info->addResponse<Object<TokenDto>>(Status::CODE_200, "application/json");
        info->addResponse<String>(Status::CODE_401, "text/plain");
    }
    ENDPOINT("POST", "api/auth/login", login,
             BODY_DTO(Object<LoginDto>, loginDto)) {
        auto token = m_authService->login(loginDto);
        if (!token) {
            return createResponse(Status::CODE_401, "Invalid credentials");
        }
        return createDtoResponse(Status::CODE_200, token);
    }

    ENDPOINT_INFO(validateToken) {
        info->summary = "Validate authentication token";
        info->addSecurityRequirement("Authorization");
        info->authorization = "Authorization";
        info->addResponse<Object<UserDto>>(Status::CODE_200, "application/json");
        info->addResponse<String>(Status::CODE_401, "text/plain");
    }
    ENDPOINT("GET", "api/auth/validate", validateToken,
             HEADER(String, authorization, "Authorization")) {
        auto user = m_authService->validateToken(authorization);
        if (!user) {
            return createResponse(Status::CODE_401, "Invalid or expired token");
        }
        return createDtoResponse(Status::CODE_200, user);
    }
};

#include OATPP_CODEGEN_END(ApiController)

#endif /* AUTH_CONTROLLER_HPP */
