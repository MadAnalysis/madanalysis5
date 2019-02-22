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


import logging
import shutil
import os
import commands
import copy


class PngHeader():

    def __init__(self):
        width            = 0
        height           = 0
        bit_depth        = 0
        color_type       = 0
        compression_type = 0
        filter_type      = 0
        interlace_type   = 0

        # - Width and height give the image dimensions in pixels. They are 4-byte
        #   integers. Zero is an invalid value. The maximum for each is (2^31)-1
        #   in order to accommodate languages which have difficulty with unsigned
        #   4-byte values.

        # - Bit depth is a single-byte integer giving the number of bits per
        #   pixel (for palette images) or per sample (for grayscale and truecolor
        #   images). Valid values are 1, 2, 4, 8, and 16, although not all values
        #   are allowed for all color types.

        # - Color type is a single-byte integer that describes the interpretation
        #   of the image data. Color type values represent  sums of the following
        #   values: 1 (palette used), 2 (color used), and 4 (full alpha used).
        #   Valid values are 0, 2, 3, 4, and 6.

        # - Compression type is a single-byte integer that indicates the method
        #   used to compress the image data. At present, only compression type 0
        #   (deflate/inflate compression with a 32K sliding window) is defined.

        # - Filter type is a single-byte integer that indicates the preprocessing
        #   method applied to the image data before compression. At present, only
        #   filter type 0 (adaptive filtering with five basic filter types) is
        #   defined. As with the compression type code, decoders must check this
        #   byte and report an error if it holds an unrecognized code.

        # - Interlace type is a single-byte integer that indicates the
        #   transmission order of the pixel data. Two values are currently
        #   defined: 0 (no interlace) or 1 (Adam7 interlace).
       

    def Print(self):
        print 'PNG file with width='+str(self.width)+\
              ' height='+str(self.height)+\
              ' bit_depth='+str(self.bit_depth)+\
              ' color_type='+str(self.color_type)+\
              ' compression_type='+str(self.compression_type)+\
              ' filter_type='+str(self.filter_type)+\
              ' interlace_type='+str(self.interlace_type)

        

class PngReader():

    png_header=[137,80,78,71,13,10,26,10]


    def __init__(self,filename):
        self.filename = filename
        self.header   = PngHeader()
        self.input    = 0


    def Open(self):
        try:
            self.input = open(self.filename,'rb')
        except:
            logging.getLogger('MA5').error("File called '"+self.filename+"' is not found.")
            return False
        return True


    def Close(self):
        try:
            self.input.close()
        except:
            logging.getLogger('MA5').error("Impossible to close the file called '"+self.filename+"'.")
            return False
        return True


    def IsValid(self):
        # Read PNG stamp
        try:
            head = self.input.read(8)
        except:
            logging.getLogger('MA5').error('The file "'+self.filename+'" seems to be empty.')
            return False
        if len(head)!=8:
            logging.getLogger('MA5').error('The file "'+self.filename+'" seems to be empty.')
            return False

        # Check the PNG stamp
        ok=True
        for ind in range(0,len(head)):
            if ord(head[ind])!=PngReader.png_header[ind]:
                ok=False
                break
        if not ok:
            logging.getLogger('MA5').error('The file "'+self.filename+'" is not a PNG file.')
            return False

        # Ok
        return True
        

    def ExtractHeader(self):

        # Begin of header
        try:
            head_length = self.input.read(4)
            head_type   = self.input.read(4)
        except:
            logging.getLogger('MA5').error('The file "'+self.filename+'" does not contain a PNG header.')
            return False

        # Check the header begin
        if head_type.upper()!='IHDR':
            logging.getLogger('MA5').error('The file "'+self.filename+'" does not contain a PNG header.')
            return False

        # Read the header
        try:
            width            = self.input.read(4)
            height           = self.input.read(4)
            bit_depth        = self.input.read(1)
            color_type       = self.input.read(1)
            compression_type = self.input.read(1)
            filter_type      = self.input.read(1)
            interlace_type   = self.input.read(1)
        except:
            logging.getLogger('MA5').error('Wrong PNG header for the file "'+self.filename+'".')
            return False

        # Decode the header
        self.header.bit_depth        = ord(bit_depth)
        self.header.color_type       = ord(color_type)
        self.header.compression_type = ord(compression_type)
        self.header.filter_type      = ord(filter_type)
        self.header.interlace_type   = ord(interlace_type)
        self.header.width            = 0
        self.header.height           = 0
        for ind in range(0,4):
            self.header.width  += ord(width[ind]) *((2**8)**(3-ind))
            self.header.height += ord(height[ind])*((2**8)**(3-ind))

        # Ok
        return True


