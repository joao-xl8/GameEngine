cmake_minimum_required(VERSION 3.16)
project(GameEngine VERSION 1.0.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set default build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Compiler-specific options
if(MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# Find SFML
find_package(PkgConfig)
if(PkgConfig_FOUND)
    pkg_check_modules(SFML sfml-all)
endif()

if(NOT SFML_FOUND)
    find_package(SFML 2.5 COMPONENTS graphics window system audio)
    if(NOT SFML_FOUND)
        message(STATUS "SFML not found via find_package, trying manual detection...")
        
        # Manual SFML detection for different platforms
        if(WIN32)
            # Windows - look for SFML in common locations
            set(SFML_ROOT "C:/SFML" CACHE PATH "SFML root directory")
            find_path(SFML_INCLUDE_DIR SFML/Graphics.hpp
                PATHS ${SFML_ROOT}/include
                      C:/SFML/include
                      C:/msys64/mingw64/include
                      $ENV{SFML_ROOT}/include)
            
            find_library(SFML_GRAPHICS_LIBRARY sfml-graphics
                PATHS ${SFML_ROOT}/lib
                      C:/SFML/lib
                      C:/msys64/mingw64/lib
                      $ENV{SFML_ROOT}/lib)
                      
        elseif(APPLE)
            # macOS - look for Homebrew installation
            execute_process(COMMAND brew --prefix OUTPUT_VARIABLE HOMEBREW_PREFIX OUTPUT_STRIP_TRAILING_WHITESPACE)
            find_path(SFML_INCLUDE_DIR SFML/Graphics.hpp
                PATHS ${HOMEBREW_PREFIX}/opt/sfml@2/include
                      ${HOMEBREW_PREFIX}/opt/sfml/include
                      /opt/homebrew/opt/sfml@2/include
                      /opt/homebrew/opt/sfml/include
                      /usr/local/opt/sfml@2/include
                      /usr/local/opt/sfml/include)
                      
            find_library(SFML_GRAPHICS_LIBRARY sfml-graphics
                PATHS ${HOMEBREW_PREFIX}/opt/sfml@2/lib
                      ${HOMEBREW_PREFIX}/opt/sfml/lib
                      /opt/homebrew/opt/sfml@2/lib
                      /opt/homebrew/opt/sfml/lib
                      /usr/local/opt/sfml@2/lib
                      /usr/local/opt/sfml/lib)
                      
        else()
            # Linux - standard system locations
            find_path(SFML_INCLUDE_DIR SFML/Graphics.hpp
                PATHS /usr/include
                      /usr/local/include)
                      
            find_library(SFML_GRAPHICS_LIBRARY sfml-graphics
                PATHS /usr/lib
                      /usr/local/lib
                      /usr/lib/x86_64-linux-gnu)
        endif()
        
        if(SFML_INCLUDE_DIR AND SFML_GRAPHICS_LIBRARY)
            set(SFML_FOUND TRUE)
            message(STATUS "SFML found manually: ${SFML_INCLUDE_DIR}")
        endif()
    endif()
endif()

if(NOT SFML_FOUND)
    message(FATAL_ERROR "SFML not found! Please install SFML development libraries.")
endif()

# Source files
set(SOURCES
    main.cpp
    src/animation.cpp
    src/assets.cpp
    src/entity.cpp
    src/entity_manager.cpp
    src/game_engine.cpp
    src/scene.cpp
    src/scene_menu.cpp
    src/scene_options.cpp
    src/scene_play.cpp
    src/vec2.cpp
)

# ImGui sources
set(IMGUI_SOURCES
    include/imgui/imgui.cpp
    include/imgui/imgui_draw.cpp
    include/imgui/imgui_tables.cpp
    include/imgui/imgui_widgets.cpp
    include/imgui/imgui-SFML.cpp
)

# Create executable
add_executable(${PROJECT_NAME} ${SOURCES} ${IMGUI_SOURCES})

# Include directories
target_include_directories(${PROJECT_NAME} PRIVATE
    src
    include/imgui
)

# Link libraries
if(SFML_FOUND AND TARGET sfml-graphics)
    # Modern SFML with targets
    target_link_libraries(${PROJECT_NAME} 
        sfml-graphics 
        sfml-window 
        sfml-system 
        sfml-audio
    )
elseif(SFML_LIBRARIES)
    # pkg-config SFML
    target_include_directories(${PROJECT_NAME} PRIVATE ${SFML_INCLUDE_DIRS})
    target_link_libraries(${PROJECT_NAME} ${SFML_LIBRARIES})
else()
    # Manual SFML
    target_include_directories(${PROJECT_NAME} PRIVATE ${SFML_INCLUDE_DIR})
    
    # Find all SFML libraries
    get_filename_component(SFML_LIB_DIR ${SFML_GRAPHICS_LIBRARY} DIRECTORY)
    
    find_library(SFML_WINDOW_LIBRARY sfml-window PATHS ${SFML_LIB_DIR})
    find_library(SFML_SYSTEM_LIBRARY sfml-system PATHS ${SFML_LIB_DIR})
    find_library(SFML_AUDIO_LIBRARY sfml-audio PATHS ${SFML_LIB_DIR})
    
    target_link_libraries(${PROJECT_NAME}
        ${SFML_GRAPHICS_LIBRARY}
        ${SFML_WINDOW_LIBRARY}
        ${SFML_SYSTEM_LIBRARY}
        ${SFML_AUDIO_LIBRARY}
    )
endif()

# Platform-specific libraries
if(WIN32)
    target_link_libraries(${PROJECT_NAME} opengl32 gdi32 winmm)
elseif(APPLE)
    find_library(OPENGL_LIBRARY OpenGL)
    find_library(FOUNDATION_LIBRARY Foundation)
    target_link_libraries(${PROJECT_NAME} ${OPENGL_LIBRARY} ${FOUNDATION_LIBRARY})
else()
    find_package(OpenGL REQUIRED)
    find_package(X11 REQUIRED)
    target_link_libraries(${PROJECT_NAME} ${OPENGL_LIBRARIES} ${X11_LIBRARIES} pthread)
endif()

# Set output directory
set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

# Copy assets to build directory (if they exist)
if(EXISTS ${CMAKE_SOURCE_DIR}/assets)
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/assets $<TARGET_FILE_DIR:${PROJECT_NAME}>/assets
    )
endif()

if(EXISTS ${CMAKE_SOURCE_DIR}/metadata)
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/metadata $<TARGET_FILE_DIR:${PROJECT_NAME}>/metadata
    )
endif()

if(EXISTS ${CMAKE_SOURCE_DIR}/config.txt)
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
        ${CMAKE_SOURCE_DIR}/config.txt $<TARGET_FILE_DIR:${PROJECT_NAME}>/config.txt
    )
endif()

# Installation
install(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION bin
)

if(EXISTS ${CMAKE_SOURCE_DIR}/assets)
    install(DIRECTORY assets DESTINATION share/${PROJECT_NAME})
endif()

if(EXISTS ${CMAKE_SOURCE_DIR}/metadata)
    install(DIRECTORY metadata DESTINATION share/${PROJECT_NAME})
endif()

# Print build information
message(STATUS "")
message(STATUS "GameEngine Build Configuration:")
message(STATUS "==============================")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "C++ standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "System: ${CMAKE_SYSTEM_NAME}")
message(STATUS "SFML found: ${SFML_FOUND}")
if(SFML_INCLUDE_DIR)
    message(STATUS "SFML include: ${SFML_INCLUDE_DIR}")
endif()
message(STATUS "")
