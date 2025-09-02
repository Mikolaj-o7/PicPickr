cmake_minimum_required(VERSION 3.10)
project(PicPickr C)

set(CMAKE_C_STANDARD 99)

find_package(raylib REQUIRED)

add_executable(${PROJECT_NAME} src/main.c)

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/include)

target_link_libraries(${PROJECT_NAME} raylib)

if(UNIX AND NOT APPLE)
  target_link_libraries(${PROJECT_NAME} m pthread dl rt X11)
endif()
