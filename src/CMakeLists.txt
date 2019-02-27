# Copyright 2018 JDCLOUD.COM
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
set (Demo_VERSION_MINOR 1)
set (Demo_VERSION_DEBUG 0)
set(VERSION ${Demo_VERSION_MAJOR}.${Demo_VERSION_MINOR}.${Demo_VERSION_DEBUG})

CONFIGURE_FILE(
    ${CMAKE_CURRENT_SOURCE_DIR}/libjdcloudsigner.pc.in
    ${CMAKE_CURRENT_BINARY_DIR}/libjdcloudsigner.pc
@ONLY)

INSTALL(
    FILES ${CMAKE_CURRENT_BINARY_DIR}/libjdcloudsigner.pc
    DESTINATION lib${LIB_SUFFIX}/pkgconfig
)

set(CMAKE_BUILD_TYPE "Release")

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
    include_directories(/usr/local/ssl/include)
    link_libraries(crypto)
    link_libraries(uuid)
endif()

aux_source_directory(. DIR_LIB_SRCS)
aux_source_directory(http DIR_LIB_SRCS)
aux_source_directory(util DIR_LIB_SRCS)
aux_source_directory(util/crypto DIR_LIB_SRCS)
aux_source_directory(util/logging DIR_LIB_SRCS)

add_library(jdcloudsigner OBJECT ${DIR_LIB_SRCS})
set_property(TARGET jdcloudsigner PROPERTY POSITION_INDEPENDENT_CODE 1)
add_library(jdcloudsigner_shared SHARED $<TARGET_OBJECTS:jdcloudsigner>)
set_target_properties(jdcloudsigner_shared PROPERTIES OUTPUT_NAME jdcloudsigner)
add_library(jdcloudsigner_static STATIC $<TARGET_OBJECTS:jdcloudsigner>)
set_target_properties(jdcloudsigner_static PROPERTIES OUTPUT_NAME jdcloudsigner)

install(TARGETS jdcloudsigner_shared
    LIBRARY
        DESTINATION lib)
install(TARGETS jdcloudsigner_static
    ARCHIVE
        DESTINATION lib)
