#ifndef GL_TOOLS_H
#define GL_TOOLS_H

#include <GL/glew.h>

namespace GL 
{
    bool gltGLEWSetup(void);
    bool gltCheckExt(const GLubyte *extensionStr, const GLubyte *checkExtension);

    bool gltLoadShader(GLuint shader, char* fn);
    GLint gltGetUniLoc(GLuint program, const GLchar *name);
    GLboolean gltInitShaders(char *fn, char *vn, GLuint &programobject);

    bool gltCheckError(const char *file, int line);
    #define gltCheckErrors() gltCheckError(__FILE__,__LINE__)
}

#endif // GL_TOOLS_H
