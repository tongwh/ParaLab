#include "gltools.h"

#include <QString>
#include <QDebug>
#include <QFile>
#include <QDataStream>

#include <iostream>
using namespace std;

bool GL::gltGLEWSetup(void)
{
    // GLEW initialize
    GLint GlewInitResult = glewInit();
    if (GlewInitResult != GLEW_OK)
    {
        const GLubyte* errorStr = glewGetErrorString(GlewInitResult);
        int size = (int) strlen(reinterpret_cast<const char*>(errorStr));
        QString string = QString::fromUtf8(reinterpret_cast<const char*>(errorStr), size);;
        cerr << "GLEW error " << string.toStdString() << "." << endl;
    }

    // check OpenGL version
    int GLVersion[2];
    glGetIntegerv(GL_MAJOR_VERSION, &GLVersion[0]);
    glGetIntegerv(GL_MINOR_VERSION, &GLVersion[1]);
    cout << "OpenGL initialized: version " << glGetString(GL_VERSION) 
         << "£¬ GLSL "<< glGetString(GL_SHADING_LANGUAGE_VERSION) << endl;

    // check sample buffer 
    static GLint buf[1], sbuf[1];
    glGetIntegerv(GL_SAMPLE_BUFFERS_ARB, buf);
    glGetIntegerv(GL_SAMPLES_ARB, sbuf);
    cout << "OpenGL sample buffers is: " << buf[0] << ", samples is: " << sbuf[0] << "." << endl;

    return GlewInitResult;
}

bool GL::gltCheckExt(const GLubyte *extensionStr, const GLubyte *checkExtension)
{
    // using strstr for extension search can result in false positives because of substring matches.
    const GLubyte  *s = extensionStr;
    GLint len = (GLint) strlen((const char *)checkExtension);

    while ((s = (const unsigned char *)strstr((const char *)s, (const char *)checkExtension)) != NULL) 
    {
        s += len;

        if ((*s == ' ') || (*s == '\0')) {
            return (GL_TRUE);
        }
    }

    return (GL_FALSE);
}

bool GL::gltLoadShader(GLuint shader, char* fn)
{
    // load a shader from file
    QFile file(fn);
    if (!file.open(QIODevice::ReadOnly))
    {
        return false;
    }

    int length = file.size();
    QDataStream in(&file);

    GLubyte *buf;
    buf = new GLubyte[length+1];

    in.readRawData((char *)buf, length);
    buf[length] = '\0';				// make it a regular C string so str* functions work

    glShaderSource(shader, 1, (const char**)&buf, &length);

    bool ret = true;
    if (glGetError() != 0)
    {
        ret = false;
    }

    delete[] buf;
    return ret;
}

GLint GL::gltGetUniLoc(GLuint program, const GLchar *name)
{
    // get the location of a uniform variable by a given name
    GLint loc;

    loc = glGetUniformLocation(program, name);

    if (loc == -1)
        printf("No such uniform named \"%s\"\n", name);

    return loc;
}

GLboolean GL::gltInitShaders(char *fn, char *vn, GLuint &programobject)
{
    // initialize vertex and fragment shader from files
    GLchar *pInfoLog = NULL;
    GLint compiled = GL_FALSE;
    GLint linked = GL_FALSE;
    GLint maxLength, length;

    // create shader and program objects.
    programobject = glCreateProgram();

    GLuint VertexShaderObject = glCreateShader(GL_VERTEX_SHADER);
    gltLoadShader(VertexShaderObject, fn);

    GLuint FragmentShaderObject = glCreateShader(GL_FRAGMENT_SHADER);
    gltLoadShader(FragmentShaderObject, vn);

    // populate the program object with the compiled shader
    glAttachShader(programobject, VertexShaderObject);
    glAttachShader(programobject, FragmentShaderObject);

    // compile the vertex shader and fragment shader
    glCompileShader(VertexShaderObject);
    glGetShaderiv(VertexShaderObject, GL_COMPILE_STATUS, &compiled);
    if (compiled == !GL_TRUE)
    {
        glGetShaderiv(VertexShaderObject, GL_INFO_LOG_LENGTH, &maxLength);

        pInfoLog = (GLchar *) new GLchar[maxLength * sizeof(GLchar)];
        glGetShaderInfoLog(VertexShaderObject, maxLength, &length, pInfoLog);
        cerr << "Vertex shader compiled failed: " << pInfoLog << ". " << endl;
        delete[] pInfoLog;

        return GL_FALSE;
    }

    glCompileShader(FragmentShaderObject);
    glGetShaderiv(FragmentShaderObject, GL_COMPILE_STATUS, &compiled);
    if (compiled == !GL_TRUE)
    {
        glGetShaderiv(FragmentShaderObject, GL_INFO_LOG_LENGTH, &maxLength);

        pInfoLog = (GLchar *) new GLchar[maxLength * sizeof(GLchar)];
        glGetShaderInfoLog(FragmentShaderObject, maxLength, &length, pInfoLog);
        cerr << "Fragment shader compiled failed: " << pInfoLog << "." << endl;
        delete[] pInfoLog;

        return GL_FALSE;
    }

    // Link the program and print out the linker log file
    glLinkProgram(programobject);
    glGetProgramiv(programobject, GL_LINK_STATUS, &linked);
    if (linked == !GL_TRUE)
    {
        glGetProgramiv(programobject, GL_INFO_LOG_LENGTH, &maxLength);

        pInfoLog = (GLchar *) new GLchar[maxLength * sizeof(GLchar)];
        glGetProgramInfoLog(programobject, maxLength, &length, pInfoLog);
        cerr << "Program linked failed: " << pInfoLog << "." << endl;
        delete[] pInfoLog;

        return GL_FALSE;
    }

    // delete the shader object to go away as soon as it is detached from 
    // the program object it is attached to.
    glDeleteShader(VertexShaderObject);
    glDeleteShader(FragmentShaderObject);

    // return link state
    return linked;
}

bool GL::gltCheckError(const char *file, int line) 
{
    bool bFoundError = false;
    GLenum error = glGetError();

    // check OpenGL status
    while (error != GL_NO_ERROR)
    {
        bFoundError = true;
        QString string;
        switch(error)
        {
        case GL_INVALID_ENUM: 
            string = "INVALID_ENUM";           
            break;
        case GL_INVALID_VALUE:
            string = "INVALID_VALUE";
            break;
        case GL_INVALID_OPERATION: 
            string = "INVALID_OPERATION";      
            break;
        case GL_INVALID_FRAMEBUFFER_OPERATION:
            string = "INVALID_FRAMEBUFFER_OPERATION";
            break;
        case GL_STACK_OVERFLOW:
            string = "GL_STACK_OVERFLOW";
            break;
        case GL_STACK_UNDERFLOW:
            string = "GL_STACK_UNDERFLOW";
            break;
        case GL_OUT_OF_MEMORY:
            string = "OUT_OF_MEMORY";
            break;
        case GL_TABLE_TOO_LARGE:
            string = "GL_TABLE_TOO_LARGE";
            break;
        }
        cerr << "GL_" << string.toStdString() << " - " << file << ':' << line << "." << endl;		
        error = glGetError();
    }

    // check frame buffer status
    GLenum fboStatus = glCheckFramebufferStatus(GL_DRAW_FRAMEBUFFER);
    if (fboStatus != GL_FRAMEBUFFER_COMPLETE)
    {
        bFoundError = true;
        QString string;
        switch (fboStatus)
        {
        case GL_FRAMEBUFFER_UNDEFINED:
            string = "GL_FRAMEBUFFER_UNDEFINED";
            break;
        case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
            string = "GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT";
            break;
        case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
            string = "GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT";
            break;
        case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
            string = "GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER";
            break;
        case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:
            string = "GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER";
            break;
        case GL_FRAMEBUFFER_UNSUPPORTED:
            string = "GL_FRAMEBUFFER_UNSUPPORTED";
            break;
        case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE:
            string = "GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE";
            break; 
        case GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS:
            string = "GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS";
            break;
        }
        cerr << "The frame buffer is not complete - " << string.toStdString() << "." << endl;
    }

    return bFoundError;
}
