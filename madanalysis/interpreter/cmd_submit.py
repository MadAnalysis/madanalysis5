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


from madanalysis.interpreter.cmd_base                           import CmdBase
from madanalysis.IOinterface.job_writer                         import JobWriter
from madanalysis.IOinterface.layout_writer                      import LayoutWriter
from madanalysis.IOinterface.job_reader                         import JobReader
from madanalysis.IOinterface.folder_writer                      import FolderWriter
from madanalysis.enumeration.report_format_type                 import ReportFormatType
from madanalysis.layout.layout                                  import Layout
from madanalysis.install.install_manager                        import InstallManager
from madanalysis.install.detector_manager                       import DetectorManager
from madanalysis.misc.run_recast                                import RunRecast
from madanalysis.IOinterface.delphescard_checker                import DelphesCardChecker


from chronometer   import Chronometer
from string_tools  import StringTools
import logging
import glob
import os
import commands
import shutil

class CmdSubmit(CmdBase):
    """Command SUBMIT"""

    def __init__(self,main,resubmit=False):
        self.resubmit=resubmit
        if not resubmit:
            CmdBase.__init__(self,main,"submit")
        else:
            CmdBase.__init__(self,main,"resubmit")
        self.forbiddenpaths=[]
        self.forbiddenpaths.append(os.path.normpath(self.main.archi_info.ma5dir+'/lib'))
        self.forbiddenpaths.append(os.path.normpath(self.main.archi_info.ma5dir+'/bin'))
        self.forbiddenpaths.append(os.path.normpath(self.main.archi_info.ma5dir+'/madanalysis'))


    def do(self,args,history):
        if not self.resubmit:
            return self.do_submit(args,history)
        else:
            return self.do_resubmit(args,history)


    def do_resubmit(self,args,history):

        # Start time
        chrono = Chronometer()
        chrono.Start()

        # Checking argument number
        if len(args)!=0:
            self.logger.warning("Command 'resubmit' takes no argument. Any argument will be skipped.")

        # Checking presence of a valid job
        if self.main.lastjob_name is "":
            self.logger.error("an analysis must be defined and ran before using the resubmit command.") 
            return False

        self.main.lastjob_status = False

        # Checking if new plots or cuts have been performed
        ToReAnalyze = False

        # Look for the last submit and resubmit
        last_submit_cmd = -1
        for i in range(len(history)-1): # Last history entry should be resubmit
            if history[i].startswith('submit') or history[i].startswith('resubmit'):
                last_submit_cmd = i

        newhistory = []
        if last_submit_cmd==-1:
            ToReAnalyze = True
        else:
            for i in range(last_submit_cmd+1,len(history)):
                newhistory.append(history[i])

        ReAnalyzeCmdList = ['plot','select','reject','set main.clustering',
                            'set main.merging', 'define', 'set main.recast',
                            'import', 'set main.isolation']

        # Determining if we have to resubmit the job
        for cmd in newhistory:

            # Split cmd line into words
            words = cmd.split()

            # Creating a line with one whitespace between each word
            cmd2 = ''
            for word in words:
                if word!='':
                    cmd2+=word+' '

            # Looping over patterns
            for pattern in ReAnalyzeCmdList:
                if cmd2.startswith(pattern): 
                    ToReAnalyze = True
                    break

            # Found?
            if ToReAnalyze:
                break

        if ToReAnalyze:
            self.logger.info("   Creating the new histograms and/or applying the new cuts...")
            # Submission
            if not self.submit(self.main.lastjob_name,history):
                return
            self.logger.info("   Updating the reports...")
        else:
            self.logger.info("   No new histogram / cut to account for. Updating the reports...")

        # Reading info from job output
        layout = Layout(self.main)
        if not self.extract(self.main.lastjob_name,layout):
            return

        # Status = GOOD
        self.main.lastjob_status = True
        # Computing
        layout.Initialize()

        # Creating the reports
        self.CreateReports([self.main.lastjob_name],history,layout)

        # End of time 
        chrono.Stop()

        self.logger.info("   Well done! Elapsed time = "+chrono.Display())

    def do_submit(self,args,history):

        # Start time
        chrono = Chronometer()
        chrono.Start()

        # No arguments
        if len(args)==0:
            dirlist = os.listdir(self.main.currentdir)
            ii = 0
            while ('ANALYSIS_'+str(ii) in dirlist):
                ii = ii+1
            args.append(self.main.currentdir+'/ANALYSIS_'+str(ii))

        # Checking argument number
        if len(args)>1:
             self.logger.error("wrong number of arguments for the command 'submit'.")
             self.help()
             return

        # Checking if a dataset has been defined
        if len(self.main.datasets)==0:
            self.logger.error("no dataset found; please define a dataset (via the command import).")
            self.logger.error("job submission aborted.")
            return

        # Treat the filename
        filename = os.path.expanduser(args[0])
        if not filename.startswith('/'):
            filename = self.main.currentdir + "/" + filename
        filename = os.path.normpath(filename)

        # Checking folder
        if filename in self.forbiddenpaths:
            self.logger.error("the folder '"+filename+"' is a MadAnalysis folder. " + \
                         "You cannot overwrite it. Please choose another folder.")
            return

        # Saving job name as global variable
        self.main.lastjob_name = filename
        self.main.lastjob_status = False

        # Submission
        self.logger.debug('Launching SampleAnalyzer ...')
        if not self.submit(filename,history):
            return

        # Reading info from job output
        self.logger.debug('Go back to the Python interface ...')
        layout = Layout(self.main)
        if not self.extract(filename,layout):
            return

        # Status = GOOD
        self.main.lastjob_status = True

        # Computing
        self.logger.info("   Preparing data for the reports ...")
        layout.Initialize()

        # Creating the reports
        if not self.main.recasting.status=="on":
            self.CreateReports(args,history,layout)

        # End of time 
        chrono.Stop()

        self.logger.info("   Well done! Elapsed time = "+chrono.Display())


    # Generating the reports
    def CreateReports(self,args,history,layout):

        output_paths = []
        modes        = []

        # Getting output filename for histo folder
        i=0
        while(os.path.isdir(args[0]+"/Output/Histos/MadAnalysis5job_"+str(i))):
            i+=1

        histopath = os.path.expanduser(args[0]+'/Output/Histos/MadAnalysis5job_'+str(i))
        if not histopath.startswith('/'):
            histopath = self.main.currentdir + "/" + histopath
        histopath = os.path.normpath(histopath)

        # Getting output filename for HTML report
        htmlpath = os.path.expanduser(args[0]+'/Output/HTML/MadAnalysis5job_'+str(i))
        if not htmlpath.startswith('/'):
            htmlpath = self.main.currentdir + "/" + htmlpath
        htmlpath = os.path.normpath(htmlpath)
        output_paths.append(htmlpath)
        modes.append(ReportFormatType.HTML)

        # Getting output filename for PDF report
        if self.main.session_info.has_pdflatex:
            pdfpath = os.path.expanduser(args[0]+'/Output/PDF/MadAnalysis5job_'+str(i))
            if not pdfpath.startswith('/'):
                pdfpath = self.main.currentdir + "/" + pdfpath
            pdfpath = os.path.normpath(pdfpath)
            output_paths.append(pdfpath)
            modes.append(ReportFormatType.PDFLATEX)

        # Getting output filename for DVI report
        if self.main.session_info.has_latex:
            dvipath = os.path.expanduser(args[0]+'/Output/DVI/MadAnalysis5job_'+str(i))
            if not dvipath.startswith('/'):
                dvipath = self.main.currentdir + "/" + dvipath
            dvipath = os.path.normpath(dvipath)
            output_paths.append(dvipath)
            modes.append(ReportFormatType.LATEX)

        # Creating folders
        if not layout.CreateFolders(histopath,output_paths,modes):
            return

        # Draw plots
        self.logger.info("   Generating all plots ...")
        if not layout.DoPlots(histopath,modes,output_paths):
            return

        # Generating the HTML report
        self.logger.info("   Generating the HMTL report ...")
        layout.GenerateReport(history,htmlpath,ReportFormatType.HTML)
        self.logger.info("     -> To open this HTML report, please type 'open'.")

        # PDF report
        if self.main.session_info.has_pdflatex:

            # Generating the PDF report
            self.logger.info("   Generating the PDF report ...")
            layout.GenerateReport(history,pdfpath,ReportFormatType.PDFLATEX)
            layout.CompileReport(ReportFormatType.PDFLATEX,pdfpath)

            # Displaying message for opening PDF
            if self.main.currentdir in pdfpath:
                pdfpath = pdfpath[len(self.main.currentdir):]
            if pdfpath[0]=='/':
                pdfpath=pdfpath[1:]
            self.logger.info("     -> To open this PDF report, please type 'open " + pdfpath + "'.")

        else:
            self.logger.warning("pdflatex not installed -> no PDF report.")

        # DVI/PDF report
        if self.main.session_info.has_latex:

            # Warning message for DVI -> PDF
            self.logger.info("   Generating the DVI report ...")
#            if not self.main.session_info.has_dvipdf:
#               self.logger.warning("dvipdf not installed -> the DVI report will not be converted to a PDF file.")

            # Generating the DVI report
            layout.GenerateReport(history,dvipath,ReportFormatType.LATEX)
            layout.CompileReport(ReportFormatType.LATEX,dvipath)

            # Displaying message for opening DVI
            if self.main.session_info.has_dvipdf:
                pdfpath = os.path.expanduser(args[0]+'/Output/DVI/MadAnalysis5job_'+str(i))
                if self.main.currentdir in pdfpath:
                    pdfpath = pdfpath[len(self.main.currentdir):]
                if pdfpath[0]=='/':
                    pdfpath=pdfpath[1:]
                self.logger.info("     -> To open the corresponding Latex file, please type 'open " + pdfpath + "'.")

        else:
            self.logger.warning("latex not installed -> no DVI/PDF report.")



    def submit(self,dirname,history):

        # checking if delphes is needed and installing/activating it if relevant
        detector_handler = DetectorManager(self.main)
        if not detector_handler.manage('delphes'):
            logging.getLogger('MA5').error('Problem with the handling of delphes/delphesMA5tune')
            return False
        if not detector_handler.manage('delphesMA5tune'):
            logging.getLogger('MA5').error('Problem with the handling of delphes/delphesMA5tune')
            return False

        # Initializing the JobWriter
        jobber = JobWriter(self.main,dirname,self.resubmit)

        # Writing process
        if not self.resubmit:
            self.logger.info("   Creating folder '"+dirname.split('/')[-1] \
                +"'...")
        else:
            self.logger.info("   Checking the structure of the folder '"+\
               dirname.split('/')[-1]+"'...")
        if not jobber.Open():
            self.logger.error("job submission aborted.")
            return False

        if not self.resubmit:
            if self.main.recasting.status != 'on':
                self.logger.info("   Copying 'SampleAnalyzer' source files...")
            if not jobber.CopyLHEAnalysis():
                self.logger.error("   job submission aborted.")
                return False
            if self.main.recasting.status != 'on' and not jobber.CreateBldDir():
                self.logger.error("   job submission aborted.")
                return False

        # In the case of recasting, there is no need to create a standard Job
        if self.main.recasting.status == "on":
            Recaster = RunRecast(self.main, dirname)
            ### Initialization
            if not Recaster.init():
                return False
            self.main.recasting.delphesruns = Recaster.delphes_runcard
            ### fastsim
            if not Recaster.fastsim():
                self.logger.error("job submission aborted.")
                return False
            ### Analyses
            if not Recaster.analysis():
                self.logger.error("job submission aborted.")
                return False
        # Otherwise, standard job
        else:
            self.logger.info("   Inserting your selection into 'SampleAnalyzer'...")
            if not jobber.WriteSelectionHeader(self.main):
                self.logger.error("job submission aborted.")
                return False
            if not jobber.WriteSelectionSource(self.main):
                self.logger.error("job submission aborted.")
                return False

        self.logger.info("   Writing the list of datasets...")
        for item in self.main.datasets:
            jobber.WriteDatasetList(item)

        self.logger.info("   Writing the command line history...")
        jobber.WriteHistory(history,self.main.firstdir)
        if self.main.recasting.status == "on":
            self.main.recasting.collect_outputs(dirname,self.main.datasets)
            self.logger.info('    -> the results can be found in:') 
            self.logger.info('       '+ dirname + '/Output/CLs_output_summary.dat')
            for item in self.main.datasets:
                self.logger.info('       '+ dirname + '/Output/'+ item.name + '/CLs_output.dat')
        else:
            layouter = LayoutWriter(self.main, dirname)
            layouter.WriteLayoutConfig()

        if not self.main.recasting.status=='on' and not self.resubmit:
            self.logger.info("   Creating Makefiles...")
            if not jobber.WriteMakefiles():
                self.logger.error("job submission aborted.")
                return False

        # Edit & check the delphes or recasting cards
        if self.main.fastsim.package in ["delphes","delphesMA5tune"] and not self.main.recasting.status=='on':
            delphesCheck = DelphesCardChecker(dirname,self.main)
            if not delphesCheck.checkPresenceCard():
                self.logger.error("job submission aborted.")
                return False
            if not delphesCheck.editCard():
                self.logger.error("job submission aborted.")
                return False
            self.logger.info("   Checking the content of the Delphes card...")
            if not delphesCheck.checkContentCard():
                self.logger.error("job submission aborted.")
                return False

        if self.resubmit and not self.main.recasting.status=='on':
            self.logger.info("   Cleaning 'SampleAnalyzer'...")
            if not jobber.MrproperJob():
                self.logger.error("job submission aborted.")
                return False

        if not self.main.recasting.status=='on':
            self.logger.info("   Compiling 'SampleAnalyzer'...")
            if not jobber.CompileJob():
                self.logger.error("job submission aborted.")
                return False

            self.logger.info("   Linking 'SampleAnalyzer'...")
            if not jobber.LinkJob():
                self.logger.error("job submission aborted.")
                return False

            for item in self.main.datasets:
                self.logger.info("   Running 'SampleAnalyzer' over dataset '"
                             +item.name+"'...")
                self.logger.info("    *******************************************************")
                if not jobber.RunJob(item):
                    self.logger.error("run over '"+item.name+"' aborted.")
                self.logger.info("    *******************************************************")
        return True


    def extract(self,dirname,layout):
        self.logger.info("   Checking SampleAnalyzer output...")
        jobber = JobReader(dirname)
        if not jobber.CheckDir():
            self.logger.error("errors have occured during the analysis.")
            return False

        for item in self.main.datasets:
            if self.main.recasting.status=='on':
                if not self.main.recasting.CheckFile(dirname,item):
                    return False
            elif not jobber.CheckFile(item):
                self.logger.error("errors have occured during the analysis.")
                return False

        if self.main.recasting.status!='on':
            self.logger.info("   Extracting data from the output files...")
            for i in range(0,len(self.main.datasets)):
                jobber.ExtractGeneral(self.main.datasets[i])
                jobber.ExtractHistos(self.main.datasets[i],layout.plotflow.detail[i])
                jobber.ExtractCuts(self.main.datasets[i],layout.cutflow.detail[i])
                if self.main.merging.enable:
                    jobber.ExtractHistos(self.main.datasets[i],layout.merging.detail[i],merging=True)
        return True


    def help(self):
        if not self.resubmit:
            self.logger.info("   Syntax: submit <dirname>")
            self.logger.info("   Performs an analysis over a list of datasets. Output is stored into the directory <dirname>.")
            self.logger.info("   If the optional argument is omitted, MadAnalysis creates a fresh directory automatically.")
            self.logger.info("   HTML and PDF reports are automatically created.")
        else:
            self.logger.info("   Syntax: resubmit")
            self.logger.info("   Update of an analysis already performed, if relevant.")
            self.logger.info("   In all cases, the HTML and PDF reports are regenerated.")


    def complete(self,text,line,begidx,endidx):

        #Resubmission case 
        if self.resubmit:
            return 

        #Getting back arguments
        args = line.split()
        nargs = len(args)
        if not text:
            nargs += 1

        #Checking number of arguments
        if nargs==2:
            output=[]
            for file in glob.glob(text+"*"):
                if os.path.isdir(file):
                    output.append(file)
            return self.finalize_complete(text,output)
        else:
            return []
