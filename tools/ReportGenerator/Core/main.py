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


class Main():
    def __init__(self,version,date,mr5dir):
        self.version = version
        self.date    = date
        self.mr5dir  = mr5dir

    def Launch(self):

        import Services.XMLreader as XML
        xml = XML.XMLreader()
        xml.Init("histo_1.xml")
        test=xml.Read()
        if test:
            xml.Print()
        print "FIN"
        return
        

        # Inputs
        input_path = ""

        # Reading info from job output
        import Core.layout
        layout = Core.layout.Layout(self.main)
        if not self.extract(filename,layout):
            return

        print "END"
        return

        # Status = GOOD
        self.main.lastjob_status = True

        # Computing
        logging.getLogger('MA5').info("   Preparing data for the reports ...")
        layout.Initialize()

        # Creating the reports
        self.CreateReports(args,history,layout)

        # End time 
        end_time=time.time()
           
        logging.getLogger('MA5').info("   Well done! Elapsed time = " + CmdSubmit.chronometer_display(end_time-start_time) )



    # Generating the reports
    def CreateReports(self,args,history,layout):

        # Getting output filename for HTML report
        logging.getLogger('MA5').info("   Generating the HMTL report ...")
        htmlpath = os.path.expanduser(args[0]+'/HTML')
        if not htmlpath.startswith('/'):
            htmlpath = self.main.currentdir + "/" + htmlpath
        htmlpath = os.path.normpath(htmlpath)

        # Generating the HTML report
        layout.GenerateReport(history,htmlpath,ReportFormatType.HTML)
        logging.getLogger('MA5').info("     -> To open this HTML report, please type 'open'.")

        # PDF report
        if self.main.session_info.has_pdflatex:

            # Getting output filename for PDF report
            logging.getLogger('MA5').info("   Generating the PDF report ...")
            pdfpath = os.path.expanduser(args[0]+'/PDF')
            if not pdfpath.startswith('/'):
                pdfpath = self.main.currentdir + "/" + pdfpath
            pdfpath = os.path.normpath(pdfpath)

            # Generating the PDF report
            layout.GenerateReport(history,pdfpath,ReportFormatType.PDFLATEX)
            layout.CompileReport(ReportFormatType.PDFLATEX,pdfpath)

            # Displaying message for opening PDF
            if self.main.currentdir in pdfpath:
                pdfpath = pdfpath[len(self.main.currentdir):]
            if pdfpath[0]=='/':
                pdfpath=pdfpath[1:]
            logging.getLogger('MA5').info("     -> To open this PDF report, please type 'open " + pdfpath + "'.")
            
        else:
            logging.getLogger('MA5').warning("pdflatex not installed -> no PDF report.")

        # DVI/PDF report
        if self.main.session_info.has_latex:

            # Getting output filename for DVI report
            logging.getLogger('MA5').info("   Generating the DVI report ...")
            dvipath = os.path.expanduser(args[0]+'/DVI')
            if not dvipath.startswith('/'):
                dvipath = self.main.currentdir + "/" + dvipath
            dvipath = os.path.normpath(dvipath)

            # Warning message for DVI -> PDF
            if not self.main.session_info.has_dvipdf:
               logging.getLogger('MA5').warning("dvipdf not installed -> the DVI report will not be converted to a PDF file.")

            # Generating the DVI report
            layout.GenerateReport(history,dvipath,ReportFormatType.LATEX)
            layout.CompileReport(ReportFormatType.LATEX,dvipath)

            # Displaying message for opening DVI
            if self.main.session_info.has_dvipdf:
                pdfpath = os.path.expanduser(args[0]+'/DVI')
                if self.main.currentdir in pdfpath:
                    pdfpath = pdfpath[len(self.main.currentdir):]
                if pdfpath[0]=='/':
                    pdfpath=pdfpath[1:]
                logging.getLogger('MA5').info("     -> To open this PDF report, please type 'open " + pdfpath + "'.")
                
        else:
            logging.getLogger('MA5').warning("latex not installed -> no DVI/PDF report.")


    def extract(self,dirname,layout):
        logging.getLogger('MA5').info("   Checking SampleAnalyzer output...")
        import Core.job_reader
        jobber = Core.job_reader.JobReader(dirname)
        if not jobber.CheckDir():
            logging.getLogger('MA5').error("errors have occured during the analysis.")
            return False
        
        for item in self.main.datasets:
            if not jobber.CheckFile(item):
                logging.getLogger('MA5').error("errors have occured during the analysis.")
                return False

        logging.getLogger('MA5').info("   Extracting data from the output files...")
        for i in range(0,len(self.main.datasets)):
            jobber.Extract(self.main.datasets[i],\
                           layout.cutflow.detail[i],\
                           0,\
                           layout.plotflow.detail[i],\
                           domerging=False)
            if self.main.merging.enable:
                jobber.Extract(self.main.datasets[i],\
                               0,\
                               layout.merging.detail[i],\
                               0,\
                               domerging=True)
        return True    
