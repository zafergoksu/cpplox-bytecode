set(LIBRARY_HEADERS
        "common.h")
set(LIBRARY_SOURCES
        "common.cpp")

set(LIBRARY_INCLUDES "./")

add_library(${LIBRARY_NAME} STATIC ${LIBRARY_SOURCES} ${LIBRARY_HEADERS})
target_include_directories(${LIBRARY_NAME} PUBLIC ${LIBRARY_INCLUDES})

add_executable(${BINARY_NAME} main.cpp)

target_link_libraries(${BINARY_NAME} PUBLIC ${LIBRARY_NAME})