#!/usr/bin/env python3

import argparse, os


def main():
    pass


if __name__ == "__main__":

    parser = argparse.ArgumentParser(description="Validation module executor for MadAnalysis 5")

    paths = parser.add_argument_group("Path handling.")
    paths.add_argument(
        "--ma5-dir",
        type=str,
        dest="MA5DIR",
        default=os.path.split(os.path.dirname(os.path.realpath(__file__)))[0],
        help=f"MadAnalysis 5 directory path. "
        f"Default `{os.path.split(os.path.dirname(os.path.realpath( __file__ )))[0]}`",
    )

    package = parser.add_argument_group("Package handling.")
    package.add_argument(
        "-vp",
        "--validation-package",
        type=str,
        dest="PACKAGE",
        choices=[
            "normal",
            "expert",
            "delphes",
            "fastjet",
            "PAD",
            "PADForSFS",
            "PADForMA5Tune",
        ],
        required=True,
        help="Choose a validation package to execute.",
    )
