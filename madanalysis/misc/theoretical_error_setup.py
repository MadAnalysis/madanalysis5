"""Combination of the theory (scale, PDF) and systematic uncertainties of the signal cross section.
"""

from math import sqrt


def comb_sqr(*args, rnd: int = None) -> float:
    r"""Combine values in quadrature.

    Args:
        *args (``float``): values to combine.
        rnd (``int``, default ``None``): number of digits to round the result to.

    Returns:
        ``float``:
        :math:`\sqrt{\sum_i x_i^2}`.
    """
    val = sqrt(sum(x**2 for x in args))
    if rnd is not None:
        return round(val, rnd)
    return val


def error_dict_setup(
    dataset: "Any", systematics: list[list[float]], linear_comb: bool
) -> dict[str, float]:
    """Build the relative cross-section variations of a dataset.

    The scale and PDF variations are combined into ``TH_up``/``TH_dn`` (linearly or in
    quadrature), and each systematic uncertainty is combined in quadrature with them
    (``sys<i>_up``/``sys<i>_dn``). Down variations are negative.

    Args:
        dataset (``Dataset``): dataset with ``scaleup``, ``scaledn``, ``pdfup`` and ``pdfdn``.
        systematics (``list[list[float]]``): (up, down) relative systematic uncertainties.
        linear_comb (``bool``): combine the scale and PDF uncertainties linearly
            (quadratically otherwise).

    Returns:
        ``dict[str, float]``:
        Relative variation for each key.
    """

    def comb(*args, rnd: "int" = 8) -> "float":
        """Combine values linearly or in quadrature (depending on ``linear_comb``).

        Args:
            *args (``float``): values to combine.
            rnd (``int``, default ``8``): number of digits to round the result to.

        Returns:
            ``float``:
            The combined value.
        """
        if linear_comb:
            return round(sum(args), rnd)
        return comb_sqr(*args, rnd=rnd)

    err_dict = {
        "scale_up": 0.0,
        "scale_dn": 0.0,
        "pdf_up": 0.0,
        "pdf_dn": 0.0,
    }
    if dataset.scaleup is not None:
        err_dict["scale_up"] = round(dataset.scaleup, 8)
        err_dict["scale_dn"] = -round(dataset.scaledn, 8)
    if dataset.pdfup is not None:
        err_dict["pdf_up"] = round(dataset.pdfup, 8)
        err_dict["pdf_dn"] = -round(dataset.pdfdn, 8)
    err_dict.update(
        {
            "TH_up": comb(err_dict["scale_up"], err_dict["pdf_up"]),
            "TH_dn": -comb(err_dict["scale_dn"], err_dict["pdf_dn"]),
        }
    )
    for idx, syst in enumerate(systematics):
        err_dict.update(
            {
                f"sys{idx}_up": comb_sqr(err_dict["TH_up"], syst[0], rnd=8),
                f"sys{idx}_dn": -comb_sqr(err_dict["TH_dn"], syst[1], rnd=8),
            }
        )
    return err_dict
