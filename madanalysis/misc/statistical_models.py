import spey
from .histfactory_reader import HF_Background, HF_Signal


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
