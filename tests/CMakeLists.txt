file(GLOB TESTS *.cpp)
file(GLOB TEST_SOURCES ${CMAKE_SOURCE_DIR}/src/*.cpp)
list(FILTER TEST_SOURCES EXCLUDE REGEX ${CMAKE_SOURCE_DIR}/src/main.cpp)

add_executable(test ${TESTS} ${TEST_SOURCES})
target_link_libraries(test gtest_main ${CURSES_LIBRARIES})