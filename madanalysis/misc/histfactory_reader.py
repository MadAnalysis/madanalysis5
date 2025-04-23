################################################################################
#
#  Copyright (C) 2012-2023 Jack Araz, Eric Conte & Benjamin Fuks
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

import copy
import json
import logging
import math
import os
import sys
from collections import OrderedDict
from pathlib import Path
from typing import Tuple

try:
    from spey_pyhf.helper_functions import WorkspaceInterpreter
except ImportError:
    WorkspaceInterpreter = None


class HistFactory:
    def __init__(self, pyhf_config):
        self.pyhf_config = pyhf_config.get("SR", {})
        self.lumi = pyhf_config.get("lumi", 1.0)
        self.path = Path(pyhf_config.get("path", "missing_path"))
        self.name = pyhf_config.get("name", "missing_name")
        self.logger = logging.getLogger("MA5")
        if isinstance(self, HF_Background):
            self.hf = {}
            self.global_config = self.pyhf_config
        elif isinstance(self, HF_Signal):
            self.hf = []

    def __call__(self, lumi):
        return self.extrapolate(lumi)

    def extrapolate(self, lumi):
        """To calculate the HL variables HF needs to be extrapolated. Expected
        observables will be extrapolated and summed, summation is superseeded
        to the observed values since there is no observation in HL.

        Modifiers are extrapolated with respect to their nature."""
        lumi = float(lumi)
        if lumi == self.lumi or self.hf in [{}, []]:
            return self.hf
        HF = copy.deepcopy(self.hf)
        lumi_scale = round(lumi / self.lumi, 6)

        if isinstance(self, HF_Background):
            # Background extrapolation
            total_expected = {}
            for SR, item in self.pyhf_config.items():
                if SR != "lumi":
                    total_expected[SR] = [0.0] * len(item["data"])

            for iSR in range(len(HF["channels"])):
                self.logger.debug(
                    "  * Extrapolating channel " + str(HF["channels"][iSR]["name"])
                )
                if len(total_expected[HF["channels"][iSR]["name"]]) == 0:
                    continue

                # modify the expected data of the sample
                for sample in range(len(HF["channels"][iSR]["samples"])):
                    self.logger.debug(
                        "    * Extrapolating "
                        + str(HF["channels"][iSR]["samples"][sample]["name"])
                        + " sample"
                    )
                    for i in range(len(HF["channels"][iSR]["samples"][sample]["data"])):
                        HF["channels"][iSR]["samples"][sample]["data"][i] *= lumi_scale
                        total_expected[HF["channels"][iSR]["name"]][i] += HF["channels"][
                            iSR
                        ]["samples"][sample]["data"][i]

                    for imod in range(
                        len(HF["channels"][iSR]["samples"][sample]["modifiers"])
                    ):
                        mod_type = HF["channels"][iSR]["samples"][sample]["modifiers"][
                            imod
                        ]["type"]
                        if mod_type in ["normsys", "normfactor", "shapefactor", "lumi"]:
                            continue

                        # extrapolate shape variables
                        elif mod_type == "shapesys":
                            for i in range(
                                len(
                                    HF["channels"][iSR]["samples"][sample]["modifiers"][
                                        imod
                                    ]["data"]
                                )
                            ):
                                HF["channels"][iSR]["samples"][sample]["modifiers"][imod][
                                    "data"
                                ][i] *= lumi_scale

                        # extrapolate histo variables
                        elif mod_type == "histosys":
                            for i in range(
                                len(
                                    HF["channels"][iSR]["samples"][sample]["modifiers"][
                                        imod
                                    ]["data"]["hi_data"]
                                )
                            ):
                                HF["channels"][iSR]["samples"][sample]["modifiers"][imod][
                                    "data"
                                ]["hi_data"][i] *= lumi_scale
                            for i in range(
                                len(
                                    HF["channels"][iSR]["samples"][sample]["modifiers"][
                                        imod
                                    ]["data"]["lo_data"]
                                )
                            ):
                                HF["channels"][iSR]["samples"][sample]["modifiers"][imod][
                                    "data"
                                ]["lo_data"][i] *= lumi_scale

                        # extrapolate stat variables
                        elif mod_type == "staterror":
                            for i in range(
                                len(
                                    HF["channels"][iSR]["samples"][sample]["modifiers"][
                                        imod
                                    ]["data"]
                                )
                            ):
                                HF["channels"][iSR]["samples"][sample]["modifiers"][imod][
                                    "data"
                                ][i] *= math.sqrt(lumi_scale)

            # replace the observed bkg with total expected bkg
            for key, item in total_expected.items():
                for iobs in range(len(HF["observations"])):
                    if key == HF["observations"][iobs]["name"]:
                        if item != []:
                            HF["observations"][iobs]["data"] = item

        elif isinstance(self, HF_Signal):  # type(self) == HF_Signal:
            # Signal extrapolation
            for i in range(len(HF)):
                if HF[i]["op"] == "remove":
                    continue
                HF[i]["value"]["data"] = [
                    round(x * lumi_scale, 6) for x in HF[i]["value"]["data"]
                ]
                # Extrapolate modifiers
                for imod in range(len(HF[i]["value"]["modifiers"])):
                    mod_type = HF[i]["value"]["modifiers"][imod]["type"]
                    if mod_type in ["normsys", "normfactor", "shapefactor", "lumi"]:
                        continue

                    # extrapolate shape variables
                    elif mod_type == "shapesys":
                        for i in range(len(HF[i]["value"]["modifiers"][imod]["data"])):
                            HF[i]["value"]["modifiers"][imod]["data"][i] *= lumi_scale

                    # extrapolate histo variables
                    elif mod_type == "histosys":
                        for i in range(
                            len(HF[i]["value"]["modifiers"][imod]["data"]["hi_data"])
                        ):
                            HF[i]["value"]["modifiers"][imod]["data"]["hi_data"][
                                i
                            ] *= lumi_scale
                        for i in range(
                            len(HF[i]["value"]["modifiers"][imod]["data"]["lo_data"])
                        ):
                            HF[i]["value"]["modifiers"][imod]["data"]["lo_data"][
                                i
                            ] *= lumi_scale

                    # extrapolate statistical variables
                    elif mod_type == "staterror":
                        for i in range(len(HF[i]["value"]["modifiers"][imod]["data"])):
                            HF[i]["value"]["modifiers"][imod]["data"][i] *= math.sqrt(
                                lumi_scale
                            )

        return HF


class HF_Background(HistFactory):
    def __init__(self, pyhf_config: dict, expected: bool = False):
        super().__init__(pyhf_config)

        if WorkspaceInterpreter is None:
            raise ImportError(
                "The 'spey_pyhf' package is required for the HistFactory class."
            )

        bkg_file = self.path.joinpath(self.name)
        self.logger.debug("Reading : %s", bkg_file)
        if bkg_file.is_file():
            with bkg_file.open("r") as json_file:
                self.hf = json.load(json_file)
        else:
            self.logger.warning("Can not find file : %s", bkg_file)

        if expected:
            self.hf = self.impose_expected()

    def size(self):
        # The number of SRs in the likelihood profile
        return list(WorkspaceInterpreter(self.hf).bin_map.values())

    def impose_expected(self):
        """
        To switch observed data with total expected data per SR bin.
        """
        total_expected = {}
        HF = copy.deepcopy(self.hf)
        for i in range(len(HF.get("observations", []))):
            total_expected[HF["observations"][i]["name"]] = [0.0] * len(
                HF["observations"][i]["data"]
            )

        for iSR in range(len(HF["channels"])):
            for sample in range(len(HF["channels"][iSR]["samples"])):
                for SRbin in range(len(HF["channels"][iSR]["samples"][sample]["data"])):
                    total_expected[HF["channels"][iSR]["name"]][SRbin] += HF["channels"][
                        iSR
                    ]["samples"][sample]["data"][SRbin]

        # replace the observed bkg with total expected bkg
        for key, item in total_expected.items():
            for iobs in range(len(HF["observations"])):
                if key == HF["observations"][iobs]["name"]:
                    HF["observations"][iobs]["data"] = [round(x, 5) for x in item]

        return HF

    def get_expected(self):
        return self.impose_expected().get("observations", [])

    def get_observed(self):
        return self.hf.get("observations", [])

    def get_sample_names(self):
        samples = {}
        HF = copy.deepcopy(self.hf)
        for iSR in range(len(HF.get("channels", []))):
            samples[HF["channels"][iSR]["name"]] = []
            for sample in range(len(HF["channels"][iSR]["samples"])):
                samples[HF["channels"][iSR]["name"]].append(
                    HF["channels"][iSR]["samples"][sample]["name"]
                )
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

    def __init__(self, pyhf_config, regiondata, xsection=-1, **kwargs):
        super().__init__(pyhf_config)
        self.signal_config = {}

        if WorkspaceInterpreter is None:
            raise ImportError(
                "The 'spey_pyhf' package is required for the HistFactory class."
            )

        with self.path.joinpath(self.name).open("r") as json_file:
            tmp_bkg = WorkspaceInterpreter(json.load(json_file))

        bin_map = tmp_bkg.bin_map
        self.poi_name = tmp_bkg.poi_name[0][1]

        for key, item in self.pyhf_config.items():
            if key != "lumi":
                self.signal_config[key] = {}
                if not item["is_included"]:
                    self.signal_config[key]["op"] = "remove"
                    self.signal_config[key]["path"] = "/channels/" + str(item["channels"])
                else:
                    self.signal_config[key]["op"] = "add"
                    self.signal_config[key]["path"] = (
                        "/channels/"
                        + str(item["channels"])
                        + "/samples/"
                        + str(
                            len(tmp_bkg["channels"][int(item["channels"])]["samples"]) - 1
                        )
                    )
                    self.signal_config[key]["bin_size"] = bin_map[key]

                self.signal_config[key]["data"] = []
                for SRname in item["data"]:
                    if kwargs.get("validate", False):
                        # initiate mock validation sequence, this requires the
                        # background to be given in kwargs
                        self.signal_config[key]["data"].append(1.0)
                    else:
                        self.signal_config[key]["data"].append(
                            regiondata[SRname]["Nf"] / regiondata[SRname]["N0"]
                        )

        self.hf = self.set_HF(
            xsection,
            background=kwargs.get("background", {}),
            add_normsys=kwargs.get("add_normsys", []),
            add_histosys=kwargs.get("add_histosys", []),
        )

    def set_HF(self, xsection, **kwargs):
        HF = []
        if xsection <= 0.0:
            return HF
        toRemove = []
        for ix, SR in enumerate(self.signal_config.keys()):
            if self.signal_config[SR]["op"] != "remove":
                SR_tmp = {
                    "op": self.signal_config[SR]["op"],
                    "path": self.signal_config[SR]["path"],
                    "value": {
                        "name": "MA5_signal_" + str(ix),
                        "data": [
                            eff * xsection * self.lumi * 1000.0
                            for eff in self.signal_config[SR]["data"]
                        ],
                        "modifiers": [
                            {"data": None, "name": "lumi", "type": "lumi"},
                            {"data": None, "name": self.poi_name, "type": "normfactor"},
                        ],
                    },
                }
                if len(SR_tmp["value"]["data"]) == 0:
                    SR_tmp["value"]["data"] = [0.0] * self.signal_config[SR]["bin_size"]
                HF.append(SR_tmp)
            else:
                toRemove.append(
                    {
                        "op": self.signal_config[SR]["op"],
                        "path": self.signal_config[SR]["path"],
                    }
                )

        # Need to sort correctly the paths to the channels to be removed
        toRemove.sort(key=lambda p: p["path"].split("/")[-1], reverse=True)
        for d in toRemove:
            HF.append(d)

        for sys in kwargs.get("add_normsys", []):
            HF = self.add_normsys(HF, sys["hi"], sys["lo"], sys["name"])
        for sys in kwargs.get("add_histosys", []):
            HF = self.add_normsys(HF, sys["hi_data"], sys["lo_data"], sys["name"])

        background = kwargs.get("background", {})
        if isinstance(background, HF_Background):
            if not self.validate_bins(background, HF):
                self.logger.warning("Signal HistFactory validation failed.")
                return []
        return HF

    def validate_bins(self, background, HF: list = None):
        if HF is None:
            HF = self.hf
        bkg_bins = background.size()
        to_validate = [False] * len(bkg_bins)
        if HF == {}:
            return all(to_validate)
        try:
            for sample in HF:
                # check if the size of the bins in the data matches the background
                if sample["op"] == "remove":
                    to_validate[int(sample["path"].split("/")[2])] = True
                    continue
                elif (
                    len(sample["value"]["data"])
                    == bkg_bins[int(sample["path"].split("/")[2])]
                ):
                    to_validate[int(sample["path"].split("/")[2])] = True
                # also check if the modifier data size matches with the background
                for modifier in sample["value"]["modifiers"]:
                    if modifier["type"] == "histosys":
                        if (
                            len(modifier["data"]["hi_data"])
                            != bkg_bins[int(sample["path"].split("/")[2])]
                        ):
                            to_validate[int(sample["path"].split("/")[2])] = False
                        if (
                            len(modifier["data"]["lo_data"])
                            != bkg_bins[int(sample["path"].split("/")[2])]
                        ):
                            to_validate[int(sample["path"].split("/")[2])] = False
        except:
            self.logger.debug("Signal HistFactory : Key error in dictionary...")
            return False
        return all(to_validate)

    def isAlive(self):
        for sample in self.hf:
            if sample["op"] != "remove":
                if any([s > 0 for s in sample["value"]["data"]]):
                    return True
        return False

    def add_normsys(self, HF, hi, lo, name):
        # systematic unc: name has to be MA5_scale, MA5_PDF, MA5_TH or MA5_sys
        # hi = 1.XX lo = 0.XX
        for i in range(len(HF)):
            if HF[i]["op"] == "remove":
                continue
            HF[i]["value"]["modifiers"].append(
                {"name": name, "type": "normsys", "data": {"hi": hi, "lo": lo}}
            )
        return HF

    def add_histosys(self, HF, hi_data, lo_data, name):
        # scale and TH uncertainties: name has to be MA5_scale, MA5_PDF, MA5_TH or MA5_sys
        # hi_data,lo_data are list!!
        for i in range(len(HF)):
            if HF[i]["op"] == "remove":
                continue
            HF[i]["value"]["modifiers"].append(
                {
                    "name": name,
                    "type": "histosys",
                    "data": {"hi_data": hi_data, "lo_data": lo_data},
                }
            )
        return HF

    def clear_modifiers(self):
        for i in range(len(self.hf)):
            self.hf[i]["value"]["modifiers"] = [
                {"data": None, "name": "lumi", "type": "lumi"},
                {"data": None, "name": "mu_SIG", "type": "normfactor"},
            ]


def get_HFID(file, SRname):
    """
    Extract the location of the profiles within the JSON file.
    """
    if os.path.isfile(file):
        with open(file, "r") as json_file:
            HF = json.load(json_file)
    else:
        return "Can not find background file: " + file
    for ch in HF["channels"]:
        if ch["name"] == SRname:
            return HF["channels"].index(ch)
    return "Invalid or corrupted info file."


def construct_histfactory_dictionary(info_root, run_recast_session) -> Tuple[dict, list]:
    """Read the info file and construct histfactory dictionary"""
    pyhf_config = OrderedDict()
    nprofile, default_lumi = 0, 0
    to_remove = []
    run_recast_session.logger.debug(" === Reading info file for pyhf ===")
    for child in info_root:
        if child.tag == "lumi":
            default_lumi = float(child.text)
        if child.tag == "pyhf":
            likelihood_profile = child.attrib.get("id", "HF-Likelihood-" + str(nprofile))
            if likelihood_profile == "HF-Likelihood-" + str(nprofile):
                nprofile += 1
            if not likelihood_profile in list(pyhf_config.keys()):
                pyhf_config[likelihood_profile] = {
                    "name": "No File name in info file...",
                    "path": os.path.join(
                        run_recast_session.pad, "Build/SampleAnalyzer/User/Analyzer"
                    ),
                    "lumi": default_lumi,
                    "SR": OrderedDict(),
                }
            for subchild in child:
                if subchild.tag == "name":
                    if (
                        run_recast_session.main.recasting.simplify_likelihoods
                        and run_recast_session.main.session_info.has_simplify
                    ):
                        main_path = pyhf_config[likelihood_profile]["path"]
                        full = str(subchild.text)
                        simplified = full.split(".json")[0] + "_simplified.json"
                        if os.path.isfile(os.path.join(main_path, simplified)):
                            pyhf_config[likelihood_profile]["name"] = simplified
                        else:
                            simplify_path = os.path.join(
                                run_recast_session.main.archi_info.ma5dir,
                                "tools/simplify/simplify-master/src",
                            )
                            try:
                                if (
                                    os.path.isdir(simplify_path)
                                    and simplify_path not in sys.path
                                ):
                                    sys.path.insert(0, simplify_path)
                                import simplify

                                run_recast_session.logger.debug(
                                    "simplify has been imported from "
                                    + " ".join(simplify.__path__)
                                )
                                run_recast_session.logger.debug("simplifying " + full)
                                with open(os.path.join(main_path, full), "r") as f:
                                    spec = json.load(f)
                                # Get model and data
                                poi_name = "lumi"
                                try:
                                    original_poi = spec["measurements"][0]["config"][
                                        "poi"
                                    ]
                                    spec["measurements"][0]["config"]["poi"] = poi_name
                                except IndexError:
                                    raise simplify.exceptions.InvalidMeasurement(
                                        "The measurement index 0 is out of bounds."
                                    )
                                model, data = simplify.model_tools.model_and_data(spec)

                                fixed_params = model.config.suggested_fixed()
                                init_pars = model.config.suggested_init()
                                # Fit the model to data
                                fit_result = simplify.fitter.fit(
                                    model,
                                    data,
                                    init_pars=init_pars,
                                    fixed_pars=fixed_params,
                                )
                                # Get yields
                                ylds = simplify.yields.get_yields(spec, fit_result, [])
                                newspec = simplify.simplified.get_simplified_spec(
                                    spec,
                                    ylds,
                                    allowed_modifiers=[],
                                    prune_channels=[],
                                    include_signal=False,
                                )
                                newspec["measurements"][0]["config"]["poi"] = original_poi
                                with open(
                                    os.path.join(main_path, simplified), "w+"
                                ) as out_file:
                                    json.dump(newspec, out_file, indent=4, sort_keys=True)
                                pyhf_config[likelihood_profile]["name"] = simplified
                            except ImportError:
                                run_recast_session.logger.warning(
                                    "To use simplified likelihoods, please install simplify"
                                )
                                pyhf_config[likelihood_profile]["name"] = str(
                                    subchild.text
                                )
                            except (
                                Exception,
                                simplify.exceptions.InvalidMeasurement,
                            ) as err:
                                run_recast_session.logger.warning(
                                    "Can not simplify " + full
                                )
                                run_recast_session.logger.debug(str(err))
                                pyhf_config[likelihood_profile]["name"] = str(
                                    subchild.text
                                )
                    else:
                        pyhf_config[likelihood_profile]["name"] = str(subchild.text)
                    run_recast_session.logger.debug(
                        pyhf_config[likelihood_profile]["name"] + " file will be used."
                    )
                elif subchild.tag == "regions":
                    for channel in subchild:
                        if channel.tag == "channel":
                            if not channel.attrib.get("name", False):
                                run_recast_session.logger.warning(
                                    "Invalid or corrupted info file"
                                )
                                run_recast_session.logger.warning(
                                    "Please check " + likelihood_profile
                                )
                                to_remove.append(likelihood_profile)
                            else:
                                data = []
                                if channel.text != None:
                                    data = channel.text.split()
                                pyhf_config[likelihood_profile]["SR"][
                                    channel.attrib["name"]
                                ] = {
                                    "channels": channel.get("id", default=-1),
                                    "data": data,
                                }
                                is_included = (
                                    (
                                        channel.get("is_included", default=0)
                                        in ["True", "1", "yes"]
                                    )
                                    if len(data) == 0
                                    else True
                                )
                                pyhf_config[likelihood_profile]["SR"][
                                    channel.attrib["name"]
                                ].update({"is_included": is_included})
                                if (
                                    pyhf_config[likelihood_profile]["SR"][
                                        channel.attrib["name"]
                                    ]["channels"]
                                    == -1
                                ):
                                    file = os.path.join(
                                        pyhf_config[likelihood_profile]["path"],
                                        pyhf_config[likelihood_profile]["name"],
                                    )
                                    ID = get_HFID(file, channel.attrib["name"])
                                    if not isinstance(ID, str):
                                        pyhf_config[likelihood_profile]["SR"][
                                            channel.attrib["name"]
                                        ]["channels"] = str(ID)
                                    else:
                                        run_recast_session.logger.warning(ID)
                                        run_recast_session.logger.warning(
                                            "Please check "
                                            + likelihood_profile
                                            + "and/or "
                                            + channel.attrib["name"]
                                        )
                                        to_remove.append(likelihood_profile)

    return pyhf_config, to_remove
