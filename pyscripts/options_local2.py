class ModelParameters(object):
    def __init__(self, Opt, i):
        self.filename_pre   = Opt.options[i][0]
        self.gradient_scale = Opt.options[i][1]

class Opt(object):
     # [ File prefix, gradient scale, qc max #iterations, rounding
    options2 = [
        [ 'bottle/bottle', 0.5 ], #  0
        [ 'fandisk/fandisk', 0.5 ], #  1
        [ 'lionhead/lionhead', 1.0 ], #  2
        [ 'dancer2/dancer2', 1.0 ], #3
        [ 'fish/fish', 0.2 ], #4
        [ 'pear/pear', 1.0 ], #5
        [ 'sphere/sphere', 1.0 ], #6        
        [ 'botijo/botijo', 1.0 ],  #7
        [ 'holes2/holes2',  0.5 ],  #8
        [ 'cone/cone',  1.0],  #9
    ]
    
    options = options2
    data_dir = 'F:/Test_data/reliable'
    
    #do_models = xrange(len(options))
    do_models = [0]
    
    #k_values = [x+1 for x in k_values]
    #k_values = [.9999, 0.9, 0.8, 0.7, 0.6, 0.5, 0.4, 0.3]
    #k_values =  [(r-1.)/(r+1.) for r in reversed([10.])]
    #k_values = [0.0]
    
    #k_values2 = [.01]    
    #k_values2 = [.1]
    
    # Absolute path of data directory (since the data directory changes on load)
    res_dir = 'F:/Test_data/reliable'
