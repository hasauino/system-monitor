add_executable(monitor ${SOURCES})

target_link_libraries(monitor ${CURSES_LIBRARIES})
#TODO : Run - Werror in CI.
target_compile_options(monitor PRIVATE -Wall -Wextra)