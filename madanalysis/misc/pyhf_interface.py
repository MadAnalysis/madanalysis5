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

import logging, sys
from typing import Sequence, Dict, Union, Optional
from numpy import warnings, isnan
import numpy as np

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
        signal: Union[Sequence, float],
        background: Union[Dict, float],
        nb: Optional[float] = None,
        delta_nb: Optional[float] = None,
    ):
        self.model = None
        self.data = None

        self.background = background
        self.signal = signal
        self.nb = nb
        self.delta_nb = delta_nb

        try:
            with warnings.catch_warnings():
                warnings.filterwarnings("ignore")
                if isinstance(signal, float) and isinstance(background, float):
                    # Create model from uncorrelated region
                    self.model = pyhf.simplemodels.uncorrelated_background(
                        [max(signal, 0.0)], [nb], [delta_nb]
                    )
                    self.data = [background] + self.model.config.auxdata
                else:
                    workspace = pyhf.Workspace(background)
                    self.model = workspace.model(
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

    def computeCLs(
        self, CLs_exp: bool = True, CLs_obs: bool = True, iteration_threshold: int = 3
    ) -> Union[float, Dict]:
        """
        Compute 1-CLs values

        Parameters
        ----------
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
        if self.model is None or self.data is None:
            if not CLs_exp or not CLs_obs:
                return -1
            else:
                return {"CLs_obs": -1, "CLs_exp": [-1] * 5}

        def get_CLs(model, data, **kwargs):
            try:
                CLs_obs, CLs_exp = pyhf.infer.hypotest(
                    1.0,
                    data,
                    model,
                    test_stat=kwargs.get("stats", "qtilde"),
                    par_bounds=kwargs.get("bounds", model.config.suggested_bounds()),
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
                    if not CLs_exp or not CLs_obs:
                        return 1
                    return {"CLs_obs": 1.0, "CLs_exp": [1.0] * 5}

        if CLs_exp and not CLs_obs:
            return CLs["CLs_exp"][2]
        elif CLs_obs and not CLs_exp:
            return CLs["CLs_obs"]

        return CLs

    @staticmethod
    def exponentiateNLL(twice_nll: float, doIt: bool) -> float:
        """

        Parameters
        ----------
        twice_nll: float
            twice negative log likelihood
        doIt: bool
            if doIt, then compute likelihood from nll, else return nll
        """
        if twice_nll is None:
            return 0.0 if doIt else 9000.0

        return np.exp(-twice_nll / 2.0) if doIt else twice_nll / 2.0

    def likelihood(self, mu: float = 1.0, nll: bool = False, expected: bool = False) -> float:
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
            if False, compute expected values, if True,
            compute a priori expected, if "posteriori" compute posteriori
            expected
        """
        if self.model is None or self.data is None:
            return -1
        # set a threshold for mu
        mu = max(mu, -20.0)
        mu = min(mu, 40.0)

        with warnings.catch_warnings():
            warnings.filterwarnings(
                "ignore",
                "Values in x were outside bounds during a minimize step, clipping to bounds",
            )
            _, twice_nllh = pyhf.infer.mle.fixed_poi_fit(
                mu, self.data, self.model, return_fitted_val=True, maxiter=200
            )
            return np.exp(-twice_nllh / 2.0) if not nll else twice_nllh / 2.0
