import logging

import spey
from numpy import isinf, isnan

from .histfactory_reader import HF_Background, HF_Signal

APRIORI = spey.ExpectationType.apriori
APOSTERIORI = spey.ExpectationType.aposteriori
OBSERVED = spey.ExpectationType.observed

logger = logging.getLogger("MA5")


def initialise_statistical_models(
    regiondata: dict,
    regions: list[str],
    xsection: float,
    lumi: float,
    simplified_model_config: dict = None,
    full_statistical_model_config: dict = None,
) -> dict[str, dict[str, spey.StatisticalModel]]:
    """
    initialise statistical models

    Args:
        regiondata (``dict``): data per region
        regions (``list[str]``): region list
        xsection (``float``): cross section
        lumi (``float``): luminosity
        simplified_model_config (``dict``, default ``None``): simplified model configuration
        full_statistical_model_config (``dict``, default ``None``): full statistical model configuration

    Returns:
        ``dict[str, dict[str, spey.StatisticalModel]]``:
        Statistical model dictionary
    """
    uncorrelated_background = {}
    simplified_likelihoods = {}
    full_likelihoods = {}

    signal_yields_per_region = {}

    # Uncorrelated background
    pdf_wrapper = spey.get_backend("default_pdf.uncorrelated_background")
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
        pdf_wrapper = spey.get_backend("default_pdf.correlated_background")
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
    """
    Compute upper limit on cross section.

    Args:
        regiondata (``dict``): data for each region
        regions (``list[str]``): list of regions
        xsection (``float``): cross section
        lumi (``float``): luminosity
        is_extrapolated (``bool``): extrapolated luminosity
        record_to (``str``): record to a specific section in regiondata

    Returns:
        ``dict``:
        regiondata
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
                regiondata[reg][f"s95{label}"] = f"{s95:20.7f}"
            else:
                if reg not in regiondata[record_to]:
                    regiondata[record_to][reg] = {}
                logger.debug("%s:: region %s s95%s = %.5f pb", record_to, reg, label, s95)
                regiondata[record_to][reg][f"s95{label}"] = f"{s95:20.7f}"
    return regiondata
