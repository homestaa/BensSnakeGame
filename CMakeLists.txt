cmake_minimum_required(VERSION 3.5.1)

project(Bens-Snake-Game)

add_definitions("-Wall" "-g")

include_directories("src")
file(GLOB SOURCES src/*.cpp)
file(COPY "res" DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)
find_package(SDL2_ttf REQUIRED)
find_package(SDL2_mixer REQUIRED)
include_directories(${PROJECT_NAME}
                    ${SDL2_INCLUDE_DIRS}
                    ${SDL2_IMAGE_INCLUDE_DIRS}
                    ${SDL2_TTF_INCLUDE_DIRS}
                    ${SDL2_MIXER_INCLUDE_DIRS})

add_executable(${PROJECT_NAME}  ${SOURCES})
target_link_libraries(${PROJECT_NAME}
                      SDL2::SDL2
                      SDL2_image::SDL2_image
                      SDL2_ttf::SDL2_ttf
                      SDL2_mixer::SDL2_mixer)