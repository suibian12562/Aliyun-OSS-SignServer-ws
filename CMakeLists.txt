cmake_minimum_required(VERSION 3.20)

project(Aliyun-OSS-SignServer-ws
    LANGUAGES CXX C
    DESCRIPTION "A Aliyun OSS sign server by cpp"
    VERSION 0.0.1
)
set(CMAKE_CXX_STANDARD 17)
set(EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
add_executable(${PROJECT_NAME} src/main.cpp)
add_subdirectory(third_party)

find_package(websocketpp CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(CURL CONFIG REQUIRED)

target_include_directories(${PROJECT_NAME}
    PRIVATE ${CMAKE_SOURCE_DIR}/third_party/osssdk/include
)

target_link_libraries(${PROJECT_NAME} PUBLIC 
osssdk
websocketpp::websocketpp
nlohmann_json::nlohmann_json
OpenSSL::SSL 
OpenSSL::Crypto
CURL::libcurl
) 