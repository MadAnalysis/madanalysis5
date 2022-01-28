import os
from typing import Text

from ma5_validation.system.exceptions import InvalidMadAnalysisPath


class PathHandler:
    LOGPATH = os.path.dirname(os.path.realpath(__file__))
    for _ in range(3):
        LOGPATH = os.path.split(LOGPATH)[0]
    LOGPATH = os.path.join(LOGPATH, "scripts", "log")
    if not os.path.isdir(LOGPATH):
        os.mkdir(LOGPATH)

    MA5PATH = os.path.dirname(os.path.realpath(__file__))
    for _ in range(4):
        MA5PATH = os.path.split(MA5PATH)[0]

    @staticmethod
    def set_ma5path(ma5path: Text):
        if not os.path.isdir(ma5_path):
            raise InvalidMadAnalysisPath(f"Invalid path: {ma5_path}")
        if not os.path.isdir(os.path.join(ma5_path, "tools/ReportGenerator/Services")):
            raise InvalidMadAnalysisPath(f"Invalid path: {ma5_path}")
        PathHandler.MA5PATH = os.path.normpath(ma5_path)

    @staticmethod
    def set_logpath(logpath: Text):
        if not os.path.isdir(logpath):
            os.mkdir(logpath)
        PathHandler.LOGPATH = logpath
