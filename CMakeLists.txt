cmake_minimum_required(VERSION 3.10)
project(2048)
add_executable(${PROJECT_NAME} 2048.c game.c game.h input.c input.h  renderer.c renderer.h storage.c storage.h)
find_package(SQLite3 REQUIRED)

find_package(Curses)
if(Curses_FOUND AND NOT TARGET Curses::Curses)
    add_library(Curses::Curses INTERFACE IMPORTED)
    set_target_properties(
            Curses::Curses
            PROPERTIES
            INTERFACE_LINK_LIBRARIES "${CURSES_LIBRARIES}"
            INTERFACE_INCLUDE_DIRECTORIES "${CURSES_INCLUDE_DIRS}"
    )
endif()
target_link_libraries(2048 SQLite::SQLite3 Curses::Curses)