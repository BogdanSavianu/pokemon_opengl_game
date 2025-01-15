cmake_minimum_required(VERSION 3.28)
project(Lab9)

set(CMAKE_CXX_STANDARD 20)

if(APPLE)
    include_directories(/usr/local/include /opt/homebrew/include)
    link_directories(/usr/local/lib /opt/homebrew/lib)
endif()

if(APPLE)
    find_library(OPENAL_LIBRARY OpenAL)
    set(OPENAL_INCLUDE_DIR "/System/Library/Frameworks/OpenAL.framework/Headers")
else()
    find_package(OpenAL REQUIRED)
endif()

if(APPLE)
    find_library(SNDFILE_LIBRARY sndfile)
    find_path(SNDFILE_INCLUDE_DIR sndfile.h
        PATHS
        /usr/local/include
        /opt/homebrew/include
    )
else()
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(SNDFILE REQUIRED sndfile)
endif()

include_directories(
    ${PROJECT_SOURCE_DIR}
    ${OPENGL_INCLUDE_DIRS}
    ${GLEW_INCLUDE_DIRS}
    ${GLM_INCLUDE_DIRS}
    ${OPENAL_INCLUDE_DIR}
    ${SNDFILE_INCLUDE_DIR}
)

set(CORE_SOURCES 
    src/core/Engine.cpp
)

set(GRAPHICS_SOURCES 
    src/graphics/shaders/Shader.cpp
    src/graphics/models/Model3D.cpp
    src/graphics/models/Mesh.cpp 
    src/graphics/effects/Rain.cpp
)

set(ENTITIES_SOURCES
    src/entities/Pokemon.cpp
)

set(INPUT_SOURCES
    src/input/Controls.cpp  
)

set(AUDIO_SOURCES
    src/audio/AudioManager.cpp
)

set(CAMERA_SOURCES
    src/camera/Camera.cpp
)

set(UTILS_SOURCES
    src/utils/stb_image.cpp
    src/utils/tiny_obj_loader.cpp
)

add_executable(Lab9 
    src/main.cpp 
    ${CORE_SOURCES}
    ${GRAPHICS_SOURCES}
    ${ENTITIES_SOURCES}
    ${INPUT_SOURCES}
    ${AUDIO_SOURCES}
    ${CAMERA_SOURCES}
    ${UTILS_SOURCES}
)

target_link_libraries(Lab9 
    glfw 
    "-framework OpenGL"
    "-framework OpenAL"
    ${SNDFILE_LIBRARY}
)

set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME "Pokemon")

if(APPLE)
    target_compile_definitions(Lab9 PRIVATE
        GL_SILENCE_DEPRECATION
        AL_SILENCE_DEPRECATION
    )
endif()

file(COPY 
    ${CMAKE_SOURCE_DIR}/shaders 
    ${CMAKE_SOURCE_DIR}/objects 
    ${CMAKE_SOURCE_DIR}/sounds 
    DESTINATION ${CMAKE_BINARY_DIR}
)