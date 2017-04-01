# the setting.cmake file

# ################################################################################################# #
# set variables in terms of platform
# ################################################################################################# #

# For 64-bit compilation on 64-bit Windows
if (MSVC)
    set(MATLAB_DIR      "D:\\Program Files (x86)\\MATLAB2012")
    set(MATLAB_PLATFORM "win64")

# For 64-bit compilation on 64-bit linux
elseif ("${CMAKE_SYSTEM_NAME}" STREQUAL "Linux")
    set(MATLAB_DIR      "/usr/local/lib/matlab")
    set(MATLAB_PLATFORM "glnxa64")
    
# For 64-bit compilation on 64-bit mac
elseif ("${CMAKE_SYSTEM_NAME}" STREQUAL "Darwin")
    set(MATLAB_DIR      "/Applications/MATLAB_R2012a.app/")
    set(MATLAB_PLATFORM "maci64")    
endif()

if (WIN32 AND MSVC) # add the support of visual leak detector
    add_definitions(-DVLD_SUPPORT)
    set(VLD_PATH "D:/Program Files (x86)/Visual Leak Detector")
    include_directories(${VLD_PATH}/include)
endif()

# ################################################################################################# #
# Set the variables from the environment, if available
# ################################################################################################# #

foreach (envar 
         MATLAB_DIR 
	 MATLAB_PLATFORM
         )
    if (NOT "$ENV{${envar}}" STREQUAL "")
        set(${envar} "$ENV{${envar}}")
    endif()
endforeach(envar)

# ################################################################################################# #
# User-specific assignments to these variables
# ################################################################################################# #

include(./settings_local.cmake OPTIONAL)

# ################################################################################################# #
# Check for the existence of required variables
# ################################################################################################# #

foreach (envar 
         MATLAB_DIR 
	 MATLAB_PLATFORM
         )
    if ("${${envar}}" STREQUAL "")
        message(FATAL_ERROR "CMAKE variable ${envar} must be defined. Check settings_local.cmake.")
    endif()
endforeach(envar)

# ################################################################################################# #
# Output the variables being used
# ################################################################################################# #
message(STATUS "Using the following settings:")
message(STATUS "    MATLAB_DIR      = ${MATLAB_DIR}")
message(STATUS "    MATLAB_PLATFORM = ${MATLAB_PLATFORM}")
message(STATUS "    GLUT_LIBRARY    = ${GLUT_LIBRARY}") 
message(STATUS "    GLEW_LIBRARY    = ${GLEW_LIBRARY}") 
