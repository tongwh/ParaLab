import os
import sys
import math

# Verify existence of PyQt4
try:
    from PyQt4 import QtCore, QtGui, QtOpenGL
except ImportError:
    print 'PyQt4 not found.'
    sys.exit(1)

# Verify existence of OpenGL
try:
    from OpenGL import GL
except ImportError:
    print 'PyOpenGL not found.'
    sys.exit(1)

# OpenGL canvas
class GLWidget(QtOpenGL.QGLWidget):
    BUTTON_TO_INDEX = { QtCore.Qt.LeftButton: 1,
                        QtCore.Qt.MidButton: 2,
                        QtCore.Qt.RightButton: 3 }

    def __init__(self, app, parent=None):
        # set OpenGL format
        glFormat = QtOpenGL.QGLFormat(); 
        glFormat.setSamples(8)
        glFormat.setSampleBuffers(True)        
        
        super(GLWidget, self).__init__(glFormat,  parent)
        self.app = app

    def minimumSizeHint(self):
        return QtCore.QSize(100, 100)

    def sizeHint(self):
        return QtCore.QSize(1024, 800)

    def paintGL(self):
        if not self.app.hasDoc():
            GL.glClearColor(1.0, 1.0, 1.0, 1.0)
            GL.glClear(GL.GL_COLOR_BUFFER_BIT | GL.GL_DEPTH_BUFFER_BIT | GL.GL_STENCIL_BUFFER_BIT)
        else:
            # Initialize GL if the document is new
            if not self.app.d().GLInitialized():
                print "INITIALIZING"
                sz = self.size()
                self.app.d().SetupGL(sz.width(), sz.height())
            self.app.d().Render()

    def resizeGL(self, width, height):
        # Create a square base size of 10x10 and scale it to the new size maintaining aspect ratio.
        # This way it conforms with what I did in the C++. In any case, it need to be top/left aligned, so the mouse x,y would be correct for RenderUI::Pick().
        new_size = QtCore.QSize(10, 10)
        size = QtCore.QSize(width, height)
        #new_size.scale(size, QtCore.Qt.KeepAspectRatio)
        new_size.scale(size,  QtCore.Qt.IgnoreAspectRatio)
        self.resize(new_size)    
        if not self.app.hasDoc(): return
        self.app.d().UpdateSize(width, height)

    def mousePressEvent(self, event):
        if not self.app.hasDoc(): return
        button = None
        if ( event.button() in [QtCore.Qt.LeftButton, QtCore.Qt.MidButton, QtCore.Qt.RightButton] ):
            button = GLWidget.BUTTON_TO_INDEX[event.button()]
        if button != None:
            self.app.d().MouseButtonDown(button, event.x(), event.y(),
                    int(event.modifiers() & QtCore.Qt.SHIFT) != 0,
                    int(event.modifiers() & QtCore.Qt.CTRL) != 0)
            self.grabMouse()
            self.app.setStatusMessageFromDoc()
            self.updateGL()

    def mouseMoveEvent(self, event):
        if not self.app.hasDoc(): return
        if self.app.d().MouseMove(event.x(), event.y()):
            self.updateGL()
            
    def mouseReleaseEvent(self, event):
        if not self.app.hasDoc(): return
        button = None
        if (event.button() in [QtCore.Qt.LeftButton, QtCore.Qt.MidButton, QtCore.Qt.RightButton]):
            button = GLWidget.BUTTON_TO_INDEX[event.button()]
        if button != None:
            self.app.d().MouseButtonUp(button, event.x(), event.y())
            self.releaseMouse()            

    def wheelEvent(self, event):
        if not self.app.hasDoc(): return
        self.app.d().MouseWheel(event.delta(),
                int(event.modifiers() & QtCore.Qt.SHIFT) != 0,
                int(event.modifiers() & QtCore.Qt.CTRL) != 0)
        self.app.setStatusMessageFromDoc()
        self.updateGL()


