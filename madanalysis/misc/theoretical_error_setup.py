from math import sqrt


def comb_sqr(*args, rnd: int = None) -> float:
    """Combine squared values"""
    val = sqrt(sum(x**2 for x in args))
    if rnd is not None:
        return round(val, rnd)
    return val


def error_dict_setup(
    dataset, systematics: list[list[float]], linear_comb: bool
) -> dict[str, float]:
    """
    Setup the error dictionary for a given dataset.

    Args:
        dataset (``_type_``): dataset description
        systematics (``list[list[float]]``): systematics description

    Returns:
        ``dict[str, float]``:
        error dictionary
    """

    def comb(*args, rnd=8):
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
