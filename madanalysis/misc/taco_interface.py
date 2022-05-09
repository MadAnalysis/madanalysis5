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

from typing import Text

from database import Region

from numpy import warnings
import numpy as np
import pyhf

# TODO: remove pyhf dependency from TACO interface


class TACORegion(Region):
    def __init__(
        self,
        analysis: Text,
        regionID: Text,
        nobs: int,
        nb: float,
        deltanb: float,
        N0: float,
        Nf: float,
        xsection: float,
        lumi: float,
        s95exp: float,
        s95obs: float,
        cls: float,
    ) -> None:
        """
        Initialization of MadAnalysis Signal Region
        :param analysis: analysis name
        :param regionID: region name
        :param N0: sum of weights before the analysis
        :param Nf: sum of weights after the final cut
        :param xsection: production cross section
        :param lumi: Luminosity of the analysis [fb^-1]
        :param s95exp: [pb] excluded xsec at 95% CLs calculated via expected events
        :param s95obs: [pb] excluded xsec at 95% CLs calculated via observed events
        :param cls: exclusion CLs
        """
        super(TACORegion, self).__init__(
            regionID=regionID, nobs=nobs, nb=nb, deltanb=deltanb, lumi=lumi, analysis=analysis
        )
        self.final_nevt = Nf
        self.initial_nevt = N0

        # Temporary: SMODELS requires xsection to be a class with value attr
        class XSEC:
            def __init__(self, xsec):
                self.value = xsec

        self.xsection = XSEC(xsection)
        self.xsec = xsection

        self.s95exp = s95exp
        self.s95obs = s95obs
        self.cls = cls

        self.marginalize = False

        self.cachedLlhds = {"exp": {}, "obs": {}}

    @property
    def analysisId(self):
        return self.analysis

    @property
    def dataType(self) -> Text:
        return "efficiencyMap"

    def getUpperLimit(self, expected: bool = False) -> float:
        """
        get exclusion cross section at 95% CLs
        :param expected: is xsec value expected or observed
        :return: float cross section in pb
        """
        return getattr(self, "s95exp" if expected else "s95obs")

    def getRValue(self, expected: bool = False) -> float:
        return self.getUpperLimit(expected) / self.xsection.value

    def likelihood(
        self,
        mu: float = 1.0,
        expected: bool = False,
        nll: bool = False,
        useCached=False,
    ) -> float:
        """
        compute the likelihood for the signal strength mu

        :param mu: signal strength
        :param expected: if true, compute expected likelihood, else observed.
                         if "posteriori", compute a posteriori expected likelihood
                         (FIXME do we need this?)
        :param nll: if True, return the negative log likelihood instead of the
                    likelihood
        """

        if useCached:
            cached = None
            if expected:
                cached = self.cachedLlhds["exp"].get(mu, None)
            else:
                cached = self.cachedLlhds["obs"].get(mu, None)

            if cached is not None:
                return cached

        def get_twice_nllh(model, data, par_bounds):
            try:
                with warnings.catch_warnings():
                    # `pyhf.infer.mle.fixed_poi_fit` returns twice negative log likelihood
                    _, twice_nllh = pyhf.infer.mle.fixed_poi_fit(
                        mu, data, model, return_fitted_val=True, par_bounds=par_bounds
                    )
                return twice_nllh
            except ValueError as err:
                return "update bounds"

        model = pyhf.simplemodels.uncorrelated_background([self.nsignal], [self.nb], [self.deltanb])
        par_bounds = model.config.suggested_bounds()
        # print(par_bounds)
        data = [self.nobs if not expected else self.nb] + model.config.auxdata

        counter = 0
        while True:
            twice_nllh = get_twice_nllh(model, data, par_bounds)
            if twice_nllh == "update bounds":
                par_bounds = [
                    (par_bounds[0][0], par_bounds[0][1] * 2.0),
                    (par_bounds[1][0], par_bounds[1][1] * 2.0),
                ]
                # print(f"update bounds: {par_bounds}")
            else:
                break
            counter += 1
            if counter > 3:
                twice_nllh = -1
                break

        if nll:
            return float(twice_nllh / 2.0)

        llhd = float(np.exp(-twice_nllh / 2.0))
        if expected:
            cached = self.cachedLlhds["exp"][mu] = llhd
        else:
            cached = self.cachedLlhds["obs"][mu] = llhd

        return llhd


if __name__ == "__main__":
    import argparse, json, os
    from smodels.tools.theoryPredictionsCombiner import TheoryPredictionsCombiner
    from smodels.tools.statistics import rootFromNLLs

    parser = argparse.ArgumentParser(description="Test TACO interface")
    path = parser.add_argument_group("Path handling")
    path.add_argument("REGIONDATA", type=str, help="Path to the JSON sterilized region data file.")

    param = parser.add_argument_group("Parameter handling")
    param.add_argument("-xs", "--xsection", type=float, dest="XSEC", help="Cross section in pb.")
    param.add_argument("-lumi", type=float, dest="LUMI", help="Luminosity in fb^-1.")

    args = parser.parse_args()
    if not os.path.isfile(args.REGIONDATA):
        raise FileNotFoundError

    with open(args.REGIONDATA, "r") as f:
        regiondata = json.load(f)

    SRs = []
    for analysis, regdat in regiondata.items():

        for regionID, data in regdat["regiondata"].items():
            if "nobs" not in data.keys():
                continue
            current_region = TACORegion(
                analysis,
                regionID,
                float(data["nobs"]),
                float(data["nb"]),
                float(data["deltanb"]),
                float(data["N0"]),
                float(data["Nf"]),
                args.XSEC,
                args.LUMI,
                float(data["s95exp"]),
                float(data["s95obs"]),
                float(data["CLs"]),
            )
            SRs.append(current_region)
        break

    print(f"Analysis : {analysis}")

    for region in SRs:
        if region.final_nevt > 0.0:
            print(f"\nRegion name: {region.region_id}")
            print(f"Expected upper limit : {region.s95exp}")
            print(f"Obs upper limit : {region.s95obs}")
            print(f"R-value : {region.getRValue()}")
            print(f"Likelihood mu = 1 : {region.likelihood(1.)}")
            print(f"Likelihood mu = 0 : {region.likelihood(0.)}")

    def clsRoot(mu, combiner, expected=False):
        # at - infinity this should be .95,
        # at + infinity it should -.05
        # Make sure to always compute the correct llhd value (from theoryPrediction)
        # and not used the cached value (which is constant for mu~=1 an mu~=0)

        mu_hat, sigma_mu, lmax = combiner.findMuHat(allowNegativeSignals=True, extended_output=True)
        nll0 = combiner.likelihood(mu_hat, expected=expected, nll=True)
        # a posteriori expected is needed here
        # mu_hat is mu_hat for signal_rel
        mu_hatA, _, nll0A = combiner.findMuHat(
            expected="posteriori", nll=True, extended_output=True
        )

        nll = combiner.likelihood(mu, nll=True, expected=expected, useCached=False)
        nllA = combiner.likelihood(mu, expected="posteriori", nll=True, useCached=False)
        ret = rootFromNLLs(nllA, nll0A, nll, nll0)
        return ret

    combiner = TheoryPredictionsCombiner(SRs)
    print("\nTheoryPredictionsCombiner: ")
    print("ul", combiner.getUpperLimitOnMu(expected=False))
    print("expected ul", combiner.getUpperLimitOnMu(expected=True))
    print("r-value", combiner.getRValue())
    print(f"expected 1-CLs : {clsRoot(1., combiner, True)}")
    print(f"obs 1-CLs : {clsRoot(1., combiner, False)}")
