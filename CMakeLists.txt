cmake_minimum_required(VERSION 3.0)
project(SDLCircle)

set(CMAKE_CXX_STANDARD 17)

# Ensure .html output if using Emscripten
if("${CMAKE_CXX_COMPILER}" MATCHES "em\\+\\+")
    set(CMAKE_EXECUTABLE_SUFFIX ".html")
endif()

# Emscripten requires flags for both compiler and linker phases
set(EM_FLAGS "-sUSE_SDL=2 -sWASM=1 -sALLOW_MEMORY_GROWTH=1 -sEXIT_RUNTIME=1 -sFULL_ES2=1 -sOFFSCREEN_FRAMEBUFFER=1 -sENVIRONMENT=web")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${EM_FLAGS}")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${EM_FLAGS}")

add_executable(main
    main.cpp
    drawCircle.cpp
    fish.cpp
)
