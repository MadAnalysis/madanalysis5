"""Construction of the Spey statistical models of the recasting mode and computation of
the upper limits on the signal cross section.
"""

import logging

import spey
from numpy import isinf, isnan

from .histfactory_reader import HF_Background, HF_Signal

APRIORI = spey.ExpectationType.apriori
APOSTERIORI = spey.ExpectationType.aposteriori
OBSERVED = spey.ExpectationType.observed

logger = logging.getLogger("MA5")


# FIXME: 'list[str]'/'dict[...]' annotations are evaluated at import time: this module cannot
# be imported with Python 3.8 (same issue in theoretical_error_setup.py, run_recast.py and
# install_service.py).
def initialise_statistical_models(
    regiondata: dict,
    regions: list[str],
    xsection: float,
    lumi: float,
    simplified_model_config: dict = None,
    full_statistical_model_config: dict = None,
) -> dict[str, dict[str, spey.StatisticalModel]]:
    r"""Build the statistical models of an analysis.

    The signal yield of each region is :math:`\sigma \times L \times 1000 \times N_f / N_0`
    (:math:`\sigma` in pb, :math:`L` in fb\ :sup:`-1`). Three kinds of models are built:

    * one ``default.uncorrelated_background`` model per signal region;
    * one ``default.correlated_background`` model (simplified likelihood) per covariance
      subset;
    * one ``pyhf`` model (full likelihood) per likelihood profile.

    Args:
        regiondata (``dict``): data per region (``nobs``, ``nb``, ``deltanb``, ``N0``, ``Nf``).
        regions (``list[str]``): signal regions.
        xsection (``float``): signal cross section in pb.
        lumi (``float``): luminosity in fb^-1.
        simplified_model_config (``dict``, default ``None``): covariance subsets
            (``{subset: {"cov_regions": [...], "covariance": [[...]]}}``).
        full_statistical_model_config (``dict``, default ``None``): HistFactory
            configurations.

    Returns:
        ``dict[str, dict[str, spey.StatisticalModel]]``:
        Models indexed by kind (``uncorrelated_background``, ``simplified_likelihoods``,
        ``full_likelihoods``) and by region/subset/profile.
    """
    uncorrelated_background = {}
    simplified_likelihoods = {}
    full_likelihoods = {}

    signal_yields_per_region = {}

    # Uncorrelated background
    pdf_wrapper = spey.get_backend("default.uncorrelated_background")
    for reg in regions:
        signal_yields_per_region[reg] = (
            xsection * lumi * 1000.0 * regiondata[reg]["Nf"] / regiondata[reg]["N0"]
        )
        uncorrelated_background[reg] = pdf_wrapper(
            signal_yields=[signal_yields_per_region[reg]],
            background_yields=[regiondata[reg]["nb"]],
            data=[regiondata[reg]["nobs"]],
            absolute_uncertainties=[regiondata[reg]["deltanb"]],
            analysis=reg,
        )

    # Simplified likelihoods
    if simplified_model_config is not None:
        pdf_wrapper = spey.get_backend("default.correlated_background")
        for cov_subset, item in simplified_model_config.items():
            cov_regions, covariance = item["cov_regions"], item["covariance"]

            observed, backgrounds, nsignal = [], [], []
            for reg in cov_regions:
                nsignal.append(signal_yields_per_region[reg])
                backgrounds.append(regiondata[reg]["nb"])
                observed.append(regiondata[reg]["nobs"])

            simplified_likelihoods[cov_subset] = pdf_wrapper(
                signal_yields=nsignal,
                background_yields=backgrounds,
                data=observed,
                covariance_matrix=covariance,
            )

    # Full likelihoods
    if full_statistical_model_config is not None:
        pdf_wrapper = spey.get_backend("pyhf")
        for llhd_profile, config in full_statistical_model_config.items():
            background = HF_Background(config)(lumi)
            signal = HF_Signal(
                config,
                regiondata,
                xsection=xsection,
                background=background,
            )(lumi)
            full_likelihoods[llhd_profile] = pdf_wrapper(
                signal_patch=signal, background_only_model=background
            )

    return {
        "uncorrelated_background": uncorrelated_background,
        "simplified_likelihoods": simplified_likelihoods,
        "full_likelihoods": full_likelihoods,
    }


def compute_poi_upper_limits(
    regiondata: dict,
    stat_models: dict,
    xsection: float,
    is_extrapolated: bool,
    record_to: str = None,
) -> dict:  # pylint: disable=too-many-arguments
    """Compute the 95% CL upper limits on the signal cross section.

    Expected (a-posteriori, or a-priori for extrapolated luminosities) and observed (not
    for extrapolated luminosities) limits are stored as ``s95exp``/``s95obs`` strings in
    pb (``-1`` if not finite).

    Args:
        regiondata (``dict``): region data, updated in place.
        stat_models (``dict``): models of one kind (see
            :func:`initialise_statistical_models`).
        xsection (``float``): signal cross section in pb (the upper limit on the signal
            strength is multiplied by it).
        is_extrapolated (``bool``): extrapolated luminosity.
        record_to (``str``, default ``None``): sub-dictionary where the results are stored
            (``"cov_subset"`` or ``"pyhf"``); results are stored per region if ``None``.

    Returns:
        ``dict``:
        The updated region data.
    """
    logger.debug("Computing upper limits...")
    if record_to is not None:
        if record_to not in regiondata:
            regiondata[record_to] = {}
    tags = (
        [[APRIORI], ["exp"]]
        if is_extrapolated
        else [[APOSTERIORI, OBSERVED], ["exp", "obs"]]
    )

    for tag, label in zip(*tags):
        for reg, stat_model in stat_models.items():
            logger.debug("running %s for %s", reg, record_to)
            s95 = stat_model.poi_upper_limit(expected=tag) * xsection
            s95 = -1 if isinf(s95) or isnan(s95) else s95
            if record_to is None:
                logger.debug("region %s s95%s = %.5f pb", reg, label, s95)
                regiondata[reg][f"s95{label}"] = f"{s95:.7f}"
            else:
                if reg not in regiondata[record_to]:
                    regiondata[record_to][reg] = {}
                logger.debug("%s:: region %s s95%s = %.5f pb", record_to, reg, label, s95)
                regiondata[record_to][reg][f"s95{label}"] = f"{s95:.7f}"
    return regiondata
