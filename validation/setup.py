################################################################################
#
#  Copyright (C) 2012-2022 Jack Araz, Eric Conte & Benjamin Fuks
#  The MadAnalysis development team, email: <ma5team@iphc.cnrs.fr>
#
#  This file is part of MadAnalysis 5.
#  Official website: <https://github.com/MadAnalysis/madanalysis5>
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

from setuptools import setup

with open("README.md", "r", encoding="utf-8") as f:
    long_description = f.read()

with open("requirements.txt", "r") as f:
    requirements = f.read()
requirements = [x for x in requirements.split("\n") if x != ""]


setup(
    name="ma5_validation",
    version="0.0.1",
    description=("Validation module for MadAnalysis 5"),
    long_description=long_description,
    long_description_content_type="text/markdown",
    url="https://github.com/MadAnalysis/madanalysis5",
    author="J.Y. Araz, B. Fuks, E. Conte",
    author_email=("jack.araz@durham.ac.uk, fuks@lpthe.jussieu.fr, eric.conte@iphc.cnrs.fr"),
    license="GPL-3.0",
    package_dir={"": "src"},
    install_requires=requirements,
    python_requires=">=3.6",
    classifiers=[
        "Intended Audience :: Science/Research",
        "Operating System :: OS Independent",
        "Programming Language :: Python :: 3",
        "Topic :: Scientific/Engineering :: Physics",
    ],
)
