# the CMakeLists.txt file

# ################################################################################################# #
# set compiler's options
# ################################################################################################# #

set(TARGET_NAME ParaLab)
set(DIRS sys_util ui mesh)

include_directories(BEFORE .. . ${DIRS}) 

# ################################################################################################# #
# set source files
# ################################################################################################# #

set(HEADER_SYS_UTIL
    sys_util/stdafx.h
    sys_util/util.h
    sys_util/logging.h
    sys_util/gltools.h
    sys_util/texture.h
)

set(SOURCE_SYS_UTIL
    sys_util/util.cpp
    sys_util/logger.cpp
    sys_util/logging.cpp
    sys_util/gltools.cpp
    sys_util/texture.cpp
    sys_util/texture_helper.cpp
)

set(HEADER_UI
    ui/render_ui_state.h
    ui/render_ui.h
    ui/document_impl.h
    ui/document.h
)

set(SOURCE_UI
    ui/document.cpp
    ui/render_ui.cpp
)

set(SOURCE_UI_QRC
    ui/paralab.qrc
)

set(HEADER_MESH
    mesh/ep_vertex.h
    mesh/ep_halfedge.h
    mesh/ep_facet.h
    mesh/ep_base.h
)

set(SOURCE_MESH
)

# ################################################################################################# #
# wrap source files
# ################################################################################################# #

qt4_add_resources(SOURCE_UI_RCC ${SOURCE_UI_QRC})

# ################################################################################################# #
# create project
# ################################################################################################# #

set(HEADERS ${HEADER_SYS_UTIL} ${HEADER_UI} ${HEADER_MESH})
set(SOURCES ${SOURCE_SYS_UTIL} ${SOURCE_UI} ${SOURCE_UI_RCC} ${SOURCE_MESH})
add_library(${TARGET_NAME} STATIC ${SOURCES} ${HEADERS})

# These are the (virtual) folders in which source and header files are organized in Visual Studio.
foreach(group ${DIRS})
    STRING(TOUPPER ${group} group_suffix)
    source_group(${group} FILES ${HEADER_${group_suffix}} ${SOURCE_${group_suffix}})
endforeach(group)
