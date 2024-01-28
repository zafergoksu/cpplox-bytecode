set(TEST_LIBRARY_SOURCE)
set(TEST_INCLUDES "./" "./src")
#set(TEST_LIBRARY_HEADER)

add_executable(test_chunk test_chunk.cpp)
target_include_directories(test_chunk PUBLIC ${TEST_INCLUDES})
target_link_libraries(test_chunk PUBLIC ${LIBRARY_NAME} gtest_main gmock_main)

add_executable(test_value test_value.cpp)
target_include_directories(test_value PUBLIC ${TEST_INCLUDES})
target_link_libraries(test_value PUBLIC ${LIBRARY_NAME} gtest_main gmock_main)

add_executable(test_vm test_vm.cpp)
target_include_directories(test_vm PUBLIC ${TEST_INCLUDES})
target_link_libraries(test_vm PUBLIC ${LIBRARY_NAME} gtest_main gmock_main)
