project(project_src)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O0 -lSDL2 -lSDL2_ttf -lSDL2_image")


# set all rc files in ./src
file(GLOB PROJECT_SRC CONFIGURE_DEPENDS "*.cpp" "*.h")

# main application
add_executable(vedit)

# add all src files
target_include_directories(vedit PUBLIC ${PORJECT_SRC})
target_sources(vedit PRIVATE ${PROJECT_SRC})

# set whispercpp dir
set(WHISPER_DIR ${CMAKE_SOURCE_DIR}/lib/whispercpp)
message(WHISPER_DIR="${WHISPER_DIR}")

# add whispercpp lib
target_link_libraries(vedit PRIVATE whisper)
target_link_directories(vedit PUBLIC ${WHISPER_DIR})

# set imgui dir
set(IMGUI_DIR ${CMAKE_SOURCE_DIR}/extern/imgui)
message(IMGUI_DIR="${IMGUI_DIR}")

# add imgui dir
target_include_directories(vedit PUBLIC ${IMGUI_DIR})
target_sources(vedit PRIVATE ${IMGUI_DIR})

target_link_libraries(vedit PRIVATE imgui)
target_link_directories(vedit PUBLIC ${IMGUI_DIR})

# set implot dir
set(IMPLOT_DIR ${CMAKE_SOURCE_DIR}/extern/implot)
message(IMPLOT_DIR="${IMPLOT_DIR}")

# add implot dir
target_include_directories(vedit PUBLIC ${IMPLOT_DIR})
target_sources(vedit PRIVATE ${IMPLOT_DIR})

target_link_libraries(vedit PRIVATE implot)
target_link_directories(vedit PUBLIC ${IMPLOT_DIR})