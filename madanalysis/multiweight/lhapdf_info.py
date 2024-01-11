"""This file includes the storage of LHAPDF data from madanalysis/input/LHAPDF.txt"""
from dataclasses import dataclass
from typing import Text


@dataclass
class PDF:
    """Stores information about PDF set"""

    pdfid: int
    name: Text
    nmembers: int
    # this class should be extended inthe future about information on
    # which method to be used for the pdfset i.e. Replicas or Eigenvectors

    def __iter__(self):
        yield from (self.pdfid + idx for idx in range(0, self.nmembers))


class LHAPDFInfo(dict):
    """
    Storage for LHAPDF information

    Args:
        file_path (``Text``): hard coded information abour pdfsets
            current file structure is comma separated 3 column
            pdfid,name,members
    """

    __slots__ = ["lhapdf"]

    def __init__(self, file_path: Text):
        lhapdf = {}
        with open(file_path, "r") as f:
            for line in f.readlines()[1:]:
                current = line.split(",")
                lhapdf.update(
                    {
                        int(current[0]): PDF(
                            pdfid=int(current[0]),
                            name=current[1],
                            nmembers=int(current[2].replace("\n", "")),
                        )
                    }
                )
        super().__init__(lhapdf)
