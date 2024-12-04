cmake_minimum_required(VERSION 3.1)
project(esp32-modbus-api)

set(CMAKE_CXX_STANDARD 11)

set(OATPP_DIR "oatpp_install_path")
list(APPEND CMAKE_PREFIX_PATH ${OATPP_DIR})

# Add project main library
add_library(${PROJECT_NAME}-lib
        src/controller/AuthController.hpp
        src/controller/ModbusController.hpp
        src/controller/SwaggerController.hpp
        src/dto/AuthDto.hpp
        src/dto/UserDto.hpp
        src/dto/ModbusDto.hpp
        src/dto/DeviceDto.hpp
        src/dto/StatusDto.hpp
        src/db/AuthDb.hpp
        src/db/ModbusDb.hpp
        src/service/AuthService.hpp
        src/service/ModbusService.hpp
        src/component/AppComponent.hpp
        src/component/DatabaseComponent.hpp
        src/component/SwaggerComponent.hpp
        src/util/ErrorHandler.hpp
        src/util/ErrorHandler.cpp
        src/util/HashUtils.hpp)


# Include directories
target_include_directories(${PROJECT_NAME}-lib PUBLIC src
    PUBLIC "${OATPP_DIR}/include"
    PUBLIC "${OATPP_DIR}/include/oatpp-1.2.5/oatpp"
    PUBLIC "${OATPP_DIR}/include/oatpp-swagger-1.2.5/oatpp-swagger"
    PUBLIC "${OATPP_DIR}/include/oatpp-sqlite-1.2.5/oatpp-sqlite"
)

# Find required packages
find_package(oatpp          1.2.5 REQUIRED)
find_package(oatpp-swagger  1.2.5 REQUIRED)
find_package(oatpp-sqlite   1.2.5 REQUIRED)


# Link dependencies
target_link_libraries(${PROJECT_NAME}-lib
        PUBLIC oatpp::oatpp
        PUBLIC oatpp::oatpp-swagger
        PUBLIC oatpp::oatpp-sqlite
)

# Add definitions
add_definitions(
    -DOATPP_SWAGGER_RES_PATH="${oatpp-swagger_INCLUDE_DIRS}/../bin/oatpp-swagger/res"
    -DDATABASE_FILE="${CMAKE_CURRENT_SOURCE_DIR}/db.sqlite"
    -DDATABASE_MIGRATIONS="${CMAKE_CURRENT_SOURCE_DIR}/sql"
)

# Create executable
add_executable(${PROJECT_NAME}-exe
        src/App.cpp)

target_link_libraries(${PROJECT_NAME}-exe ${PROJECT_NAME}-lib)
