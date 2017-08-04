# min version req'd
cmake_minimum_required(VERSION 3.1)

# name of project
project(meshing)

# add C++11 standard
set(CMAKE_CXX_STANDARD 11)

# set to release build type
set(CMAKE_BUILD_TYPE Release)

# output folder for binaries
set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/bin)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR})

# copy all shader files
file(GLOB GLSL_FILES "src/*.glsl")

# compile flags and library locations
if(WIN32)
    set(CMAKE_CXX_FLAGS_RELEASE "/O2")
    set(CMAKE_CXX_FLAGS "/EHsc /W3 /MT /std:c++latest")
    include_directories(${CMAKE_SOURCE_DIR}/include)
    link_directories(${CMAKE_SOURCE_DIR}/lib "C:/Program Files (x86)/Microsoft SDKs/Windows/v7.1A/Lib/x64")
    set(PROJECT_LINK_LIBS glfw3dll glew32 OpenGL32 )
    file(GLOB DLL_FILES "lib/*.dll")
    file(COPY ${DLL_FILES} DESTINATION ${CMAKE_BINARY_DIR}/Release NO_SOURCE_PERMISSIONS)
    file(COPY ${DLL_FILES} DESTINATION ${CMAKE_BINARY_DIR}/Debug NO_SOURCE_PERMISSIONS)
    file(COPY ${GLSL_FILES} DESTINATION ${CMAKE_BINARY_DIR}/Release NO_SOURCE_PERMISSIONS)
    file(COPY ${GLSL_FILES} DESTINATION ${CMAKE_BINARY_DIR}/Debug NO_SOURCE_PERMISSIONS)
elseif(UNIX)
    set(CMAKE_CXX_FLAGS_RELEASE "-O3")
    set(CMAKE_CXX_FLAGS_DEBUG "-O0 -g")
    set(CMAKE_CXX_FLAGS "-Wall -std=c++11 -Wfatal-errors -pthread")
    include_directories("/usr/include")
    link_directories("/usr/lib")
    set(PROJECT_LINK_LIBS GLEW GL glfw)
    file(COPY ${GLSL_FILES} DESTINATION ${CMAKE_BINARY_DIR} NO_SOURCE_PERMISSIONS)
endif()





# wildcard add source files like so:
file(GLOB SOURCES "src/*.cpp")

add_executable(main ${SOURCES})
target_link_libraries(main ${PROJECT_LINK_LIBS})

# CMAKE_BINARY_DIR is same as CMAKE_SOURCE_DIR if building in-source, otherwise is the top level dir of build tree
# CMAKE_SOURCE_DIR is dir where cmake was started aka top level source dir
# EXECUTABLE_OUTPUT_PATH is where cmake should put all executable files
# LIBRARY_OUTPUT_PATH is where cmake should put all libraries
# PROJECT_NAME is the name of the project set by project() command
# PROJECT_SOURCE_DIR contains full path to root of source directory


# to add a file (or your wildcard source files) like so:
#add_executable(main ${SOURCES})

# to build your project do the following:

# create cmake configuration files inside folder named 'build'
# cmake -H. -Bbuild

# build executable using build files in build directory
# cmake --build build --config Release

# can also create build files by entering build folder then calling cmake on the root dir
# cmake ..

# to clean the project build files simply clear out the build directory:
# rm -r build/*

# LIBRARY SECTION ----------------------------------

# do cmake_minimum_required(version), project(name), include_directories(include), file(GLOB SOURCES "src/*.cpp") as normal

# do a release build
# set(CMAKE_BUILD_TYPE Release)

# generate a shared library from the sources
# add_library(libname SHARED ${SOURCES})

# set location for lib install; unix example follows. is installed with sudo make install command, or cmake --install (probably)
# install(TARGETS name DESTINATION /usr/lib)

# static linkage of libs:
# add_library(name STATIC ${SOURCES})

# USING A LIBRARY IN CMAKE ------------------------------

# for shared lib linkage:
# set(PROJECT_LINK_LIBS libname.so)
# link_directories(libdirectory)

# for static lib linkage:
# set(PROJECT_LINK_LIBS libname.a)
# link_directories(libdirectory)

# must also include the headers for any type of library linkage
# include_directories(library_include_directory)

# last steps; SOURCES are the non-library source files
# add_executable(exename ${SOURCES})
# target_link_libraries(exename ${PROJECT_LINK_LIBS})
