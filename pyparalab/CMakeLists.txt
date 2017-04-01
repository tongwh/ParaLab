set(SOURCE_SWIG_SUBDIR  src)
set(OUTPUT_SWIG_SUBDIR  pyparalab)
set(SOURCE_SWIG_DIR  ${PROJECT_SOURCE_DIR}/${SOURCE_SWIG_SUBDIR})
set(OUTPUT_SWIG_DIR  ${PROJECT_BINARY_DIR}/${OUTPUT_SWIG_SUBDIR}/${CMAKE_CFG_INTDIR})
set(SOURCE_SWIG_DEPS ${SOURCE_SWIG_DIR}/ui/document.h)
set(SOURCE_SWIG_FILE ${SOURCE_SWIG_DIR}/ui/document.h)
set(OUTPUT_SWIG_CPP  ${OUTPUT_SWIG_DIR}/document_wrap.cpp)
set(OUTPUT_SWIG_PY   ${OUTPUT_SWIG_DIR}/pyparalab.py)
set(BOOST $ENV{BOOSTROOT})

# run SWIG
include_directories(BEFORE ${PROJECT_SOURCE_DIR}/src/ui ${PYTHON_INCLUDE_PATH} ${PYTHON_INCLUDE_DIRS} ${PROJECT_SOURCE_DIR})

add_custom_command(OUTPUT ${OUTPUT_SWIG_PY} ${OUTPUT_SWIG_CPP}
    COMMAND ${CMAKE_COMMAND} -E remove ${OUTPUT_SWIG_CPP} ${OUTPUT_SWIG_PY}  # remove stale files in case swig fails
    COMMAND swig -v -python -c++ -o ${OUTPUT_SWIG_CPP} -outdir ${OUTPUT_SWIG_DIR} ${SOURCE_SWIG_FILE}
    #MAIN_DEPENDENCY ${SOURCE_SWIG_FILE}
    DEPENDS ${SOURCE_SWIG_DEPS}
    IMPLICIT_DEPENDS  # get implicit dependencies from the source file
    VERBATIM
)

# Compile into python-loadable library
add_library(pyparalab SHARED ${OUTPUT_SWIG_CPP})
add_dependencies(pyparalab ${OUTPUT_SWIG_CPP})

if (WIN32)
    # On windows, the file extension needs to be pyd for python to load it properly
    set(PYLIB_SUFFIX .pyd)
else()
    set(PYLIB_SUFFIX .so) 
endif()
set(PYLIB_NAME _pyparalab${PYLIB_SUFFIX})

set_target_properties(pyparalab
    PROPERTIES
    OUTPUT_NAME ${PYLIB_NAME}
    PREFIX ""
    SUFFIX ""
)

# Make sure this is called AFTER the output file name is set
add_link_settings(pyparalab)

# COPY TO FINAL DIRECTORY
# Copy pyparalab.py and the shared library into
#       ${CMAKE_SOURCE_DIR}/${CMAKE_CFG_INTDIR}
# from
#       ${OUTPUT_SWIG_DIR}

set(SOURCE_PY ${OUTPUT_SWIG_PY})
set(SOURCE_PYLIB ${OUTPUT_SWIG_DIR}/${PYLIB_NAME})
if (MSVC)
    set(FINAL_DIR ${CMAKE_SOURCE_DIR}/${CMAKE_CFG_INTDIR})
else()
    set(FINAL_DIR ${CMAKE_SOURCE_DIR}/${CMAKE_BUILD_TYPE})
endif()
set(FINAL_PY ${FINAL_DIR}/pyparalab.py)
set(FINAL_PYLIB ${FINAL_DIR}/${PYLIB_NAME})

message("Final location of python module: ${FINAL_DIR}")
add_custom_command(OUTPUT ${FINAL_PYLIB} ${FINAL_PY}
    POST_BUILD
    COMMAND echo "Copying files to ${FINAL_DIR}/"
    COMMAND ${CMAKE_COMMAND} -E make_directory ${FINAL_DIR}
    COMMAND ${CMAKE_COMMAND} -E copy ${SOURCE_PYLIB} ${FINAL_PYLIB}
    COMMAND ${CMAKE_COMMAND} -E copy ${SOURCE_PY} ${FINAL_PY}
    DEPENDS ${SOURCE_PYLIB} ${SOURCE_PY}
)

add_custom_target(pyparalab_copy ALL
    DEPENDS ${FINAL_PY} ${FINAL_PYLIB}
)

# Make sure there is something to copy before copying
add_dependencies(pyparalab_copy pyparalab)
