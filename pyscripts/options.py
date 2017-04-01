class ModelParameters(object):
    def __init__(self, Opt, i):
        self.filename_pre   = Opt.options[i][0]
        self.gradient_scale = Opt.options[i][1]
        self.texture_scale  = Opt.options[i][2]
        self.distortion_max = 0.6

        # rendering params
        self.cull = True


class Opt(object):
    import os
    # Absolute path of data directory (since the data directory changes on load)
    #data_dir = '/Users/nicopietroni/Dropbox/TTraceModels/Tracing'
    #data_dir = '/Users/nicopietroni/Dropbox/TTraceModels/NewModels'#/inputmodels'
    #data_dir = '/Users/nicopietroni/Dropbox/TTraceModels/'#inputmodels'
    #data_dir = '/Users/nicopietroni/Dropbox/TTraceModels/miq_bommes/'
    #data_dir = '/Users/nicopietroni/Dropbox/TTraceModels/NewModels/miq_bommes'
    #data_dir = '/Users/nicopietroni/Desktop/inputmodels2'
    #data_dir = '/Users/nicopietroni/Desktop/MIQBommesCheck1'
    #data_dir = '/Users/nicopietroni/Desktop/ToSolve/DataOK/nonlinear/'
    data_dir = '/Users/nicopietroni/Desktop/ToSolve/'

    
    # [ File prefix, gradient scale ]
    options0 = [
                #['bimba200K' , 46.44885, 1 ],
                #['bimba50K' , 35.18224, 1 ],
                #['bozbezbozzel200K' , 1.87825, 0.5 ],
                #['bozbezbozzel50K' , 0.66359, 0.5 ],
                #['camel50K' , 72.21220, 1 ],
                #['camille_hand200K' , 0.36393, 1 ],
                #['camille_hand50K' , 0.31420, 1 ],
                #['carter200K' , 0.42858, 1 ],
                #['carter50K' , 0.34710, 17 ],
                #['chair200K' , 0.13097, 1 ],
                #['chair50K' , 0.08388, 0.26684 ],
                #['chinese_lion200K' , 0.58563, 2 ],
                #['chinese_lion50K' , 0.70699, 1 ],
                #['dragonstand_recon50k' , 1247.78325, 0.5 ],#40
                #['eros200K' , 0.40557, 1 ],
                #['eros50K' , 0.18868, 2 ],
                #['filigree50k' , 95.05382, 2 ],
                #['gargoyle50K' , 0.26500, 2 ],
                #['heptoroid200k' , 4.33277, 4 ],
                #['igea200k' , 380.86343, 2 ],
                #['kitten200K' , 0.38180, 4 ],
                #['kitten50K' , 0.41339, 2 ],
                #['knot200K' , 0.35422, 2 ],
                #['knot50K' , 0.12601, 4 ],
                #['lucy200k' , 0.33779, 0.25 ],
                #['magalie_hand200K' , 0.53263, 1 ],
                #['magalie_hand50K' , 0.19188, 2 ],
                #['mask50K' , 7.48540, 8 ],
                #['master_cylinder200K' , 16.24176, 4 ],
                #['master_cylinder50K' , 3.52319, 16 ],
                #['oil_pump200K' , 2.00333, 0.5 ],
                #['oil_pump50K' , 1.88990, 0.5 ],#100
                #['pulley200K' , 0.24594, 2 ],
                #['pulley50K' , 0.17413, 4 ],
                #['red_circular_box200K' , 0.55678, 2 ],
                #['red_circular_box50K' , 0.37616, 2 ],
                #['rolling_stage200K' , 5.01059, 8 ],#120
                #['rolling_stage50K' , 7.33138, 4 ],
                #['seahorse2_200K' , 0.53882, 0.5 ],
                #['seahorse2_50K' , 0.52244, 0.5 ],
                #['sediapatch1_200K' , 0.23413, 2 ],
                #['sediapatch1_50K' , 0.10089, 4 ],
                #['vase-lion50K' , 48.62853, 2 ],#140
                #['vase200K' , 118.22731, 1 ],
                #['vase50K' , 44.43522, 1 ],
                #['vh_skin50K' , 746.89035, 0.25 ],
                #['yeahright_200k' , 21.79235, 0.5 ],
                #['yeahright_300k' , 36.49062, 0.5 ],
                ['aircraft' , 83.03299, 1.0 ],
                ['armadillo' , 0.71724, 1.0 ],
                ['armchair' , 0.05346, 4 ],
                ['beetle_refined' , 9.88911, 1.0 ],
                ['bimba100K' , 40.28298, 1 ],
                ['blade' , 112.67628, 1 ],
                ['botijo' , 8.58220, 4 ],
                ['bozbezbozzel100K' , 1.28592, 0.5 ],
                ['bumpy_sphere' , 0.32751, 2 ],
                ['bumpy_torus' , 0.70431, 4 ],
                ['bunnyBotsch' , 93.79018, 4 ],#10
                ['buste' , 0.29294, 1 ],
                ['camel' , 183.98213, 0.5 ],
                ['camille_hand100K' , 0.29153, 1 ],
                ['carter100K' , 0.42414, 1 ],
                ['casting_refined' , 246.42681, 4 ],
                ['chair' , 0.13831, 1 ],
                ['chair100K' , 0.13831, 1 ],
                ['chinese_lion100K' , 0.39410, 2 ],
                ['cow2' , 35.97403, 2 ],
                ['cup' , 0.19068, 4 ],#20
                ['dancer2' , 0.26335, 2 ],
                ['dancer_25k' , 0.82725, 1 ],
                ['david' , 14.03623, 1 ],
                ['deformed_armadillo' , 1.10600, 1 ],
                ['dente' , 0.70574, 4 ],
                ['dilo' , 189.04121, 0.5 ],
                ['dragonstand_recon100K' , 591.02315, 1 ],
                ['duck' , 0.17207, 2 ],
                ['eight' , 4.13234, 8 ],
                ['elephant' , 44.69670, 2 ],#30
                ['elk' , 0.90219, 0.5 ],
                ['eros100K' , 0.19247, 2 ],
                ['face-YO' , 0.12084, 2 ],
                ['fandisk' , 2.06607, 4 ],
                ['femur' , 38.92495, 0.25 ],
                ['fertility_tri' , 0.15251, 4 ],
                ['filigree100k' , 320.41817, 1 ],
                ['fish' , 0.37000, 1 ],
                ['focal-octa' , 311.26963, 0.25 ],
                ['foot' , 7.36572, 4 ],#40
                ['gargoyle100K' , 1.14127, 1 ],
                ['genus3' , 20.64895, 4 ],
                ['hand' , 0.10284, 4 ],
                ['hand_olivier' , 15.28389, 4 ],
                ['helmet' , 8.74351, 8 ],
                ['heptoroid100k' , 4.63954, 4],
                ['holes3' , 0.02356, 8 ],
                ['homer' , 52.70617, 2 ],
                ['horse' , 73.71102, 1 ],
                ['igea100k' , 341.55763, 2 ],#50
                ['isidore_horse' , 0.36402, 0.5 ],
                ['joint' , 5.21515, 8 ],
                ['julius' , 59.58330, 2 ],
                ['kitten100K' , 0.44479, 2 ],
                ['knot1' , 2.77609, 16 ],
                ['knot100K' , 0.15410, 4 ],
                ['laurent_hand' , 0.24020, 2 ],
                ['laurent_hand50K' , 0.14507, 4 ],
                ['lion_recon_50K' , 241.01136, 2 ],
                ['lucy100k' , 0.15905, 1 ],#60
                ['magalie_hand100K' , 0.46699, 1 ],
                ['mannequin-devil' , 6.76656, 0.5 ],
                ['mask' , 3.47445, 8 ],
                ['master_cylinder100K' , 12.06927, 4 ],
                ['max-planck' , 0.06834, 4 ],
                ['moai' , 1.59510, 4 ],
                ['neptune0' , 0.28152, 1 ],
                ['nicolo_da_uzzano' , 0.36406, 32 ],
                ['oil_pump100K' , 1.88990, 0.5 ],
                ['oni' , 63.28596, 1 ],#70
                ['pear' , 1.32032, 2 ],
                ['pensatore' , 0.21036, 1 ],
                ['pig' , 20.52549, 4 ],
                ['plane-sphere' , 1.33141, 16 ],
                ['polygirl' , 315.21503, 0.5 ],
                ['pulley100K' , 0.23962, 2 ],
                ['ramses' , 0.16656, 1 ],
                ['raptor50K' , 397.86616, 0.25 ],
                ['red_circular_box100K' , 0.37019, 2 ],
                ['retinal' , 7.26850, 4 ],#80
                ['rgb_dragon' , 1.38095, 0.5 ],
                ['robocat_deci' , 3.74578, 0.5 ],
                ['rocker_arm' , 3.78462, 2 ],
                ['rolling_stage100K' , 8.26956, 4 ],
                ['santa' , 331.66126, 4 ],
                ['screwdriver' , 41.87548, 1 ],
                ['sculpt' , 11.44081, 4 ],
                ['seahorse2_100K' , 0.37648, 0.5 ],
                ['sediapatch1_100K' , 0.22200, 2 ],
                ['shark' , 805.15611, 0.125 ],#90
                ['sharp_sphere' , 0.36196, 4 ],
                ['smooth-feature' , 1.03482, 4 ],
                ['thai_statue' , 0.31428, 1 ],
                ['three_peaks' , 1.41910, 2 ],
                ['torso' , 112.10846, 1 ],
                ['trim-star' , 0.63960, 4 ],
                ['twirl' , 8.38045, 0.25 ],
                ['vase-lion100K' , 244.53211, 0.5 ],
                ['vase100K' , 97.62744, 1 ],
                ['venus' , 0.74252, 16 ],#100
                ['vh_skin' , 1258.95868, 0.125 ],
                ['woodenfish' , 0.38834, 0.5 ],
                ['wrench50K' , 0.29550, 2 ],
                #['yeahright_100k' , 20.61962, 0.5 ],
    ]
    
    optionsNew=[
                ['dancing_children100K',1,0.25],
                ['amphora' , 1, 1 ],
                ['block' , 0.88830, 8 ],
                ['brain100k' , 3.52495, 0.5 ],
                #['gearbox' , 1114.14683, 0.5 ],
                ['grayloc' , 2.42298, 0.25 ],
                ['greek_sculpture' , 0.26365, 2 ],
                ['mannequin_mc' , 0.08312, 2 ],
                ['mouse' , 0.50773, 2 ],
                ['pegaso' , 0.85233, 0.5 ],
                ['pierrot100k' , 0.77397, 1 ],
                ['uu-memento100k' , 0.71463, 1 ],
    ]
    
    optionsToSolve=[
                    ['shark' , 805.1561100, 0.125 ],
                    ['beetle_refined' , 9.88911, 1.0 ],
                    ['vh_skin' , 1258958.68, 0.125 ],
                    ['yeahright_200k1' , 20619620, 0.5 ],
                    ['yeahright_300k' , 20619.62, 0.5 ]
                ]
    
    optionsBommesCheck1=[
                        ['helmet' , 8.74351, 8 ],
                        ['twirl' , 8.38045, 0.25 ],
                        ['armchair' , 0.05346, 4 ],
                        ['carter100K' , 0.42414, 1 ],
                        ['deformed_armadillo' , 1.10600, 1 ],
                        ['filigree100k' , 320.41817, 1 ],
                        ['gargoyle100K' , 1.14127, 1 ],
                        ['hand_olivier' , 15.28389, 4 ],
                        ['heptoroid100k' , 4.63954, 4],
                        ['isidore_horse' , 0.36402, 0.5 ],
                        ['joint' , 5.21515, 8 ],
                        ['kitten100K' , 0.44479, 2 ],
                        ['knot1' , 2.77609, 16 ],
                        ['mannequin-devil' , 6.76656, 0.5 ],
                        ['nicolo_da_uzzano' , 0.36406, 32 ],
                        ['polygirl' , 315.21503, 0.5 ],#fino qua messi!
                        ['raptor50K' , 397.86616, 0.25 ],
                        ['screwdriver' , 41.87548, 1 ],
                        ['sculpt' , 11.44081, 4 ],
                        ['seahorse2_100K' , 0.37648, 0.5 ],
                        ['thai_statue' , 0.31428, 1 ],
                        ['vase-lion100K' , 244.53211, 0.5 ],
                        ['amphora' , 1, 1 ],
                        ['brain100k' , 3.52495, 0.5 ],
                        ['greek_sculpture' , 0.26365, 2 ],
                        ['pegaso' , 0.85233, 0.5 ],
                        ]
    
    optionsMIQCheck1=[
                        ['thai_statue' , 0.001, 1 ],
                        ['vase100K' , 1, 1 ],
                       ]
    
    optionsMIQCheck2=[
                      ['grayloc' , 0.002, 0.25 ],
                      ]
    
    optionsOURCheck2=[
                      ['amphora' , 1, 1 ],
                      ['grayloc' , 0.002, 0.25 ],
                      ]
    
    options = optionsToSolve

  

    #do_models = range(16,22)#len(options)) # run ALL
    do_models = range(3,4)#len(options))
    #do_models = range(0,len(options)) # run ALL
    
    
    # Generate EPS or PNG
    GENERATE_EPS=False

