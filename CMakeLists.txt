cmake_minimum_required(VERSION 3.16)
project(HotAndCold VERSION 1.0.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Enable compiler warnings
if(MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# Find SFML 3.0
find_package(SFML 3.0 COMPONENTS graphics window system REQUIRED)

# Source files
set(SOURCES
    main.cpp
    Game.cpp
    Board.cpp
    Character.cpp
    Controller.cpp
    Doors.cpp
    Gates.cpp
    LevelSelect.cpp
)

# Header files
set(HEADERS
    include/Game.h
    include/Board.h
    include/Character.h
    include/Controller.h
    include/Doors.h
    include/Gates.h
    include/LevelSelect.h
)

# Create executable
add_executable(hot_and_cold ${SOURCES} ${HEADERS})

# Include directories
target_include_directories(hot_and_cold PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Link SFML libraries
target_link_libraries(hot_and_cold
    sfml-graphics
    sfml-window
    sfml-system
)

# Copy data folder to build directory
add_custom_command(TARGET hot_and_cold POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/data $<TARGET_FILE_DIR:hot_and_cold>/data
)

# Install target
install(TARGETS hot_and_cold DESTINATION bin)
install(DIRECTORY data DESTINATION bin)

# Print configuration
message(STATUS "Hot and Cold - Configuration Summary")
message(STATUS "  C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "  Build Type: ${CMAKE_BUILD_TYPE}")
message(STATUS "  SFML Version: ${SFML_VERSION}")
