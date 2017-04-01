# function's scope, which is mostly for convenience and needn't necessarily be
# the case.
# The global variables are: app, env

# TODO:
# - Need to load existing entries in forms from document.

import os, sys, traceback

if not hasattr(os.path, 'relpath'):
    def relpath(target, base=os.curdir):
        if not os.path.exists(target):
            raise OSError, 'Target does not exist: ' + target
        if not os.path.isdir(base):
            raise OSError, 'Base is not a directory or does not exist: '+ base
        base_list = (os.path.abspath(base)).split(os.sep)
        target_list = (os.path.abspath(target)).split(os.sep)
        for i in range(min(len(base_list), len(target_list))):
            if base_list[i] <> target_list[i]: break
        else:
            i += 1
        rel_list = [os.pardir] * (len(base_list)-i) + target_list[i:]
        return os.path.join(*rel_list)

    os.path.relpath = relpath
    
if sys.argv[0][-9:] != 'pypara.py':
    sys.exit()
    
from PyQt4 import QtCore, QtGui

from gl_widget import GLWidget
from OpenGL import GL

import math

env = None

Doc = Document

# Need to initialize this here so that the settings are available in the
# array definitions further down for qt_builder.
#G_ = GlobalSettings()
#G_.Initialize()
# don't load settings if a script is going to be invoked
if not options.ui_script_file:
    if isinstance(__file__,  unicode):
        temp = os.path.dirname(os.path.abspath(__file__)).encode("ascii")
    else:
        temp = os.path.dirname(os.path.abspath(__file__))
#    G_.LoadOptionsFromDirectory(temp)
   
###############################################################################
def _trans(str):
    return QtCore.QCoreApplication.translate('QtGui.QMainWindow', str)

import types
def _find(f, container):
    func = f if (type(f) == types.FunctionType) else (lambda x: x == f)
    for i in xrange(len(container)):
        if func(container[i]):
            return i
    return None
################################################################################

################################################################################
# Main window

class MainWindow(QtGui.QMainWindow):
    """Main application window"""

    def __init__(self, app, parent=None):
        super(MainWindow, self).__init__(parent)
        
        global options # defined in pyparalab.py
        self.setWindowTitle(_trans('PyPara [%s]' % (options.mode)))
        icon = QtGui.QIcon('pypara.png')
        app.setWindowIcon(icon)
        self.setWindowIcon(icon)
        
        # GL Widget in the center
        self.glWidget = GLWidget(app)
        fr = QtGui.QFrame()
        layout = QtGui.QVBoxLayout()
        layout.setMargin(0)
        #fr.setStyleSheet("border: 1px solid blue")
        #layout.setAlignment(QtCore.Qt.AlignTop)
        layout.addWidget(self.glWidget)
        fr.setLayout(layout)
        self.setCentralWidget(fr)
        
        self.setStatusBar(QtGui.QStatusBar())
        self.statusBar().showMessage('No file loaded.')
        self.permaBar = QtGui.QLabel()
        self.permaBar.setText('Info bar')
        self.permaBar.setFrameShape(QtGui.QFrame.Panel)
        self.permaBar.setFrameShadow(QtGui.QFrame.Sunken)
        self.statusBar().addPermanentWidget(self.permaBar)

        # Tab widget in a (resizeable/relocatable) DockWidget on the right
        self.operationsWidget = createTabbedSidebar(self)
        self.operationsWidget.setMinimumWidth(400)
        self.dockWidget = QtGui.QDockWidget(_trans('Tools & Options'), self)
        self.dockWidget.setAllowedAreas(QtCore.Qt.LeftDockWidgetArea|QtCore.Qt.RightDockWidgetArea)
        self.dockWidget.setWidget(self.operationsWidget)
        self.addDockWidget(QtCore.Qt.RightDockWidgetArea, self.dockWidget)      
        
    def setupRecentFileWidget(self, fileList):
        # remove all items from current group widget
        recentGroupLayout = env.widget('recent_files').layout()
        recentGroupLayout.setMargin(0)
        recentGroupLayout.setSpacing(0)
        while recentGroupLayout.count() > 0:
            child = recentGroupLayout.takeAt(0)
            child.widget().setParent(None) # This is *critical* for removing the widget properly

        global app

        # add all files into layout
        for i in range(len(fileList)):
            file = fileList[i]
            fileButton = QtGui.QPushButton(_trans('&%d. %s' % (i+1, os.path.basename(file))))
            fileButton.setFlat(True)
            fileButton.setToolTip(_trans(file))

            # Annoyingly, I have to do this to create a local file variable
            # (for continuation to work as expected)
            def createLambda(file):
                def myLambda():
                    if preAction('open'):
                        app.loadMeshFrom(file)
                    postAction('open')
                return myLambda

            QtCore.QObject.connect(fileButton,
                    QtCore.SIGNAL('clicked(bool)'), createLambda(file))
            recentGroupLayout.addWidget(fileButton)

        # invalidate QLayoutItems for proper display
        for i in range(recentGroupLayout.count()):
            recentGroupLayout.itemAt(i).invalidate()
        recentGroupLayout.invalidate()

    def setDoc(self, doc):
        glWidget.setDoc(doc)

    def resizeGL(self, w, h):
        gw, gh = self.glWidget.width(), self.glWidget.height()
        mw = mh = min(self.width(), self.height())
        self.resize(mw-gw+w, mh-gh+h)

    def updateGL(self):
        self.glWidget.updateGL()

    def setStatusMessage(self, message):
        self.statusBar().showMessage(_trans(message))

    def setPermaMessage(self, message):
        self.permaBar.setText(_trans(message))
        
################################################################################
# Application

class Application(QtGui.QApplication):
    RECENT_FILE_LIST_SIZE = 5

    def __init__(self, args, loadSettings):
        super(Application, self).__init__(args)

        self.doc = None    # no document loaded
        self.script = None # not running a script

        self.startDir = os.path.dirname(os.path.abspath(__file__))

         # Initialize and load the global settings
#        global G_
#        self.global_settings = G_
        
        # Load the default settings
        self.qt_settings = QtCore.QSettings('pypara.ini', QtCore.QSettings.IniFormat)

        # Initialize window
        self.mainWindow = MainWindow(self)
        self.mainWindow.showMaximized()
#        if 'minimize_on_start' in opt and options.ui_script_file:
#            self.mainWindow.showMinimized()
#        fileList = self.readRecentFileList()
#        self.mainWindow.setupRecentFileWidget(fileList)

    def deinitializeSettings(self):
        #self.global_settings.Deinitialize()
        self.qt_settings.sync() # save the Qt Settings  
        
    def readRecentFileList(self):
        size = self.qt_settings.beginReadArray('recent')
        fileList = []
        for i in range(size):
            self.qt_settings.setArrayIndex(i)
            fileList.append(str(self.qt_settings.value('file').toString()))
        self.qt_settings.endArray()
        return fileList

    def writeRecentFileList(self, fileList):
        self.qt_settings.beginWriteArray('recent')
        for i in range(len(fileList)):
            self.qt_settings.setArrayIndex(i)
            self.qt_settings.setValue('file', QtCore.QVariant(fileList[i]))
        self.qt_settings.endArray()
        self.qt_settings.sync() # save the Qt Settings

    def addFileToRecentList(self, filename):
        fileList = self.readRecentFileList()
        if filename in fileList:
            fileList.remove(filename)
        fileList.insert(0, filename)
        del fileList[Application.RECENT_FILE_LIST_SIZE:]
        self.writeRecentFileList(fileList)
        self.mainWindow.setupRecentFileWidget(fileList)        
        
    def hasDoc(self):
        return self.doc
        
    def d(self):
        return self.doc

    def getQtPath(self, qtKey):
        return str(self.qt_settings.value(qtKey, QtCore.QVariant('.')).toString())

    def setQtPathToParentOf(self, qtKey, filename):
        self.qt_settings.setValue(qtKey, QtCore.QVariant(os.path.dirname(os.path.abspath(filename))))

    def setStatusMessageFromDoc(self):
        if not self.hasDoc(): return
        self.mainWindow.setStatusMessage(str(self.d().StatusMessage()))
      
    def loadMesh(self):
        global options # defined in pypara.py        
        ChangeDirectory(self.getQtPath('recent/loaddirectory'))
        filename = getInputFilename('Open Mesh', '', 'Mesh Files (*.obj *.off)')
        if not filename: return False

        self.setQtPathToParentOf('recent/loaddirectory', filename)
        return self.loadMeshFrom(filename)
        
    def loadMeshFrom(self, filename, bMeshOnly = False):
        global options # defined in pypara.py
        filename = os.path.abspath(filename)
        self.closeMesh() # deallocate previous document
        ChangeDirectory(os.path.dirname(os.path.abspath(filename))) # set current path
        self.doc = Document()
        if not self.doc.LoadDocument(filename, bMeshOnly): # load the document
            self.doc = None
            self.mainWindow.setWindowTitle('[%s] No file loaded' % (options.mode))
            return False
        else:
            self.mainWindow.setWindowTitle('[%s] %s' % (options.mode, filename))
            self.addFileToRecentList(filename)
            return True
            
    def saveMesh(self):
        ChangeDirectory(self.getQtPath('recent/savedirectory'))
        filename = getOutputFilename('Save Mesh', '', 'Mesh Files (*.obj)')
        if not filename: return

        self.setQtPathToParentOf('recent/savedirectory', filename)
        self.saveMeshTo(filename, env.get('save_wrapped_ply'))

    def saveMeshTo(self, filename, save_wrapped_ply = 1):
        global options # defined in pyconf.py
        filename = os.path.abspath(filename)
        ChangeDirectory(os.path.dirname(os.path.abspath(filename)))   # set current path
        self.doc.SaveDocument(filename, save_wrapped_ply)       # save the document
        self.mainWindow.setWindowTitle('[%s] %s' % (options.mode, filename))
        self.addFileToRecentList(filename)

    def closeMesh(self):
        if self.hasDoc():
            self.doc.CloseDocument()   # deallocate meshes
            app.setStatusMessageFromDoc() 
            self.doc = None
            
    def runUIScript(self, scriptfile, ignorePause = False):
        scriptfile = os.path.abspath(scriptfile) # important since the directory will be changed
        chdirToPathOf(scriptfile) # Run from the script's directory
        print 'Running script \'' + scriptfile + '\'...'
        execfile(scriptfile)
        localDict = locals()

        # Before running script: change to GL context
        self.mainWindow.glWidget.makeCurrent()

        if ('script_OptionsString' not in localDict) or ('script_RunScript' not in localDict):
            message = 'ERROR: Script did not define \'script_OptionsString()\' and \'script_RunScript()\'.'
            print(message)
            self.mainWindow.setStatusMessage(message)
        else:
            #self.reloadSettings(script_OptionsString())
            #script_RunScript()
            # Cannot invoke directly as above. According to documentation the
            # local namespace is not reliably modified.
            # XXX XXX this doesn't clear the existing options in PETSc
            #self.reloadSettings(localDict['script_OptionsString']()) # set the options
            self.script = None
            try:
                self.script = localDict['script_RunScript']() # run the script (or create generator)
            except Exception, e:
                print('----------------------------------------')
                traceback.print_exc()
                print('----------------------------------------')
                self.doc.SetStatusMessage('ERROR: The script threw an exception. See the console for details.')
            import types
            if type(self.script) != types.GeneratorType:
                self.script = None
            else: # if the script is a generator, then run it
                MAX_LABEL = 35
                self.continueUIScript(ignorePause)

        # After running script: redraw GL and update status bar
        postAction(None) # needed only when script invoked on command line

    def continueUIScript(self, ignorePause = False):
        if not self.script:
            message = 'ERROR: No running script to continue.'
            if self.hasDoc():
                self.doc.SetStatusMessage(message)
            else:
                self.mainWindow.setStatusMessage(message)
        else:
            try:
                while True:
                    self.script.next()
                    if not ignorePause: break
            except StopIteration:
                self.stopUIScript()
            except Exception, e:
                self.stopUIScript()
                print('----------------------------------------')
                traceback.print_exc()
                print('----------------------------------------')
                message = 'ERROR: The script threw an exception. See the console for details.'
                if self.hasDoc():
                    self.doc.SetStatusMessage(message)
                else:
                    self.mainWindow.setStatusMessage(message)

    def stopUIScript(self):
        if not self.script:
            message = 'ERROR: No running script to stop.'
            if self.hasDoc():
                self.doc.SetStatusMessage(message)
            else:
                self.mainWindow.setStatusMessage(message)
        else:
            self.script = None
        
################################################################################
# Side bar
from qt_builder.ui_builder import *

LARGE_INT = 100000000
INT = (-LARGE_INT, LARGE_INT)
POS_INT = (1, LARGE_INT)
NEG_INT = (-LARGE_INT, -1)
NON_NEG_INT = (0, LARGE_INT)
NON_POS_INT = (-LARGE_INT, 0)

LARGE_REAL = 1.0e30
REAL2 = (-LARGE_REAL, LARGE_REAL, 15)
NON_NEG_REAL2 = (0.0, LARGE_REAL, 15)
NON_POS_REAL2 = (-LARGE_REAL, 0.0, 15)
NON_NEG_REAL3 = (0.0, LARGE_REAL, 15)
NON_POS_REAL3 = (-LARGE_REAL, 0.0, 15)

def preAction(widgetName):
    global app
    if (app.hasDoc() and app.d().HasMesh()) or (widgetName in
            ('exit', 'open', 'reload', 'resize', 'write_script',
                'script.write', 'script.run', 'script.continue', 'script.stop')):
        return True
    else:
        app.mainWindow.setStatusMessage('*** Load a mesh first ***')
        app.setStatusMessageFromDoc()
        return False

def postAction(widgetName):
    global app
    if app.hasDoc() and app.d().HasMesh():
        app.setStatusMessageFromDoc()
    app.mainWindow.updateGL()
    
def ChangeDirectory(dir):
  if os.path.exists(dir):
    os.chdir(dir)

# change directory to path of
def chdirToPathOf(filename):
    ChangeDirectory(os.path.dirname(os.path.abspath(filename)))

# filename = getInputFilename('Open Image', '/path/to/stuff', 'Image Files (*.png *.jpg *.bmp)');
def getInputFilename(title, path, fileTypes, changedir = False, mandatory = False):
    filename = None
    while filename == None or filename == '':
        filename = QtGui.QFileDialog.getOpenFileName(None, _trans(title), path, _trans(fileTypes));
        if not mandatory:
            break
    if filename == '' or filename == None:
        return None
    else:
        filename = str(filename)
        if changedir:
            ChangeDirectory(os.path.dirname(os.path.abspath(filename)))
        return filename

def getOutputFilename(title, path, fileTypes, changedir = False, mandatory = False):
    filename = None
    while filename == None or filename == '':
        filename = QtGui.QFileDialog.getSaveFileName(None, _trans(title), path, _trans(fileTypes));
        if not mandatory:
            break
    if filename == '' or filename == None:
        return None
    else:
        filename = str(filename)
        if changedir:
            ChangeDirectory(os.path.dirname(os.path.abspath(filename)))
        return filename

############################################################
# File tab

FILE_TAB = [ # file tab
    '&File',
    [ UI_GROUP_BOX, {'label': 'Mesh', 'collapsible': True},
        [ UI_BUTTON, {'label': 'Open', 'name': 'open', 'shortcut': 'Ctrl+o'}, lambda: app.loadMesh() ],
        [ UI_BUTTON, {'label': 'Save', 'shortcut': 'Ctrl+s'}, lambda: app.saveMesh() ],
        [ UI_BUTTON, {'label': 'Close [was Ctrl+w]'}, lambda: app.closeMesh() ],
        [ UI_GROUP_BOX, {'label': 'Recent Files', 'name': 'recent_files'} ],
    ]
]

############################################################
# Edit tab

VIEW_TAB = [#view tab
    '&View',
    [ UI_GROUP_BOX, {'label': 'Render mode'},
        [ UI_CHECK_BOX, {'label': 'Select mode'}, lambda f: app.d().SetFlag(Doc.F_MODE_SELECT , f) ],
        [ UI_CHECK_BOX, {'label': 'Animation mode'}, lambda f: app.d().SetFlag(Doc.F_MODE_ANIMATE , f) ], 
        [ UI_COMBO_BOX, {'label': 'Shader name: ', 'int_returned': True, 'selected': 0}, lambda i: app.d().SetShaderIndex(i),
                ['Null', 'Phong', 'Golden', 'Checkerboard', 'Cartoons', 'Contours']],
    ]
]

TABBED_SIDEBAR = [
    UI_TABS, {},
    FILE_TAB,
    VIEW_TAB
]

def createTabbedSidebar(parentWindow):
        global env
        (sidebar, env) = recursiveCreateWidgets(TABBED_SIDEBAR, parentWindow,
                preAction=preAction, postAction=postAction)
        return sidebar
        
################################################################################
# the main program

# don't load settings if a script is going to be invoked
app = Application(sys.argv, not options.ui_script_file)

# run UI script if any specified
if options.ui_script_file:    # Run script if supplied
    app.runUIScript(options.ui_script_file)
    
# continue running the application
retval = app.exec_()
app.deinitializeSettings()
sys.exit(retval)    
