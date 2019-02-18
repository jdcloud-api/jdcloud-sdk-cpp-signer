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

set (Demo_VERSION_MAJOR 0)
set (Demo_VERSION_MINOR 1)
set (Demo_VERSION_DEBUG 0)

set(CMAKE_BUILD_TYPE "Release")

if(WIN32)
	include_directories("C:\\Users\\Administrator\\Downloads\\openssl-OpenSSL_1_0_2n\\build\\include")
	link_libraries("C:\\Users\\Administrator\\Downloads\\openssl-OpenSSL_1_0_2n\\build\\lib\\libeay32.lib")
elseif(APPLE)
	if (EXISTS /usr/local/opt/openssl)
		list(APPEND CMAKE_PREFIX_PATH "/usr/local/opt/openssl")
		set(CMAKE_EXE_LINKER_FLAGS "-L/usr/local/opt/openssl/lib")
		include_directories(/usr/local/opt/openssl/include)
	endif()
	link_libraries(ssl)
	link_libraries(crypto)
elseif(UNIX)
	include_directories(/usr/local/ssl/include)
	link_libraries(ssl)
	link_libraries(crypto)
	link_libraries(uuid)
endif()

aux_source_directory(. DIR_LIB_SRCS)
aux_source_directory(http DIR_LIB_SRCS)
aux_source_directory(util DIR_LIB_SRCS)
aux_source_directory(util/crypto DIR_LIB_SRCS)
aux_source_directory(util/logging DIR_LIB_SRCS)

add_library(jdcloudsigner ${DIR_LIB_SRCS})
