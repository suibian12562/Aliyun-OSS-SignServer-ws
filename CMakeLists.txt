cmake_minimum_required(VERSION 3.20)

project(Aliyun-OSS-SignServer-ws
    LANGUAGES CXX C
    DESCRIPTION "A Aliyun OSS sign server by cpp"
    VERSION 0.0.1
)

if ()
	if (${CMAKE_SYSTEM_NAME} STREQUAL "Android")
		set(PLATFORM_ANDROID 1)
		set(TARGET_OS "ANDROID")
	else()
		message(FATAL_ERROR "Do not support target platform")
	endif()
else()
	if(CMAKE_HOST_APPLE)
		set(PLATFORM_APPLE 1)
		set(TARGET_OS "APPLE")
	elseif(CMAKE_HOST_UNIX)
		set(PLATFORM_LINUX 1)
		set(TARGET_OS "LINUX")
	elseif(CMAKE_HOST_WIN32)
		set(PLATFORM_WINDOWS 1)
		set(TARGET_OS "WINDOWS")
	else()
		message(FATAL_ERROR "Do not support unknown host OS")
	endif()
endif()

set(EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
add_executable(${PROJECT_NAME} src/main.cpp src/main.h)
add_subdirectory(third_party)


if (${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
 set(CMAKE_TOOLCHAIN_FILE "G:/vs/VC/vcpkg/scripts/buildsystems/vcpkg.cmake")
 set(CMAKE_PREFIX_PATH "G:/vs/VC/vcpkg/installed/x64-windows/share")
endif()

find_package(websocketpp CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(CURL CONFIG REQUIRED)

target_include_directories(${PROJECT_NAME}
	PRIVATE ${CMAKE_SOURCE_DIR}/third_party/aliyun-oss-cpp-sdk/sdk/include
	PRIVATE ${CMAKE_SOURCE_DIR}/third_party/aliyun-oss-cpp-sdk/sdk/
	cpp-sdk${STATIC_LIB_SUFFIX}
)

if (${TARGET_OS} STREQUAL "WINDOWS")
target_include_directories(${PROJECT_NAME}
PRIVATE ${CMAKE_SOURCE_DIR}/third_party/aliyun-oss-cpp-sdk/third_party/include)
endif()	

target_link_libraries(${PROJECT_NAME} PUBLIC 
websocketpp::websocketpp
nlohmann_json::nlohmann_json
cpp-sdk
OpenSSL::SSL 
OpenSSL::Crypto
CURL::libcurl
) 