import os, sys

# Invoke pyconf.py (unless already within it)
if os.path.basename(sys.argv[0]) != 'pypara.py':
    import subprocess
    # invoke: python <path>/pyconf.py -s <this script> <this script's args>
    #pygui = os.path.realpath(os.path.join(os.environ['PARALAB_DIR'], 'py', 'pypara.py'))
    pygui = 'F:/ParaLab/py/pypara.py'
    args = sys.argv[1:]
    args = ['python', pygui, '-s', os.path.abspath(__file__) ] + args
    os.chdir(os.path.dirname(pygui))
    subprocess.call(args)
    sys.exit(0)
    
# Returns a PETSc options string for initialization.
def script_OptionsString():
    #matscriptdir = os.path.join(os.environ['PARALAB_DIR'], 'scripts', 'correct_metapoly')
    matscriptdir = 'F:/ParaLab/scripts'

# Script content.
# TO PAUSE: use yield statements
# VARIABLES/OBJECTS:
#   global: app, env  
#   Document instance: app.d()
def script_RunScript():
    global app, env
    import os, sys, inspect,  csv
    
    # Load options
    cmd_folder = os.path.realpath(os.path.abspath(os.path.split(inspect.getfile( inspect.currentframe() ))[0]))
    if cmd_folder not in sys.path:
        sys.path.insert(0, cmd_folder)
    try:
        from options_local2 import Opt,  ModelParameters
    except ImportError:
        from options import Opt,  ModelParameters
        
    #app.RunMatlabCommand('clear all;')        
        
    data_dir = Opt.data_dir
    do_models = Opt.do_models
    
    # Iterate models
    for i in do_models:
        model_parms = ModelParameters(Opt, i)

        filename_pre = model_parms.filename_pre
        filename_mesh = os.path.join(data_dir, filename_pre + '.off')
        filename_field = os.path.join(data_dir, filename_pre + '.ffield')
        
        #raw_input()
        
        # output model info
        if not app.loadMeshFrom(filename_mesh):
            print('### SCRIPT: ERROR loading file %s' % (repr(filename_mesh)))
            print('### SCRIPT PAUSED (load file manually and continue)')
            yield           

