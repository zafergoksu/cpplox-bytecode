set(LIBRARY_HEADERS
        chunk.h
        common.h
        compiler.h
        debug.h
        lox.h
        scanner.h
        token.h
        utility.h
        value.h
        vm.h)

set(LIBRARY_SOURCES
        chunk.cpp
        compiler.cpp
        debug.cpp
        lox.cpp
        scanner.cpp
        token.cpp
        value.cpp
        vm.cpp)

set(LIBRARY_INCLUDES "./")

add_library(${LIBRARY_NAME} STATIC ${LIBRARY_SOURCES} ${LIBRARY_HEADERS})
target_include_directories(${LIBRARY_NAME} PUBLIC ${LIBRARY_INCLUDES})

add_executable(${BINARY_NAME} main.cpp)

target_link_libraries(${BINARY_NAME} PUBLIC ${LIBRARY_NAME})
