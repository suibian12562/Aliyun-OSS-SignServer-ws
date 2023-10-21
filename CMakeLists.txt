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
add_executable(${PROJECT_NAME} src/main.cpp)
add_subdirectory(third_party)

set(CMAKE_PREFIX_PATH ".vcpkg_installed/x64-windows/share")
find_package(websocketpp CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)

target_link_libraries(${PROJECT_NAME} PRIVATE 
websocketpp::websocketpp
nlohmann_json::nlohmann_json
) 
target_include_directories(${PROJECT_NAME}
	PRIVATE ${CMAKE_SOURCE_DIR}/third_party/aliyun-oss-cpp-sdk/sdk/include
	PRIVATE ${CMAKE_SOURCE_DIR}/third_party/aliyun-oss-cpp-sdk/sdk/
	cpp-sdk${STATIC_LIB_SUFFIX}
	)



if (${TARGET_OS} STREQUAL "WINDOWS")

target_include_directories(${PROJECT_NAME}
   PRIVATE ${CMAKE_SOURCE_DIR}/third_party/aliyun-oss-cpp-sdk/third_party/include)

   target_link_libraries(${PROJECT_NAME} PRIVATE 
    websocketpp::websocketpp
    nlohmann_json::nlohmann_json
    cpp-sdk
	#curl 
	#crypto 
	#pthread
	${CRYPTO_LIBS}
	${CLIENT_LIBS}
) 
endif()	



if (${TARGET_OS} STREQUAL "LINUX")
target_link_libraries(${PROJECT_NAME} PRIVATE 
    websocketpp::websocketpp
    nlohmann_json::nlohmann_json
    cpp-sdk
	curl 
	crypto 
	pthread
	${CRYPTO_LIBS}
	${CLIENT_LIBS}
) 
endif()

if (${TARGET_OS} STREQUAL "WINDOWS")
	set(WLIB_TARGET "Win32")
	if (CMAKE_CL_64)
	set(WLIB_TARGET "x64")
	endif()
	set(CRYPTO_LIBS 
		${CMAKE_SOURCE_DIR}/third_party/aliyun-oss-cpp-sdk/third_party/lib/${WLIB_TARGET}/ssleay32.lib 
		${CMAKE_SOURCE_DIR}/third_party/aliyun-oss-cpp-sdk/third_party/lib/${WLIB_TARGET}/libeay32.lib)
	set(CRYPTO_INCLUDE_DIRS 
		${CMAKE_SOURCE_DIR}/third_party/aliyun-oss-cpp-sdk/third_party/include) 

	set(CLIENT_LIBS 
		${CMAKE_SOURCE_DIR}/third_party/aliyun-oss-cpp-sdk/third_party/lib/${WLIB_TARGET}/libcurl.lib) 
	set(CLIENT_INCLUDE_DIRS 
	${CMAKE_SOURCE_DIR}/third_party/aliyun-oss-cpp-sdk/third_party/include) 
else()
	include(FindCURL)
	include(FindOpenSSL)

	if(NOT CURL_FOUND)
		message(FATAL_ERROR "Could not find curl")
	endif()

	if(NOT OPENSSL_FOUND)
		message(FATAL_ERROR "Could not find openssl")
	endif()

	set(CRYPTO_LIBS ${OPENSSL_LIBRARIES})
	set(CRYPTO_INCLUDE_DIRS ${OPENSSL_INCLUDE_DIR}) 
	set(CRYPTO_LIBS_ABSTRACT_NAME crypto ssl)

	set(CLIENT_LIBS ${CURL_LIBRARIES})
	set(CLIENT_INCLUDE_DIRS ${CURL_INCLUDE_DIRS}) 
	set(CLIENT_LIBS_ABSTRACT_NAME curl)
endif()
# target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_17)
# set(EXPORT_COMPILE_COMMANDS ON)

# add_subdirectory(third_party)
# include_directories(
#     third_party/aliyun-oss-cpp-sdk/sdk/include
#     )


# # 链接依赖库
# set(CMAKE_PREFIX_PATH "./vcpkg_installed/x64-windows/share")

# find_package(nlohmann_json CONFIG REQUIRED)
# find_package(websocketpp CONFIG REQUIRED)


# file(GLOB RELEASE_LIB_FILES "${CMAKE_CURRENT_BINARY_DIR}/Release/lib/*")
# file(GLOB DEBUG_LIB_FILES "${CMAKE_CURRENT_BINARY_DIR}/Debug/lib/*")
# set(ALL_LIB_FILES ${RELEASE_LIB_FILES} ${DEBUG_LIB_FILES})
# if(ALL_LIB_FILES)
#     message("Found files: ${ALL_LIB_FILES}")
#     foreach(LIB_FILE ${ALL_LIB_FILES})
#         add_custom_command(
#             TARGET ${PROJECT_NAME} PRE_BUILD
#             COMMAND ${CMAKE_COMMAND} -E copy
#                     "${LIB_FILE}"
#                     "${CMAKE_CURRENT_BINARY_DIR}"
#         )
#     endforeach()
# else()
#     message("No files found in ${CMAKE_CURRENT_BINARY_DIR}/release/lib or ${CMAKE_CURRENT_BINARY_DIR}/Debug/lib")
# endif()



# target_link_libraries(${PROJECT_NAME} PRIVATE 
#     websocketpp::websocketpp
#     nlohmann_json::nlohmann_json
#     alibabacloud-oss-cpp-sdk
#     cpp-sdk
# ) 


# # include_directories(main
# #     third_party/aliyun-oss-cpp-sdk/sdk/include
# # )



