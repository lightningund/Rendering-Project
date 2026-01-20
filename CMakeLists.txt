cmake_minimum_required(VERSION 3.10.0)
project(Template VERSION 0.1.0 LANGUAGES CXX)

add_executable(${PROJECT_NAME} main.cpp cell.cpp vec.cpp map.cpp ray.cpp task.cpp)

target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_20)

# Need to define SFML_STATIC as a preprocessor def
target_compile_definitions(${PROJECT_NAME} PUBLIC SFML_STATIC)

# Where to look for the header files
target_include_directories(${PROJECT_NAME} PUBLIC "D:/CS_ALIAS/C++/@LIBS/MSVC/x64/include/")

# Where to look for the .lib files
target_link_directories(${PROJECT_NAME} PUBLIC "D:/CS_ALIAS/C++/@LIBS/MSVC/x64/lib/")

# The debug libraries
set(D_LIBS sfml-main-d sfml-graphics-s-d sfml-window-s-d sfml-system-s-d opengl32 freetype winmm gdi32)
# The release libraries
set(LIBS sfml-main sfml-graphics-s sfml-window-s sfml-system-s opengl32 freetype winmm gdi32)

# Select one of them to link to the project
target_link_libraries(${PROJECT_NAME} PRIVATE
	$<$<CONFIG:DEBUG>: ${D_LIBS}>
	$<$<CONFIG:RELEASE>: ${LIBS}>
)