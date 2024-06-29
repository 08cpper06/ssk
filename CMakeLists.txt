cmake_minimum_required(VERSION 3.8)


project(ssk)

if (MSVC)
	set(CMAKE_CXX_FLAGS "/std:c++20 /EHsc /DWIN64")
else()
	set(CMAKE_CXX_FLAGS "-std=c++20 -Wdeprecated-declarations")
endif()

add_executable(${PROJECT_NAME}
	./src/main.cpp
	./src/lexer.cpp
	./src/parser.cpp
	./src/runtime.cpp
)

target_include_directories(${PROJECT_NAME} PUBLIC ./include)