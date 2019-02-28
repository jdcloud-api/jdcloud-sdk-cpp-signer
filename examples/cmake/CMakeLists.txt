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

cmake_minimum_required(VERSION 3.5)
project(Main)

set(CMAKE_CXX_STANDARD 11)

find_package(PkgConfig)

if(APPLE)
    if (NOT OPENSSL_ROOT_DIR AND EXISTS /usr/local/opt/openssl)
        set(OPENSSL_ROOT_DIR /usr/local/opt/openssl)
    endif()
    FIND_PACKAGE(OpenSSL)
endif(APPLE)

pkg_check_modules(MAIN REQUIRED libjdcloud_signer)

add_executable(main main.cpp)
target_include_directories(main PUBLIC ${MAIN_INCLUDE_DIRS})
target_link_options(main PUBLIC ${MAIN_LDFLAGS})
