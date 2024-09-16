cmake_minimum_required(VERSION 3.26.5)

# Set the project name
project(Chess)

# Set the C++ standard
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Find the SDL2 package installed natively
find_package(SDL2 REQUIRED)

# Include SDL2 header files
include_directories(${SDL2_INCLUDE_DIRS})

# Add source files
set(SOURCES
	src/main.cpp
	src/AI.cpp
	src/Board.cpp
	src/Check.cpp
	src/CheckUtils.cpp
	src/Game.cpp
	src/GUI.cpp
	src/Piece.cpp
)

# Add the source files for your project
add_executable(${PROJECT_NAME} ${SOURCES})

# Link SDL2 library to your project
target_link_libraries(${PROJECT_NAME} 
	PRIVATE
	${SDL2_LIBRARIES}
)

