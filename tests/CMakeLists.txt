set(TEST_SOURCES
        test_chunk.cpp
        test_scanner.cpp
        test_value.cpp
        test_vm.cpp)
set(TEST_INCLUDES "./")

if (COMPILE_TESTS)
    include(CTest)
    foreach (test_source IN LISTS TEST_SOURCES)
        string(REGEX REPLACE "\\.cpp$" "" test_source_name ${test_source})
        add_executable(${test_source_name} ${test_source})
        target_include_directories(${test_source_name} PUBLIC ${TEST_INCLUDES})
        target_link_libraries(${test_source_name} PUBLIC ${LIBRARY_NAME} gtest_main gmock_main)
        add_test(NAME ${test_source_name} COMMAND ${test_source_name})
    endforeach ()
endif ()
