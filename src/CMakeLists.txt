# Copyright 2018,2019 JDCLOUD.COM
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http:#www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

include(GNUInstallDirs)

set (Demo_VERSION_MAJOR 0)
set (Demo_VERSION_MINOR 2)
set (Demo_VERSION_DEBUG 0)
set(VERSION ${Demo_VERSION_MAJOR}.${Demo_VERSION_MINOR}.${Demo_VERSION_DEBUG})
set(SOVERSION ${VERSION})

CONFIGURE_FILE(
    ${CMAKE_CURRENT_SOURCE_DIR}/libjdcloud_signer.pc.in
    ${CMAKE_CURRENT_BINARY_DIR}/libjdcloud_signer.pc
@ONLY)

INSTALL(
    FILES ${CMAKE_CURRENT_BINARY_DIR}/libjdcloud_signer.pc
    DESTINATION lib${LIB_SUFFIX}/pkgconfig
)

if(WIN32)
    include_directories("C:\\Users\\Administrator\\Downloads\\openssl-OpenSSL_1_0_2n\\build\\include")
    link_libraries("C:\\Users\\Administrator\\Downloads\\openssl-OpenSSL_1_0_2n\\build\\lib\\libeay32.lib")
elseif(APPLE)
    if (NOT OPENSSL_ROOT_DIR AND EXISTS /usr/local/opt/openssl)
        set(OPENSSL_ROOT_DIR /usr/local/opt/openssl)
    endif()
    FIND_PACKAGE(OpenSSL)
    link_libraries(OpenSSL::Crypto)
elseif(UNIX)
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(depends REQUIRED libcrypto uuid)
    link_libraries(${depends_LIBRARIES})
    include_directories(${depends_INCLUDE_DIRS})
endif()

aux_source_directory(. DIR_LIB_SRCS)
aux_source_directory(http DIR_LIB_SRCS)
aux_source_directory(util DIR_LIB_SRCS)
aux_source_directory(util/crypto DIR_LIB_SRCS)
aux_source_directory(logging DIR_LIB_SRCS)

add_library(jdcloudsigner OBJECT ${DIR_LIB_SRCS})
target_include_directories(jdcloudsigner PUBLIC ${CMAKE_SOURCE_DIR}/include)
target_include_directories(jdcloudsigner PRIVATE "${CMAKE_SOURCE_DIR}/internal")
target_include_directories(jdcloudsigner PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
set_property(TARGET jdcloudsigner PROPERTY POSITION_INDEPENDENT_CODE 1)
set_property(TARGET jdcloudsigner PROPERTY CXX_STANDARD 11)

add_library(jdcloudsigner_shared SHARED $<TARGET_OBJECTS:jdcloudsigner>)
set_target_properties(jdcloudsigner_shared PROPERTIES OUTPUT_NAME jdcloud_signer)
set_target_properties(jdcloudsigner_shared PROPERTIES SOVERSION ${SOVERSION})

add_library(jdcloudsigner_static STATIC $<TARGET_OBJECTS:jdcloudsigner>)
set_target_properties(jdcloudsigner_static PROPERTIES OUTPUT_NAME jdcloud_signer)

install(TARGETS jdcloudsigner_shared
    LIBRARY
        DESTINATION lib)
install(TARGETS jdcloudsigner_static
    ARCHIVE
        DESTINATION lib)

add_executable(jdcloud_signer_test
    tests/TestMain.cpp
    tests/Sha256Test.cpp
    tests/JdcloudSignerImplTest.cpp
    tests/URITest.cpp
)
target_link_libraries(jdcloud_signer_test PUBLIC gtest jdcloudsigner_shared)
target_include_directories(jdcloud_signer_test PRIVATE "${CMAKE_SOURCE_DIR}/include" "${CMAKE_SOURCE_DIR}/internal")
add_test(NAME jdcloud_signer_test COMMAND jdcloud_signer_test)

