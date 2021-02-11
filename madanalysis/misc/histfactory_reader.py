################################################################################
#  
#  Copyright (C) 2012-2020 Jack Araz, Eric Conte & Benjamin Fuks
#  The MadAnalysis development team, email: <ma5team@iphc.cnrs.fr>
#  
#  This file is part of MadAnalysis 5.
#  Official website: <https://launchpad.net/madanalysis5>
#  
#  MadAnalysis 5 is free software: you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation, either version 3 of the License, or
#  (at your option) any later version.
#  
#  MadAnalysis 5 is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
#  GNU General Public License for more details.
#  
#  You should have received a copy of the GNU General Public License
#  along with MadAnalysis 5. If not, see <http://www.gnu.org/licenses/>
#  
################################################################################


from __future__ import absolute_import
import json, os, copy, math, logging
from six.moves import range


class HistFactory(object):
    def __init__(self,pyhf_config):
        self.pyhf_config = pyhf_config.get('SR'  , {})
        self.lumi        = pyhf_config.get('lumi', 1.)
        self.path        = pyhf_config.get('path', 'missing_path')
        self.name        = pyhf_config.get('name', 'missing_name')
        self.logger      = logging.getLogger('MA5')
        if type(self) == HF_Background:
            self.hf             = {}
            self.global_config  = self.pyhf_config
        elif type(self) == HF_Signal:
            self.hf      = []

    def __call__(self,lumi):
        return self.extrapolate(lumi)

    @classmethod
    def __type__(self):
        return self.__name__

    def extrapolate(self,lumi):
        """ To calculate the HL variables HF needs to be extrapolated. Expected
            observables will be extrapolated and summed, summation is superseeded
            to the observed values since there is no observation in HL. 
            
            Modifiers are extrapolated with respect to their nature."""
        lumi = float(lumi)
        if lumi == self.lumi or self.hf in [{},[]]:
            return self.hf
        HF = copy.deepcopy(self.hf)
        lumi_scale = round(lumi/self.lumi, 6)

        if type(self) == HF_Background:
            # Background extrapolation
            total_expected = {}
            for SR, item in self.pyhf_config.items():
                if SR != 'lumi':
                    total_expected[SR] = [0.0]*len(item['data'])

            for iSR in range(len(HF['channels'])):
                self.logger.debug('  * Extrapolating channel '+ str(HF['channels'][iSR]['name']))
                if len(total_expected[HF['channels'][iSR]['name']]) == 0:
                    continue

                # modify the expected data of the sample
                for sample in range(len(HF['channels'][iSR]['samples'])):
                    self.logger.debug('    * Extrapolating '+str(HF['channels'][iSR]['samples'][sample]['name'])+ ' sample')
                    for i in range(len(HF['channels'][iSR]['samples'][sample]['data'])):
                        HF['channels'][iSR]['samples'][sample]['data'][i] *= lumi_scale
                        total_expected[HF['channels'][iSR]['name']][i] += HF['channels'][iSR]['samples'][sample]['data'][i]

                    for imod in range(len(HF['channels'][iSR]['samples'][sample]['modifiers'])):
                        mod_type = HF['channels'][iSR]['samples'][sample]['modifiers'][imod]['type']
                        if mod_type in ['normsys', 'normfactor', 'shapefactor', 'lumi']:
                            continue

                        # extrapolate shape variables
                        elif mod_type == 'shapesys':
                            for i in range(len(HF['channels'][iSR]['samples'][sample]['modifiers'][imod]['data'])):
                                HF['channels'][iSR]['samples'][sample]['modifiers'][imod]['data'][i] *= lumi_scale

                        # extrapolate histo variables
                        elif mod_type == 'histosys':
                            for i in range(len(HF['channels'][iSR]['samples'][sample]['modifiers'][imod]['data']['hi_data'])):
                                HF['channels'][iSR]['samples'][sample]['modifiers'][imod]['data']['hi_data'][i] *= lumi_scale
                            for i in range(len(HF['channels'][iSR]['samples'][sample]['modifiers'][imod]['data']['lo_data'])):
                                HF['channels'][iSR]['samples'][sample]['modifiers'][imod]['data']['lo_data'][i] *= lumi_scale

                        # extrapolate stat variables
                        elif mod_type == 'staterror':
                            for i in range(len(HF['channels'][iSR]['samples'][sample]['modifiers'][imod]['data'])):
                                HF['channels'][iSR]['samples'][sample]['modifiers'][imod]['data'][i] *= math.sqrt(lumi_scale)

            # replace the observed bkg with total expected bkg
            for key, item in total_expected.items():
                for iobs in range(len(HF['observations'])):
                    if key == HF['observations'][iobs]['name']:
                        if item != []:
                            HF['observations'][iobs]['data'] = item

        elif type(self) == HF_Signal:
            # Signal extrapolation
            for i in range(len(HF)):
                if HF[i]['op'] == 'remove':
                    continue
                HF[i]['value']['data'] = [round(x*lumi_scale,6) for x in HF[i]['value']['data']]
                # Extrapolate modifiers
                for imod in range(len(HF[i]['value']['modifiers'])):
                    mod_type = HF[i]['value']['modifiers'][imod]['type']
                    if mod_type in ['normsys', 'normfactor', 'shapefactor', 'lumi']:
                        continue

                    # extrapolate shape variables
                    elif mod_type == 'shapesys':
                        for i in range(len(HF[i]['value']['modifiers'][imod]['data'])):
                            HF[i]['value']['modifiers'][imod]['data'][i] *= lumi_scale

                    # extrapolate histo variables
                    elif mod_type == 'histosys':
                        for i in range(len(HF[i]['value']['modifiers'][imod]['data']['hi_data'])):
                            HF[i]['value']['modifiers'][imod]['data']['hi_data'][i] *= lumi_scale
                        for i in range(len(HF[i]['value']['modifiers'][imod]['data']['lo_data'])):
                            HF[i]['value']['modifiers'][imod]['data']['lo_data'][i] *= lumi_scale

                    # extrapolate statistical variables
                    elif mod_type == 'staterror':
                        for i in range(len(HF[i]['value']['modifiers'][imod]['data'])):
                            HF[i]['value']['modifiers'][imod]['data'][i] *= math.sqrt(lumi_scale)

        return HF


class HF_Background(HistFactory):
    def __init__(self, pyhf_config, expected=False):
        super(HF_Background, self).__init__(pyhf_config)
        self.logger.debug('Reading : '+os.path.join(self.path,self.name))
        if os.path.isfile(os.path.join(self.path,self.name)):
            with open(os.path.join(self.path,self.name),'r') as json_file:
                self.hf = json.load(json_file)
        else:
            self.logger.warning('Can not find file : '+ os.path.join(self.path,self.name))

        if expected:
            self.hf = self.impose_expected()

    def size(self):
        # The number of SRs in the likelihood profile
        return [len(x.get('data',[])) for x in self.get_observed()]

    def impose_expected(self):
        """
            To switch observed data with total expected data per SR bin.
        """
        total_expected = {}
        HF             = copy.deepcopy(self.hf)
        for i in range(len(HF.get('observations',[]))):
            total_expected[HF['observations'][i]['name']] = [0.0]*len(HF['observations'][i]['data'])

        for iSR in range(len(HF['channels'])):
            for sample in range(len(HF['channels'][iSR]['samples'])):
                for SRbin in range(len(HF['channels'][iSR]['samples'][sample]['data'])):
                    total_expected[HF['channels'][iSR]['name']][SRbin] += \
                        HF['channels'][iSR]['samples'][sample]['data'][SRbin]

        # replace the observed bkg with total expected bkg
        for key, item in total_expected.items():
            for iobs in range(len(HF['observations'])):
                if key == HF['observations'][iobs]['name']:
                    HF['observations'][iobs]['data'] = [round(x,5) for x in item]

        return HF

    def get_expected(self):
        return self.impose_expected().get('observations',[])

    def get_observed(self):
        return self.hf.get('observations',[])

    def get_sample_names(self):
        samples = {}
        HF      = copy.deepcopy(self.hf)
        for iSR in range(len(HF.get('channels',[]))):
            samples[HF['channels'][iSR]['name']] = []
            for sample in range(len(HF['channels'][iSR]['samples'])):
                samples[HF['channels'][iSR]['name']].append(HF['channels'][iSR]['samples'][sample]['name'])
        return samples






class HF_Signal(HistFactory):
    """
        HistFactory requires a jsonpathch file to be attached to the bkg. 
        BKG histfactory includes a configuration file which is necessary to
        construct the signal patch.
        
        **kwargs are for initialization of uncertainties in the future
        also background can be inputted for simultaneous validation of the profile.
        
        validate = True  will initiate a mock validation sequence to ensure that 
        the construction of pyhf_config is correct. The validation requires the 
        background sample to be completed. self.hf == [] means that validation
        is failed and correct pyhf_config is needed.
    """
    def __init__(self,pyhf_config, regiondata, xsection=-1, **kwargs):
        super(HF_Signal,self).__init__(pyhf_config)
        self.signal_config = {}
        for key, item in self.pyhf_config.items():
            if key != 'lumi':
                self.signal_config[key] = {'path' : '/channels/'+\
                                                    str(item['channels'])+'/samples/0'}
                if item['data'] == []:
                    self.signal_config[key]['op'] = 'remove'
                else:
                    self.signal_config[key]['op'] = 'add'
                self.signal_config[key]['data'] = []
                for SRname in item['data']:
                    if kwargs.get('validate',False):
                        # initiate mock validation sequence, this requires the
                        # background to be given in kwargs
                        self.signal_config[key]['data'].append(1.)
                    else:
                        self.signal_config[key]['data'].append(regiondata[SRname]['Nf']/regiondata[SRname]['N0'])

        self.hf = self.set_HF(xsection, background   = kwargs.get('background',  {}),
                                        add_normsys  = kwargs.get('add_normsys', []),
                                        add_histosys = kwargs.get('add_histosys',[]),)

    def set_HF(self,xsection,**kwargs):
        HF = []
        if xsection<=0.:
            return HF
        for ix, SR in enumerate(self.signal_config.keys()):
            SR_tmp = {'op'    : self.signal_config[SR]['op'],
                      'path'  : self.signal_config[SR]['path'],
                      'value' : {'name'      : 'MA5_signal_'+str(ix),
                                 'data'      : [round(eff*xsection*self.lumi*1000.,6) 
                                                 for eff in self.signal_config[SR]['data'] ],
                                 'modifiers' : [
                                                 {u'data': None, 
                                                  u'name': u'lumi', 
                                                  u'type': u'lumi'},
                                                 {u'data': None, 
                                                  u'name': u'mu_SIG', 
                                                  u'type': u'normfactor'}
                                                ]}
                      }
            if self.signal_config[SR]['op'] == 'remove':
                SR_tmp['value']['modifiers'] = []
            HF.append(SR_tmp)

        for sys in kwargs.get('add_normsys',[]):
            HF = self.add_normsys(HF,sys['hi'],sys['lo'],sys['name'])
        for sys in kwargs.get('add_histosys',[]):
            HF = self.add_normsys(HF,sys['hi_data'],sys['lo_data'],sys['name'])

        background = kwargs.get('background',{})
        if type(background) == HF_Background:
            if not self.validate_bins(background,HF):
                self.logger.warning('Signal HistFactory validation failed.')
                return []
        return HF

    def validate_bins(self,background,HF=[]):
        if HF == []:
            HF = self.hf
        bkg_bins    = background.size()
        to_validate = [False]*len(bkg_bins)
        if HF == {}:
            return all(to_validate)
        try:
            for sample in HF:
                # check if the size of the bins in the data matches the background
                if sample['op'] == 'remove':
                    to_validate[int(sample['path'].split('/')[2])] = True
                    continue
                elif len(sample['value']['data']) == bkg_bins[int(sample['path'].split('/')[2])]:
                    to_validate[int(sample['path'].split('/')[2])] = True
                # also check if the modifier data size matches with the background
                for modifier in sample['value']['modifiers']:
                    if modifier['type'] == 'histosys':
                        if len(modifier['data']['hi_data']) != bkg_bins[int(sample['path'].split('/')[2])]:
                            to_validate[int(sample['path'].split('/')[2])] = False
                        if len(modifier['data']['lo_data']) != bkg_bins[int(sample['path'].split('/')[2])]:
                            to_validate[int(sample['path'].split('/')[2])] = False
        except:
            self.logger.debug('Signal HistFactory : Key error in dictionary...')
            return False
        return all(to_validate)

    def isAlive(self):
        for sample in self.hf:
            if any([s>0 for s in sample['value']['data']]):
                return True
        return False

    def add_normsys(self,HF, hi, lo, name):
        # systematic unc: name has to be MA5_scale, MA5_PDF, MA5_TH or MA5_sys
        # hi = 1.XX lo = 0.XX
        for i in range(len(HF)):
            if HF[i]['op'] == 'remove':
                continue
            HF[i]['value']['modifiers'].append({ "name": name, 
                                                 "type": "normsys", 
                                                 "data": {"hi": hi, 
                                                          "lo": lo}
                                                })
        return HF

    def add_histosys(self,HF,hi_data,lo_data,name):
        # scale and TH uncertainties: name has to be MA5_scale, MA5_PDF, MA5_TH or MA5_sys
        # hi_data,lo_data are list!!
        for i in range(len(HF)):
            if HF[i]['op'] == 'remove':
                continue
            HF[i]['value']['modifiers'].append({ "name": name,
                                                  "type": "histosys", 
                                                  "data": {"hi_data": hi_data, 
                                                           "lo_data": lo_data}
                                                 })
        return HF

    def clear_modifiers(self):
        for i in range(len(self.hf)):
            self.hf[i]['value']['modifiers'] = [
                                                 {'data': None, 
                                                  'name': 'lumi', 
                                                  'type': 'lumi'},
                                                 {'data': None, 
                                                  'name': 'mu_SIG', 
                                                  'type': 'normfactor'}
                                                ]





def get_HFID(file,SRname):
    """
        Extract the location of the profiles within the JSON file.
    """
    if os.path.isfile(file):
        with open(file,'r') as json_file:
            HF = json.load(json_file)
    else:
        return 'Can not find background file: '+file
    for ch in HF['channels']:
        if ch['name'] == SRname:
            return HF['channels'].index(ch)
    return 'Invalid or corrupted info file.'




#def merge_backgrounds(Background1,Background2):
#    """
#        Merging method for two bakcground JSON file. It merges "only" the files 
#        with same POI and version.
#    """
#    if {} in [Background1.hf, Background2.hf] or type(Background1) != type(Background2):
#        return Background1, 0
#    logging.getLogger('MA5').debug('merging :'+', '.join(list(Background1.global_config.keys())+\
#                                                         list(Background2.pyhf_config.keys())))
#    measurements = []
#    # merge common measurements
#    for measurement1 in Background1.hf.get('measurements',[]):
#        poi   = measurement1['config']['poi']
#        param = measurement1['config']['parameters'] 
#        for measurement2 in Background2.hf.get('measurements',[]):
#            if poi == measurement2['config']['poi']:
#                for parameter in measurement2['config']['parameters'] :
#                    if parameter not in param:
#                        param += parameter
#                measurements.append({'name'   : measurement1['name'],
#                                     'config' : {'parameters' : param,
#                                                 'poi'        : poi
#                                                 }})
#
#    if measurements == [] or Background1.hf['version'] != Background2.hf['version'] :
#        logging.getLogger('MA5').warning('Merging failed: Either measurements or versions does not match...')
#        logging.getLogger('MA5').warning(', '.join(list(Background2.pyhf_config.keys()))+' will not be added.')
#        return Background1, 0 # only get uncontradctory poi
#
#    logging.getLogger('MA5').debug('measurements are matching...')
#    extend = len(Background1.get_observed())
#    for profile, info in Background2.pyhf_config.items():
#        check = [x for x in Background1.global_config.keys() if x.startswith(profile)]
#        profile += ''+(len(check)>0)*('_ma5_'+str(len(check)))
#        Background1.global_config[profile] = {}
#        Background1.global_config[profile]['channels'] = str(int(info['channels'])+extend)
#        #print str(int(info['channels'])+extend)
#        Background1.global_config[profile]['data'    ] = info['data']
#
#    for ch in Background2.hf['channels']:
#        Background1.hf['channels'].append(ch)
#    for obs in Background2.hf['observations']:
#        Background1.hf['observations'].append(obs)
#    Background1.hf['measurements'] = measurements
#    return Background1, 1

