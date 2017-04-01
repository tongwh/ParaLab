# the CMakeLists.txt file

project(ParaLab)

# ################################################################################################# #
# find cmake
# ################################################################################################# #

# check the cmake version and set policy
cmake_minimum_required(VERSION 2.6.2)
if ("${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION}" VERSION_GREATER 2.6)
    if ("${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION}.${CMAKE_PATCH_VERSION}" VERSION_GREATER 2.8.3)
        cmake_policy(VERSION 2.8.4)
     else()
        cmake_policy(VERSION 2.6)
    endif()
endif()

set(CMAKE_ALLOW_LOOSE_LOOP_CONSTRUCTS true)
#set(CMAKE_VERBOSE_MAKEFILE on)
 
if (COMMAND cmake_policy)
    cmake_policy(SET CMP0003 NEW)  
endif()

# force out of source builds
if ("${PROJECT_SOURCE_DIR}" STREQUAL "${PROJECT_BINARY_DIR}")
    message(FATAL_ERROR "In-source builds are not allowed."
    " Remove CMakeCache.txt and CMakeFiles/ from the source"
    " directory and try 'mkdir build; cd build; cmake ..'")
endif()

# set default cmake build type to RelWithDebInfo (None Debug Release RelWithDebInfo MinSizeRel)
if (CMAKE_BUILD_TYPE)
else()
    set(CMAKE_BUILD_TYPE "Release")
endif()

message(STATUS "BUILD TYPE: ${CMAKE_BUILD_TYPE}")

# ################################################################################################# #
# find CGAL and its components
# ################################################################################################# #

find_package(CGAL REQUIRED COMPONENTS)

if (NOT CGAL_FOUND)
    message(STATUS "This project requires the CGAL library, and will not be compiled.")
    return()  
endif()

include(${CGAL_USE_FILE})

# Boost and its components
find_package(Boost REQUIRED COMPONENTS)

if (NOT Boost_FOUND)
    message(STATUS "This project requires the Boost library, and will not be compiled.")
    return()  
endif()

# ################################################################################################# #
# find Qt and its components
# ################################################################################################# #
find_package(Qt4 REQUIRED COMPONENTS QtCore QtGui QtOpenGL)

if (NOT QT_FOUND)
    message(STATUS "This project requires the Qt library, and will not be compiled.")
    return()  
endif()

include(${QT_USE_FILE})

# ################################################################################################# #
# find Python
# ################################################################################################# #

find_package(PythonLibs REQUIRED)
if (NOT PYTHONLIBS_FOUND)
    message(STATUS "This project requies the Python library, and will not be compiled.")
else()
    message(STATUS "Python")
    message(STATUS "    INCLUDE: ${PYTHON_INCLUDE_DIRS}")
    message(STATUS "    LIBRARY: ${PYTHON_LIBRARY}")
endif()

# ################################################################################################# #
# load local setting
# ################################################################################################# #

include(./settings.cmake)

# ################################################################################################# #
# set dependencies and link libraries
# ################################################################################################# #
function(add_link_settings var)
    add_dependencies(${var} ParaLab)
    target_link_libraries(${var} ParaLab
        ${QT_LIBRARIES} 
        ${OPENGL_LIBRARY} 
        ${GLEW_LIBRARY} 
        ${GLUT_LIBRARY}
	${PYTHON_LIBRARY}
    )

  if (WIN32 AND MSVC)
      # To remove the console window along with your GUI program, you must do two things. 
      # First, add ${QT_QTMAIN_LIBRARY} to your target_link_libraries command. 
      # Second, you must specify /SUBSYSTEM:WINDOWS for all build configurations (Release, Debug, etc) 
      # that you want to not pop up the console window.
      #set_target_properties(${TARGET_NAME} PROPERTIES LINK_FLAGS_RELEASE "/SUBSYSTEM:WINDOWS")

      # add some macro defintions
      add_definitions(-DHAVE_ROUND) # for removing warning of python
      
      # add the support of visual leak detector
      target_link_libraries(${var} user32.lib)
      target_link_libraries(${var} ${VLD_PATH}/lib/win64/vld.lib)

      # Matlab engine
      target_link_libraries(${var}
          ${MATLAB_STATIC_DIR}/libeng.lib
          ${MATLAB_STATIC_DIR}/libmat.lib
          ${MATLAB_STATIC_DIR}/libmx.lib
      )
  endif()

  set_target_properties(${var}
      PROPERTIES
      INSTALL_RPATH_USE_LINK_PATH true
  )
endfunction()

# ################################################################################################# #
# add subdirectories to the build
# ################################################################################################# #
add_subdirectory(src)
add_subdirectory(pyparalab)
