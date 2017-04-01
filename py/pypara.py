#!/usr/bin/env python

import os, sys

if __name__ != '__main__':
    sys.exit()

# Parse the options and decide what to do
from optparse import OptionParser
parser = OptionParser()
parser.add_option('-m', '--mode', dest='mode', default='Release',
        help='Debug, Release (or another directory within ../pyparalab) or test (default Release)')
parser.add_option('-s', '--ui-script', dest='ui_script_file', default=None,
        help='Script file that runs with the user interface')
(options, args) = parser.parse_args()

# Load pyparalab module
# test version
if options.mode == 'test':
    # Load the Document module
    from test_classes import *
    
# actual module
else:
    # IMPORTANT:
    # Load the OpenGL module BEFORE loading the pyparalab module to avoid
    # the strange segfault with std::cerr output.
    from OpenGL import GL, GLUT
    GLUT.glutInit('') # IMPORTANT: needed for rendering fonts within ParaLab

    module_directory = os.path.realpath(
            os.path.join(os.getcwd(), '..', options.mode))
    sys.path.append(module_directory)

    try:
        from pyparalab import *
    except ImportError, e:
        print 'Unable to load pyparalab module'
        print str(e)
        sys.exit(1)

# Additional paths to add to the module directory
script_helper_directory = os.path.realpath(os.path.join(os.getcwd(), '..', 'pyscripts', 'helpers'))
sys.path.append(script_helper_directory)

# run the gui
execfile('qt_gui.py')

