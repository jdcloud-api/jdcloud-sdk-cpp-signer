add_library(gtest STATIC "googletest/src/gtest-all.cc")
target_link_libraries(gtest PUBLIC pthread)
target_include_directories(gtest PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/googletest/include)
target_include_directories(gtest PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/googletest)
