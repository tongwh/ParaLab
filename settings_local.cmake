# the setting_local.cmake file

# ################################################################################################# #
# for 64-bit compilatoin on Windows
# ################################################################################################# #

# switch to multi-processor compilation
add_definitions(/MP)  

# to add debug information for relase mode
#set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /Zi")
#set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS} /DEBUG")  

# add Matlab include and link directory
set(MATLAB_PLATFORM "Win64")
set(MATLAB_STATIC_DIR ${MATLAB_DIR}/extern/lib/${MATLAB_PLATFORM}/microsoft)
include_directories(BEFORE ${MATLAB_DIR}/extern/include ${MATLAB_DIR}/simulink/include ${MATLAB_DIR}/extern/include/cpp)
link_directories(${MATLAB_DIR}/bin/${MATLAB_PLATFORM}) # Windows links statically

# add gmm and eigen directory
include_directories("D:\\Lib\\Eigen-3.1.3")
include_directories("D:\\Lib\\Gmm-4.2\\include")

# add OpenGL related directories
set(OPENGL_LIBRARY  opengl32.lib glu32.lib)
include_directories("C:\\Program Files\\Microsoft SDKs\\Windows\\v6.0A\\Include")
set(GLUT_LIBRARY "C:\\Program Files\\Microsoft SDKs\\Windows\\v6.0A\\Lib\\x64\\glut32.lib")
set(GLEW_LIBRARY "C:\\Program Files\\Microsoft SDKs\\Windows\\v6.0A\\Lib\\x64\\glew32.lib")
#set(GLUT_LIBRARY "D:\\Program Files (x86)\\Microsoft Visual Studio 10.0\\VC\\lib\\amd64\\glut32.lib")
#set(GLEW_LIBRARY "D:\\Program Files (x86)\\Microsoft Visual Studio 10.0\\VC\\lib\\amd64\\glew32.lib")
