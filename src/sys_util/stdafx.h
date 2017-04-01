#ifndef STDAFX_H
#define STDAFX_H

// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently,
// but are changed infrequently

#ifdef _WIN32
#include <windows.h>
#endif

// general stuff
#include <assert.h>
#include <vector>
#include <string>

// OpenGL related stuff
#include <GL/glew.h>
#include <GL/glut.h>
#include <GL/gl.h>
#include <GL/glu.h>

#define DO_NOT_DISABLE_NDEBUG

#endif // STDAFX_H

