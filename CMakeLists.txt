cmake_minimum_required(VERSION 3.20)

project(Aliyun-OSS-SignServer-ws
    LANGUAGES CXX C
    DESCRIPTION "A Aliyun OSS sign server by cpp"
    VERSION 0.0.1
)

add_executable(${PROJECT_NAME} src/main.cpp)
target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_17)
set(EXPORT_COMPILE_COMMANDS ON)

add_subdirectory(third_party)
include_directories(
    third_party/aliyun-oss-cpp-sdk/sdk/include
    )


# 链接依赖库
set(CMAKE_PREFIX_PATH "./vcpkg_installed/x64-windows/share")

find_package(nlohmann_json CONFIG REQUIRED)
find_package(websocketpp CONFIG REQUIRED)


file(GLOB RELEASE_LIB_FILES "${CMAKE_CURRENT_BINARY_DIR}/Release/lib/*")
file(GLOB DEBUG_LIB_FILES "${CMAKE_CURRENT_BINARY_DIR}/Debug/lib/*")
set(ALL_LIB_FILES ${RELEASE_LIB_FILES} ${DEBUG_LIB_FILES})
if(ALL_LIB_FILES)
    message("Found files: ${ALL_LIB_FILES}")
    foreach(LIB_FILE ${ALL_LIB_FILES})
        add_custom_command(
            TARGET ${PROJECT_NAME} PRE_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy
                    "${LIB_FILE}"
                    "${CMAKE_CURRENT_BINARY_DIR}"
        )
    endforeach()
else()
    message("No files found in ${CMAKE_CURRENT_BINARY_DIR}/release/lib or ${CMAKE_CURRENT_BINARY_DIR}/Debug/lib")
endif()



target_link_libraries(${PROJECT_NAME} PRIVATE 
    websocketpp::websocketpp
    nlohmann_json::nlohmann_json
    alibabacloud-oss-cpp-sdk
    cpp-sdk
) 


# include_directories(main
#     third_party/aliyun-oss-cpp-sdk/sdk/include
# )



