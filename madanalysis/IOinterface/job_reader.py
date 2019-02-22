################################################################################
#  
#  Copyright (C) 2012-2019 Eric Conte, Benjamin Fuks
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


from madanalysis.selection.instance_name      import InstanceName
from madanalysis.dataset.sample_info          import SampleInfo
from madanalysis.layout.cut_info              import CutInfo
from madanalysis.IOinterface.saf_block_status import SafBlockStatus
from madanalysis.layout.histogram             import Histogram
from madanalysis.layout.histogram_logx        import HistogramLogX
from madanalysis.layout.histogram_frequency   import HistogramFrequency
import glob
import logging
import shutil
import os
import commands
import copy


class JobReader():

    def __init__(self,jobdir):
        self.path       = jobdir
        self.safdir    = os.path.normpath(self.path+"/Output")

    def CheckDir(self):
        if not os.path.isdir(self.path):
            logging.getLogger('MA5').error("Directory called '"+self.path+"' is not found.")
            return False
        elif not os.path.isdir(self.safdir):
            logging.getLogger('MA5').error("Directory called '"+self.safdir+"' is not found.")
            return False
        else:
            return True

    def CheckFile(self,dataset):
        name=InstanceName.Get(dataset.name)
        if os.path.isfile(self.safdir+"/"+name+"/"+name+".saf"):
            return True
        else:
            logging.getLogger('MA5').error("File called '"+self.safdir+"/"+name+'/'+name+".saf' is not found.")
            return False


    def ExtractSampleInfo(self,words,numline,filename):

        # Creating container for info
        results = SampleInfo()

        # Extracting xsection
        try:
            results.xsection=float(words[0])
        except:
            logging.getLogger('MA5').error("xsection is not a float value:"+\
                          words[0])

        # Extracting xsection error
        try:
            results.xerror=float(words[1])
        except:
            logging.getLogger('MA5').error("xsection_error is not a float value:"+\
                          words[1])

        # Extracting number of events
        try:
            results.nevents=int(words[2])
        except:
            logging.getLogger('MA5').error("nevents is not an integer value:"+\
                          words[2])

        # Extracting sum positive weights
        try:
            results.sumw_positive=float(words[3])
        except:
            logging.getLogger('MA5').error("sum_weight+ is not a float value:"+\
                          words[3])

        # Extracting sum negative weights
        try:
            results.sumw_negative=float(words[4])
        except:
            logging.getLogger('MA5').error("sum_weight- is not a float value:"+\
                          words[4])

        return results


    def ExtractCutLine(self,words,numline,filename):

        # Extracting xsection
        try:
            a=float(words[0])
        except:
            logging.getLogger('MA5').error("Counter is not a float value:"+words[0])

        # Extracting xsection error
        try:
            b=float(words[1])
        except:
            logging.getLogger('MA5').error("Counter is not a float value:"+words[1])

        # Returning exracting values
        return [a,b]



    def ExtractDescription(self,words,numline,filename):

        # Extracting nbins
        try:
            a=int(words[0])
        except:
            logging.getLogger('MA5').error("nbin is not a int value:"+words[0])

        # Extracting xmin
        try:
            b=float(words[1])
        except:
            logging.getLogger('MA5').error("xmin is not a float value:"+words[1])

        # Extracting xmax
        try:
            c=float(words[2])
        except:
            logging.getLogger('MA5').error("xmax is not a float value:"+words[2])

        # Returning exracting values
        return [a,b,c]

    def ExtractStatisticsInt(self,words,numline,filename):

        # Extracting positive
        try:
            a=int(words[0])
        except:
            logging.getLogger('MA5').error(str(words[0])+' must be a positive integer value @ "'+filename+'" line='+str(numline))
            a=0
        if a<0:
            logging.getLogger('MA5').error(str(words[0])+' must be a positive integer value @ "'+filename+'" line='+str(numline))
            a=0

        # Extracting negative
        try:
            b=int(words[1])
        except:
            logging.getLogger('MA5').error(str(words[1])+' must be a positive integer value @ "'+filename+'" line='+str(numline))
            b=0
        if b<0:
            logging.getLogger('MA5').error(str(words[1])+' must be a positive integer value @ "'+filename+'" line='+str(numline))
            b=0

        # Returning exracting values
        return [a,b]


    def ExtractStatisticsFloat(self,words,numline,filename):

        # Extracting positive
        try:
            a=float(words[0])
        except:
            logging.getLogger('MA5').error(str(words[0])+' must be a float value @ "'+filename+'" line='+str(numline))
            a=0.

        # Extracting negative
        try:
            b=float(words[1])
        except:
            logging.getLogger('MA5').error(str(words[1])+' must be a float value @ "'+filename+'" line='+str(numline))
            b=0.

        # Returning exracting values
        return [a,b]


    def ExtractDataFreq(self,words,numline,filename):

        # Extracting label
        try:
            a=int(words[0])
        except:
            logging.getLogger('MA5').error(str(words[0])+' must be an integer value @ "'+filename+'" line='+str(numline))
            a=0.

        # Extracting positive
        try:
            b=float(words[1])
        except:
            logging.getLogger('MA5').error(str(words[1])+' must be a float value @ "'+filename+'" line='+str(numline))
            b=0.

        # Extracting negative
        try:
            c=float(words[2])
        except:
            logging.getLogger('MA5').error(str(words[2])+' must be a float value @ "'+filename+'" line='+str(numline))
            c=0.

        # Returning exracting values
        return [a,b,c]


    # Extracting data from the SAF file
    # sample & file info -> dataset
    # cut counters       -> initial & cut
    # merging plots      -> merging
    # selection plots    -> plot

    def ExtractGeneral(self,dataset):

        # Getting the output file name
        name=InstanceName.Get(dataset.name)
        filename = self.safdir+"/"+name+"/"+name+".saf"

        # Opening the file
        try:
            file = open(filename,'r')
        except:
            logging.getLogger('MA5').error("File called '"+filename+"' is not found")
            return

        # Initializing tags
        beginTag       = SafBlockStatus()
        endTag         = SafBlockStatus()
        globalTag      = SafBlockStatus()
        detailTag      = SafBlockStatus()

        # Loop over the lines
        numline=0
        for line in file:

            # Incrementing line counter
            numline+=1

            # Removing comments
            index=line.find('#')
            if index!=-1:
                line=line[:index]

            # Treating line
            line=line.lstrip()
            line=line.rstrip()
            words=line.split()
            if len(words)==0:
                continue

            # Looking for tag 'SampleGlobalInfo'
            if len(words)==1 and words[0][0]=='<' and words[0][-1]=='>':
                if words[0].lower()=='<safheader>':
                    beginTag.activate()
                elif words[0].lower()=='</safheader>':
                    beginTag.desactivate()
                if words[0].lower()=='<saffooter>':
                    endTag.activate()
                elif words[0].lower()=='</saffooter>':
                    endTag.desactivate()
                if words[0].lower()=='<sampleglobalinfo>':
                    globalTag.activate()
                elif words[0].lower()=='</sampleglobalinfo>':
                    globalTag.desactivate()
                elif words[0].lower()=='<sampledetailedinfo>':
                    detailTag.activate()
                elif words[0].lower()=='</sampledetailedinfo>':
                    detailTag.desactivate()

            # Looking for summary sample info
            elif globalTag.activated and len(words)==5:
                dataset.measured_global = self.ExtractSampleInfo(words,numline,filename)

            # Looking for detail sample info (one line for each file)
            elif detailTag.activated and len(words)==5:
                dataset.measured_detail.append(self.ExtractSampleInfo(words,numline,filename))

        # Information found ?
        if beginTag.Nactivated==0 or beginTag.activated:
            logging.getLogger('MA5').error("SAF header <SAFheader> and </SAFheader> is not found.")
        if endTag.Nactivated==0 or endTag.activated:
            logging.getLogger('MA5').error("SAF footer <SAFfooter> and </SAFfooter> is not found.")
        if globalTag.Nactivated==0 or globalTag.activated:
            logging.getLogger('MA5').error("Information corresponding to the block "+\
                          "<SampleGlobalInfo> is not found.")
            logging.getLogger('MA5').error("Information on the dataset '"+dataset.name+\
                          "' are not updated.")
        if detailTag.Nactivated==0 or globalTag.activated:
            logging.getLogger('MA5').error("Information corresponding to the block "+\
                          "<SampleDetailInfo> is not found.")
            logging.getLogger('MA5').error("Information on the dataset '"+dataset.name+\
                          "' are not updated.")

        # Closing the file
        file.close()

    def ExtractHistos(self,dataset,plot,merging=False):
        # Getting the output file name
        name=InstanceName.Get(dataset.name)
        i=0
        if merging:
            while(os.path.isdir(self.safdir+"/"+name+"/MergingPlots_"+str(i))):
                i+=1
            filename = self.safdir+"/"+name+"/MergingPlots_"+str(i-1)+"/Histograms/histos.saf"
        else:
            while(os.path.isdir(self.safdir+"/"+name+"/MadAnalysis5job_"+str(i))):
                i+=1
            filename = self.safdir+"/"+name+"/MadAnalysis5job_"+str(i-1)+"/Histograms/histos.saf"

        # Opening the file
        try:
            file = open(filename,'r')
        except:
            logging.getLogger('MA5').error("File called '"+filename+"' is not found")
            return

        # Initializing tags
        beginTag       = SafBlockStatus()
        endTag         = SafBlockStatus()
        histoTag       = SafBlockStatus()
        histoLogXTag   = SafBlockStatus()
        histoFreqTag   = SafBlockStatus()
        descriptionTag = SafBlockStatus()
        statisticsTag  = SafBlockStatus()
        dataTag        = SafBlockStatus()

        # Initializing temporary containers
        histoinfo     = Histogram() 
        histologxinfo = HistogramLogX() 
        histofreqinfo = HistogramFrequency()
        data_positive = []
        data_negative = []
        labels        = []

        # Loop over the lines
        numline=0
        for line in file:
            # Incrementing line counter
            numline+=1

            # Removing comments
            index=line.find('#')
            if index!=-1:
                line=line[:index]

            # Treating line
            line=line.lstrip()
            line=line.rstrip()
            words=line.split()
            if len(words)==0:
                continue

            # decoding the file
            if len(words)==1 and words[0][0]=='<' and words[0][-1]=='>':
                if words[0].lower()=='<safheader>':
                    beginTag.activate()
                elif words[0].lower()=='</safheader>':
                    beginTag.desactivate()
                elif words[0].lower()=='<saffooter>':
                    endTag.activate()
                elif words[0].lower()=='</saffooter>':
                    endTag.desactivate()
                elif words[0].lower()=='<description>':
                    descriptionTag.activate()
                elif words[0].lower()=='</description>':
                    descriptionTag.desactivate()
                elif words[0].lower()=='<statistics>':
                    statisticsTag.activate()
                elif words[0].lower()=='</statistics>':
                    statisticsTag.desactivate()
                elif words[0].lower()=='<data>':
                    dataTag.activate()
                elif words[0].lower()=='</data>':
                    dataTag.desactivate()
                elif words[0].lower()=='<histo>':
                    histoTag.activate()
                elif words[0].lower()=='</histo>':
                    histoTag.desactivate()
                    plot.histos.append(copy.copy(histoinfo))
                    plot.histos[-1].positive.array = data_positive[:]
                    plot.histos[-1].negative.array = data_negative[:]
                    histoinfo.Reset()
                    data_positive = []
                    data_negative = []
                elif words[0].lower()=='<histofrequency>':
                    histoFreqTag.activate()
                elif words[0].lower()=='</histofrequency>':
                    histoFreqTag.desactivate()
                    plot.histos.append(copy.copy(histofreqinfo))
                    plot.histos[-1].labels = labels[:]
                    plot.histos[-1].positive.array = data_positive[:]
                    plot.histos[-1].negative.array = data_negative[:]
                    histofreqinfo.Reset()
                    data_positive = []
                    data_negative = []
                    labels        = []
                elif words[0].lower()=='<histologx>':
                    histoLogXTag.activate()
                elif words[0].lower()=='</histologx>':
                    histoLogXTag.desactivate()
                    plot.histos.append(copy.copy(histologxinfo))
                    plot.histos[-1].positive.array = data_positive[:]
                    plot.histos[-1].negative.array = data_negative[:]
                    histologxinfo.Reset()
                    data_positive = []
                    data_negative = []

            # Looking from histogram description
            elif descriptionTag.activated:
                if descriptionTag.Nlines==0:
                    if len(line)>1 and line[0]=='"' and line[-1]=='"':
                        myname=line[1:-1]
                        if histoTag.activated:
                            histoinfo.name=myname
                        elif histoLogXTag.activated:
                            histologxinfo.name=myname
                        elif histoFreqTag.activated:
                            histofreqinfo.name=myname
                    else:
                        logging.getLogger('MA5').error('invalid name for histogram @ line=' + str(numline) +' : ')
                        logging.getLogger('MA5').error(str(line))
                elif descriptionTag.Nlines==1 and not histoFreqTag.activated and len(words)==3:
                    results = self.ExtractDescription(words,numline,filename)
                    if histoTag.activated:
                        histoinfo.nbins=results[0]
                        histoinfo.xmin=results[1]
                        histoinfo.xmax=results[2]
                    elif histoLogXTag.activated:
                        histologxinfo.nbins=results[0]
                        histologxinfo.xmin=results[1]
                        histologxinfo.xmax=results[2]
                    else:
                        logging.getLogger('MA5').error('invalid histogram description @ line=' + str(numline) +' : ')
                        logging.getLogger('MA5').error(str(line))
                elif descriptionTag.Nlines>=1:
                    if histoTag.activated and len(words)==1:
                        histoinfo.regions.append(words[0])
                    elif histoLogXTag.activated and len(words)==1:
                        histologxinfo.regions.append(words[0])
                    elif histoFreqTag.activated and len(words)==1:
                        histofreqinfo.regions.append(words[0])
                    else:
                        logging.getLogger('MA5').error('invalid region for a histogram @ line=' + str(numline) +' : ')
                        logging.getLogger('MA5').error(str(line))
                else:
                    logging.getLogger('MA5').warning('Extra line is found: '+line)
                descriptionTag.newline()

            # Looking from histogram statistics
            elif statisticsTag.activated and len(words)==2:
                if statisticsTag.Nlines==0:
                    results = self.ExtractStatisticsInt(words,numline,filename)
                    if histoTag.activated:
                        histoinfo.positive.nevents=results[0]
                        histoinfo.negative.nevents=results[1]
                    elif histoLogXTag.activated:
                        histologxinfo.positive.nevents=results[0]
                        histologxinfo.negative.nevents=results[1]
                    elif histoFreqTag.activated:
                        histofreqinfo.positive.nevents=results[0]
                        histofreqinfo.negative.nevents=results[1]

                elif statisticsTag.Nlines==1:
                    results = self.ExtractStatisticsFloat(words,numline,filename)
                    if histoTag.activated:
                        histoinfo.positive.sumwentries=results[0]
                        histoinfo.negative.sumwentries=results[1]
                    elif histoLogXTag.activated:
                        histologxinfo.positive.sumwentries=results[0]
                        histologxinfo.negative.sumwentries=results[1]
                    elif histoFreqTag.activated:
                        histofreqinfo.positive.sumwentries=results[0]
                        histofreqinfo.negative.sumwentries=results[1]

                elif statisticsTag.Nlines==2:
                    results = self.ExtractStatisticsInt(words,numline,filename)
                    if histoTag.activated:
                        histoinfo.positive.nentries=results[0]
                        histoinfo.negative.nentries=results[1]
                    elif histoLogXTag.activated:
                        histologxinfo.positive.nentries=results[0]
                        histologxinfo.negative.nentries=results[1]
                    elif histoFreqTag.activated:
                        histofreqinfo.positive.nentries=results[0]
                        histofreqinfo.negative.nentries=results[1]

                elif statisticsTag.Nlines==3:
                    results = self.ExtractStatisticsFloat(words,numline,filename)
                    if histoTag.activated:
                        histoinfo.positive.sumw=results[0]
                        histoinfo.negative.sumw=results[1]
                    elif histoLogXTag.activated:
                        histologxinfo.positive.sumw=results[0]
                        histologxinfo.negative.sumw=results[1]
                    elif histoFreqTag.activated:
                        histofreqinfo.positive.sumw=results[0]
                        histofreqinfo.negative.sumw=results[1]

                elif statisticsTag.Nlines==4 and not histoFreqTag.activated:
                    results = self.ExtractStatisticsFloat(words,numline,filename)
                    if histoTag.activated:
                        histoinfo.positive.sumw2=results[0]
                        histoinfo.negative.sumw2=results[1]
                    elif histoLogXTag.activated:
                        histologxinfo.positive.sumw2=results[0]
                        histologxinfo.negative.sumw2=results[1]

                elif statisticsTag.Nlines==5 and not histoFreqTag.activated:
                    results = self.ExtractStatisticsFloat(words,numline,filename)
                    if histoTag.activated:
                        histoinfo.positive.sumwx=results[0]
                        histoinfo.negative.sumwx=results[1]
                    elif histoLogXTag.activated:
                        histologxinfo.positive.sumwx=results[0]
                        histologxinfo.negative.sumwx=results[1]

                elif statisticsTag.Nlines==6 and not histoFreqTag.activated:
                    results = self.ExtractStatisticsFloat(words,numline,filename)
                    if histoTag.activated:
                        histoinfo.positive.sumw2x=results[0]
                        histoinfo.negative.sumw2x=results[1]
                    elif histoLogXTag.activated:
                        histologxinfo.positive.sumw2x=results[0]
                        histologxinfo.negative.sumw2x=results[1]

                else:
                    logging.getLogger('MA5').warning('Extra line is found: '+line)
                statisticsTag.newline()

            # Looking from histogram data [ histo and histoLogX ]
            elif dataTag.activated and len(words)==2 and (histoTag.activated or histoLogXTag.activated):
                results = self.ExtractStatisticsFloat(words,numline,filename)
                if dataTag.Nlines==0:
                    if histoTag.activated:
                        histoinfo.positive.underflow=results[0]
                        histoinfo.negative.underflow=results[1]
                    elif histoLogXTag.activated:
                        histologxinfo.positive.underflow=results[0]
                        histologxinfo.negative.underflow=results[1]
                elif dataTag.Nlines==(histoinfo.nbins+1):
                    if histoTag.activated:
                        histoinfo.positive.overflow=results[0]
                        histoinfo.negative.overflow=results[1]
                    elif histoLogXTag.activated:
                        histologxinfo.positive.overflow=results[0]
                        histologxinfo.negative.overflow=results[1]
                elif dataTag.Nlines>=1 and dataTag.Nlines<=histoinfo.nbins:
                    if histoTag.activated or histoLogXTag.activated:
                        data_positive.append(results[0])
                        data_negative.append(results[1])
                else:
                    logging.getLogger('MA5').warning('Extra line is found: '+line)
                dataTag.newline()

            # Looking from histogram data [ histoFreq ]
            elif dataTag.activated and len(words)==3 and histoFreqTag.activated:
                results = self.ExtractDataFreq(words,numline,filename)
                labels.append(results[0])
                data_positive.append(results[1])
                data_negative.append(results[2])
                dataTag.newline()    

        # Information found ?
        if beginTag.Nactivated==0 or beginTag.activated:
            logging.getLogger('MA5').error("histos.saf: SAF header <SAFheader> and </SAFheader> is not found.")
        if endTag.Nactivated==0 or endTag.activated:
            logging.getLogger('MA5').error("histos.saf: SAF footer <SAFfooter> and </SAFfooter> is not found.")

        # Closing the file
        file.close()

    def ExtractCuts(self,dataset,cut):
        # Getting the output file name
        name=InstanceName.Get(dataset.name)
        i=0
        while(os.path.isdir(self.safdir+"/"+name+"/MadAnalysis5job_"+str(i))):
            i+=1
        filenames = glob.glob(self.safdir+"/"+name+"/MadAnalysis5job_"+str(i-1)+"/Cutflows/*.saf")

        # Treating the files one by one
        for myfile in filenames:
            # Opening the file
            try:
                file = open(myfile,'r')
            except:
                logging.getLogger('MA5').error("File called '"+myfile+"' is not found")
                return

            # Initializing tags
            beginTag       = SafBlockStatus()
            endTag         = SafBlockStatus()
            initialTag     = SafBlockStatus()
            cutTag         = SafBlockStatus()
            cutinfo       = CutInfo()
            cutflow_for_region = []

            # Loop over the lines
            numline=0
            for line in file:

                # Incrementing line counter
                numline+=1

                # Removing comments
                index=line.find('#')
                if index!=-1:
                    line=line[:index]

                # Treating line
                line=line.lstrip()
                line=line.rstrip()
                words=line.split()
                if len(words)==0:
                    continue

                # Looking for tag 'SampleGlobalInfo'
                if len(words)==1 and words[0][0]=='<' and words[0][-1]=='>':
                    if words[0].lower()=='<safheader>':
                        beginTag.activate()
                    elif words[0].lower()=='</safheader>':
                        beginTag.desactivate()
                    elif words[0].lower()=='<saffooter>':
                        endTag.activate()
                    elif words[0].lower()=='</saffooter>':
                        endTag.desactivate()
                    elif words[0].lower()=='<initialcounter>':
                        initialTag.activate()
                    elif words[0].lower()=='</initialcounter>':
                        initialTag.desactivate()
                    elif words[0].lower()=='<counter>':
                        cutTag.activate()
                    elif words[0].lower()=='</counter>':
                        cutTag.desactivate()
                        cutinfo.cutregion = myfile.split('/')[-1].split('.')[:-1]
                        cutflow_for_region.append(copy.copy(cutinfo))
                        cutinfo.Reset()

                elif initialTag.activated and len(words)==2:
                    results = self.ExtractCutLine(words,numline,myfile)
                    if initialTag.Nlines==0:
                        cut.initial.nentries_pos = results[0]
                        cut.initial.nentries_neg = results[1]
                    elif initialTag.Nlines==1:
                        cut.initial.sumw_pos = results[0]
                        cut.initial.sumw_neg = results[1]
                    elif initialTag.Nlines==2:
                        cut.initial.sumw2_pos = results[0]
                        cut.initial.sumw2_neg = results[1]
                    else:
                        logging.getLogger('MA5').warning('Extra line is found: '+line)
                    initialTag.newline()

                # Looking for cut counter
                elif cutTag.activated and '"' in line:
                    if cutTag.Nlines==0:
                        cutinfo.cutname = line.strip()
                    else:
                        logging.getLogger('MA5').warning('Extra line is found: '+line)
                    cutTag.newline()

                elif cutTag.activated and len(words)==2:
                    results = self.ExtractCutLine(words,numline,myfile)
                    if cutTag.Nlines==1:
                        cutinfo.nentries_pos = results[0]
                        cutinfo.nentries_neg = results[1]
                    elif cutTag.Nlines==2:
                        cutinfo.sumw_pos = results[0]
                        cutinfo.sumw_neg = results[1]
                    elif cutTag.Nlines==3:
                        cutinfo.sumw2_pos = results[0]
                        cutinfo.sumw2_neg = results[1]
                    else:
                        logging.getLogger('MA5').warning('Extra line is found: '+line)
                    cutTag.newline()

            # Information found ?
            if beginTag.Nactivated==0 or beginTag.activated:
                logging.getLogger('MA5').error(myfile.split('/')[-1]+": SAF header <SAFheader> and </SAFheader> is not found.")
            if endTag.Nactivated==0 or endTag.activated:
                logging.getLogger('MA5').error(myfile.split('/')[-1]+": SAF footer <SAFfooter> and </SAFfooter> is not found.")

            # Closing the file
            file.close()
            cut.cuts.append(copy.copy(cutflow_for_region))
