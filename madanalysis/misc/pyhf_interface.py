################################################################################
#
#  Copyright (C) 2012-2022 Jack Araz, Eric Conte & Benjamin Fuks
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

import logging, sys, scipy
from typing import Sequence, Dict, Union, Optional
from numpy import warnings, isnan
import numpy as np
from madanalysis.misc.histfactory_reader import HF_Background, HF_Signal

try:
    import pyhf
except ImportError as err:
    logging.getLogger("MA5").error("Can't import pyhf.")
    sys.exit()

from pyhf.optimize import mixins

pyhf.pdf.log.setLevel(logging.CRITICAL)
pyhf.workspace.log.setLevel(logging.CRITICAL)
mixins.log.setLevel(logging.CRITICAL)
pyhf.set_backend("numpy", precision="64b")


class PyhfInterface:
    """
    pyhf interface for MadAnalysis 5
    :param signal: json patch signal
    :param background: json dict for background
    """

    def __init__(
        self,
        signal: Union[HF_Signal, float],
        background: Union[HF_Background, float],
        nb: Optional[float] = None,
        delta_nb: Optional[float] = None,
    ):
        self.model = None
        self.data = None

        self.background = background
        self.signal = signal
        self.nb = nb
        self.delta_nb = delta_nb

    @staticmethod
    def _initialize_workspace(
        signal: Union[Sequence, float],
        background: Union[Dict, float],
        nb: Optional[float] = None,
        delta_nb: Optional[float] = None,
        expected: Optional[bool] = False,
    ):
        """
        Initialize pyhf workspace

        Parameters
        ----------
        signal: Union[Sequence, float]
            number of signal events or json patch
        background: Union[Dict, float]
            number of observed events or json dictionary
        nb: Optional[float]
            number of expected background events (MC)
        delta_nb: Optional[float]
            uncertainty on expected background events
        expected: bool
            if true prepare apriori expected workspace, default False

        Returns
        -------
        Tuple:
            Workspace(can be none in simple case), model, data
        """
        workspace, model, data = None, None, None
        try:
            with warnings.catch_warnings():
                warnings.filterwarnings("ignore")
                if isinstance(signal, float) and isinstance(background, (float, int)):
                    if expected:
                        background = nb
                    # Create model from uncorrelated region
                    model = pyhf.simplemodels.uncorrelated_background(
                        [max(signal, 0.0)], [nb], [delta_nb]
                    )
                    data = [background] + model.config.auxdata

                elif isinstance(signal, HF_Signal) and isinstance(background, HF_Background):
                    HF = background()
                    if expected:
                        HF = background.get_expected()
                    workspace = pyhf.Workspace(HF)
                    model = workspace.model(
                        patches=[signal],
                        modifier_settings={
                            "normsys": {"interpcode": "code4"},
                            "histosys": {"interpcode": "code4p"},
                        },
                    )
                    data = workspace.data(model)
                elif isinstance(signal, list) and isinstance(background, dict):
                    workspace = pyhf.Workspace(background)
                    model = workspace.model(
                        patches=[signal],
                        modifier_settings={
                            "normsys": {"interpcode": "code4"},
                            "histosys": {"interpcode": "code4p"},
                        },
                    )
                    data = workspace.data(model)
        except (pyhf.exceptions.InvalidSpecification, KeyError) as err:
            logging.getLogger("MA5").error("Invalid JSON file!! " + str(err))
        except Exception as err:
            logging.getLogger("MA5").debug("Unknown error, check PyhfInterface " + str(err))

        return workspace, model, data

    def computeCLs(
        self, mu: float = 1.0, expected: bool = False, iteration_threshold: int = 3, **kwargs
    ) -> Union[float, Dict]:
        """
        Compute 1-CLs values

        Parameters
        ----------
        mu: float
            POI signal strength
        CLs_exp: bool
            if true return expected CLs value
        CLs_obs: bool
            if true return observed CLs value
        iteration_threshold: int
            maximum number of trials

        Returns
        -------
        float or dict:
            CLs values {"CLs_obs": xx, "CLs_exp": [xx] * 5} or single CLs value
        """
        _, self.model, self.data = self._initialize_workspace(
            self.signal, self.background, self.nb, self.delta_nb, expected
        )

        if self.model is None or self.data is None:
            if "CLs_exp" in kwargs.keys() or "CLs_obs" in kwargs.keys():
                return -1
            else:
                return {"CLs_obs": -1, "CLs_exp": [-1] * 5}

        def get_CLs(model, data, **keywordargs):
            try:
                CLs_obs, CLs_exp = pyhf.infer.hypotest(
                    mu,
                    data,
                    model,
                    test_stat=keywordargs.get("stats", "qtilde"),
                    par_bounds=keywordargs.get("bounds", model.config.suggested_bounds()),
                    return_expected_set=True,
                )

            except (AssertionError, pyhf.exceptions.FailedMinimization, ValueError) as err:
                logging.getLogger("MA5").debug(str(err))
                # dont use false here 1.-CLs = 0 can be interpreted as false
                return "update bounds"

            # if isnan(float(CLs_obs)) or any([isnan(float(x)) for x in CLs_exp]):
            #     return "update mu"
            CLs_obs = float(CLs_obs[0]) if isinstance(CLs_obs, (list, tuple)) else float(CLs_obs)

            return {
                "CLs_obs": 1.0 - CLs_obs,
                "CLs_exp": list(map(lambda x: float(1.0 - x), CLs_exp)),
            }

        with warnings.catch_warnings():
            warnings.filterwarnings("ignore")
            # pyhf can raise an error if the poi_test bounds are too stringent
            # they need to be updated dynamically.
            arguments = dict(bounds=self.model.config.suggested_bounds(), stats="qtilde")
            it = 0
            while True:
                CLs = get_CLs(self.model, self.data, **arguments)
                if CLs == "update bounds":
                    arguments["bounds"][self.model.config.poi_index] = (
                        arguments["bounds"][self.model.config.poi_index][0],
                        2 * arguments["bounds"][self.model.config.poi_index][1],
                    )
                    logging.getLogger("MA5").debug(
                        "Hypothesis test inference integration bounds has been increased to "
                        + str(arguments["bounds"][self.model.config.poi_index])
                    )
                    it += 1
                elif isinstance(CLs, dict):
                    if isnan(CLs["CLs_obs"]) or any([isnan(x) for x in CLs["CLs_exp"]]):
                        arguments["stats"] = "q"
                        arguments["bounds"][self.model.config.poi_index] = (
                            arguments["bounds"][self.model.config.poi_index][0] - 5,
                            arguments["bounds"][self.model.config.poi_index][1],
                        )
                        logging.getLogger("MA5").debug(
                            "Hypothesis test inference integration bounds has been increased to "
                            + str(arguments["bounds"][self.model.config.poi_index])
                        )
                    else:
                        break
                else:
                    it += 1
                # hard limit on iteration required if it exceeds this value it means
                # Nsig >>>>> Nobs
                if it >= iteration_threshold:
                    if "CLs_exp" in kwargs.keys() or "CLs_obs" in kwargs.keys():
                        return 1
                    return {"CLs_obs": 1.0, "CLs_exp": [1.0] * 5}

        if kwargs.get("CLs_exp", False):
            return CLs["CLs_exp"][2]
        elif kwargs.get("CLs_obs", False):
            return CLs["CLs_obs"]

        return CLs

    def likelihood(
        self,
        mu: float = 1.0,
        nll: bool = False,
        expected: bool = False,
        iteration_threshold: int = 3,
    ) -> float:
        """
        Returns the value of the likelihood.
        Inspired by the `pyhf.infer.mle` module but for non-log likelihood

        Parameters
        ----------
        mu: float
            POI: signal strength
        nll: bool
            if true, return nll, not llhd
        expected: bool
            if true, compute expected likelihood, else observed.
        """
        _, self.model, self.data = self._initialize_workspace(
            self.signal, self.background, self.nb, self.delta_nb, expected
        )

        if self.model is None or self.data is None:
            return -1
        # set a threshold for mu
        mu = max(mu, -20.0)
        mu = min(mu, 40.0)

        def computellhd(model, data, bounds):
            try:
                _, twice_nllh = pyhf.infer.mle.fixed_poi_fit(
                    mu,
                    self.data,
                    self.model,
                    return_fitted_val=True,
                    maxiter=200,
                    par_bounds=bounds,
                )
            except (AssertionError, pyhf.exceptions.FailedMinimization, ValueError) as err:
                logging.getLogger("MA5").debug(str(err))
                return "update bounds"

            return twice_nllh

        with warnings.catch_warnings():
            warnings.filterwarnings(
                "ignore",
                "Values in x were outside bounds during a minimize step, clipping to bounds",
            )

            bounds = self.model.config.suggested_bounds()
            it = 0
            while True:
                twice_nllh = computellhd(self.model, self.data, bounds)
                if twice_nllh == "update bounds":
                    bounds[self.model.config.poi_index] = (
                        bounds[self.model.config.poi_index][0] - 5.0,
                        2.0 * bounds[self.model.config.poi_index][1],
                    )
                    it += 1
                else:
                    break
                if it >= iteration_threshold:
                    logging.getLogger("MA5").debug("pyhf mle.fit failed")
                    return float("nan")

            return np.exp(-twice_nllh / 2.0) if not nll else twice_nllh / 2.0

    def sigma_mu(self, expected: bool = False):
        """
        get an estimate for the standard deviation of mu around mu_hat this is only used for
        initialisation of the optimizer, so can be skipped in the first version,

        adapted from smodels.tools.pyhfInterface

        expected:
            get muhat for the expected likelihood, not observed.
        """
        workspace, self.model, self.data = self._initialize_workspace(
            self.signal, self.background, self.nb, self.delta_nb, expected
        )

        if workspace is not None:
            obss, bgs, bgVars, nsig = {}, {}, {}, {}
            channels = workspace.channels
            for chdata in workspace["channels"]:
                if not chdata["name"] in channels:
                    continue
                bg, var = 0.0, 0.0
                for sample in chdata["samples"]:
                    if sample["name"] == "Bkg":
                        tbg = sample["data"][0]
                        bg += tbg
                        hi = sample["modifiers"][0]["data"]["hi_data"][0]
                        lo = sample["modifiers"][0]["data"]["lo_data"][0]
                        delta = max((hi - bg, bg - lo))
                        var += delta**2
                    if sample["name"] == "bsm":
                        ns = sample["data"][0]
                        nsig[chdata["name"]] = ns
                bgs[chdata["name"]] = bg
                bgVars[chdata["name"]] = var
            for chdata in workspace["observations"]:
                if not chdata["name"] in channels:
                    continue
                obss[chdata["name"]] = chdata["data"][0]
            vars = []
            for c in channels:
                # poissonian error
                if nsig[c] == 0.0:
                    nsig[c] = 1e-5
                poiss = (obss[c] - bgs[c]) / nsig[c]
                gauss = bgVars[c] / nsig[c] ** 2
                vars.append(poiss + gauss)
            var_mu = np.sum(vars)
            n = len(obss)
            return float(np.sqrt(var_mu / (n**2)))

        else:
            return (
                float(np.sqrt(self.delta_nb**2 + self.nb) / self.signal)
                if self.signal > 0.0
                else 0.0
            )

    def muhat(
        self,
        expected: bool = False,
        allowNegativeSignals: bool = True,
        iteration_threshold: int = 3,
    ):
        """
        get the value of mu for which the likelihood is maximal. this is only used for
        initialisation of the optimizer, so can be skipped in the first version of the code.

        Parameters
        ----------
        expected: bool
            get muhat for the expected likelihood, not observed.
        allowNegativeSignals: bool
            if true, allow negative values for mu
        """
        workspace, self.model, self.data = self._initialize_workspace(
            self.signal, self.background, self.nb, self.delta_nb, expected
        )

        def computeMu(model, data, **keywordargs):
            try:
                muhat, maxNllh = pyhf.infer.mle.fit(
                    data,
                    model,
                    return_fitted_val=True,
                    par_bounds=keywordargs.get("bounds", model.config.suggested_bounds()),
                )
            except (AssertionError, pyhf.exceptions.FailedMinimization, ValueError) as err:
                logging.getLogger("MA5").debug(str(err))
                # dont use false here 1.-CLs = 0 can be interpreted as false
                return "update bounds", None

            return muhat, maxNllh

        with warnings.catch_warnings():
            warnings.filterwarnings(
                "ignore",
                "Values in x were outside bounds during a minimize step, clipping to bounds",
            )
            bounds = self.model.config.suggested_bounds()
            if allowNegativeSignals:
                bounds[self.model.config.poi_index] = (-5.0, 10.0)
            arguments = dict(bounds=bounds)
            it = 0
            while True:
                muhat, maxNllh = computeMu(self.model, self.data, **arguments)
                if muhat == "update bounds":
                    arguments["bounds"][self.model.config.poi_index] = (
                        2.0 * arguments["bounds"][self.model.config.poi_index][0],
                        2.0 * arguments["bounds"][self.model.config.poi_index][1],
                    )
                    it += 1
                else:
                    break
                if it >= iteration_threshold:
                    logging.getLogger("MA5").debug("pyhf mle.fit failed")
                    return float("nan")

        return muhat[self.model.config.poi_index]

    def computeUpperLimitOnMu(self, expected: bool = False):
        """
        Compute upper limit on POI (signal strength)
        Parameters
        ----------
        expected:bool
        """
        computer = lambda mu: self.computeCLs(mu=mu, expected=expected, CLs_obs=True) - 0.95

        low, hig = 1.0, 1.0
        while computer(low) > 0.95:
            low *= 0.1
            if low < 1e-10:
                break
        while computer(hig) < 0.95:
            hig *= 10.0
            if hig > 1e10:
                break

        return scipy.optimize.brentq(computer, low, hig, xtol=low / 100.0)
