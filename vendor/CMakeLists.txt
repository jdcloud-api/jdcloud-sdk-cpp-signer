add_library(gtest STATIC "googletest/src/gtest-all.cc")
target_include_directories(gtest PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/googletest/include)
target_include_directories(gtest PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/googletest)
