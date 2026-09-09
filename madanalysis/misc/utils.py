import logging
import os
import re
from pathlib import Path

log = logging.getLogger("MA5")


def get_runs(dirname: str) -> None:
    """Retreive analyses from recasting card"""
    del_runs = []
    ana_runs = []
    ## decoding the card
    card = Path(dirname).joinpath("Input/recasting_card.dat").absolute()
    with card.open("r", encoding="utf-8") as f:
        for line in f.readlines():
            if len(line.strip()) == 0 or line.strip().startswith("#"):
                continue
            myline = line.split()
            if myline[2].lower() == "on" and (myline[1], myline[3]) not in del_runs:
                del_runs.append((myline[1], myline[3]))
            if myline[2].lower() == "on":
                ana_runs.append((myline[1], myline[0]))
    ## saving the information and exti
    return del_runs, ana_runs


def edit_recasting_card(editor: str, dirname: str) -> None:
    """Prompt to edit the recasting card"""
    log.info("Would you like to edit the recasting Card ? (Y/N)")
    allowed_answers = ["n", "no", "nope", "y", "yes", "yeap"]
    answer = ""
    while answer not in allowed_answers:
        answer = input("Answer: ")
        answer = answer.lower()
    if answer not in ["no", "n", "nope"]:
        os.system(editor + " " + dirname + "/Input/recasting_card.dat")


def read_xsec(path: str) -> float:
    """Read cross section value from SAF file"""
    saf_file = Path(path)
    if not saf_file.exists():
        return 0.0
    with saf_file.open("r", encoding="utf-8") as f:
        smp_info = (
            [
                match.group(1)
                for match in re.finditer(r"<SampleGlobalInfo>(.*?)<", f.read(), re.S)
            ][0]
            .splitlines()[-1]
            .split()
        )
    return float(smp_info[0])


def clean_region_name(mystr):
    newstr = mystr.replace("/", "_slash_")
    newstr = newstr.replace("->", "_to_")
    newstr = newstr.replace(">=", "_greater_than_or_equal_to_")
    newstr = newstr.replace(">", "_greater_than_")
    newstr = newstr.replace("<=", "_smaller_than_or_equal_to_")
    newstr = newstr.replace("<", "_smaller_than_")
    newstr = newstr.replace(" ", "_")
    newstr = newstr.replace(",", "_")
    newstr = newstr.replace("+", "_")
    newstr = newstr.replace("-", "_")
    newstr = newstr.replace("(", "_lp_")
    newstr = newstr.replace(")", "_rp_")
    return newstr
