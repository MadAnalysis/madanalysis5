def selection_10():

    # Library import
    import numpy
    import matplotlib
    import matplotlib.pyplot   as plt
    import matplotlib.gridspec as gridspec

    # Library version
    matplotlib_version = matplotlib.__version__
    numpy_version      = numpy.__version__

    # Histo binning
    xBinning = numpy.linspace(-1.0,1.0,101,endpoint=True)

    # Creating data sequence: middle of each bin
    xData = numpy.array([-0.99,-0.97,-0.95,-0.9299999999999999,-0.91,-0.89,-0.87,-0.85,-0.83,-0.81,-0.79,-0.77,-0.75,-0.73,-0.71,-0.69,-0.6699999999999999,-0.6499999999999999,-0.63,-0.61,-0.59,-0.5700000000000001,-0.55,-0.53,-0.51,-0.49,-0.47,-0.44999999999999996,-0.42999999999999994,-0.41000000000000003,-0.39,-0.37,-0.35,-0.32999999999999996,-0.30999999999999994,-0.29000000000000004,-0.27,-0.25,-0.22999999999999998,-0.20999999999999996,-0.18999999999999995,-0.16999999999999993,-0.15000000000000002,-0.13,-0.10999999999999999,-0.08999999999999997,-0.06999999999999995,-0.04999999999999993,-0.030000000000000027,-0.010000000000000009,0.010000000000000009,0.030000000000000027,0.050000000000000044,0.07000000000000006,0.09000000000000008,0.1100000000000001,0.13000000000000012,0.15000000000000013,0.16999999999999993,0.18999999999999995,0.20999999999999996,0.22999999999999998,0.25,0.27,0.29000000000000004,0.31000000000000005,0.33000000000000007,0.3500000000000001,0.3700000000000001,0.3900000000000001,0.4099999999999999,0.42999999999999994,0.44999999999999996,0.47,0.49,0.51,0.53,0.55,0.5700000000000001,0.5900000000000001,0.6100000000000001,0.6300000000000001,0.6500000000000001,0.6699999999999999,0.69,0.71,0.73,0.75,0.77,0.79,0.81,0.8300000000000001,0.8500000000000001,0.8700000000000001,0.8900000000000001,0.9100000000000001,0.9299999999999999,0.95,0.97,0.99])

    # Creating weights for histo: y11_WEIGHTS_0
    y11_WEIGHTS_0_weights = numpy.array([0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,15.44783,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0])

    # Creating a new Canvas
    fig   = plt.figure(figsize=(8.75,6.25),dpi=80)
    frame = gridspec.GridSpec(1,1)
    pad   = fig.add_subplot(frame[0])

    # Creating a new Stack
    pad.hist(x=xData, bins=xBinning, weights=y11_WEIGHTS_0_weights,\
             label="$testset$", histtype="stepfilled", rwidth=1.0,\
             color="#5954d8", edgecolor="#5954d8", linewidth=1, linestyle="solid",\
             bottom=None, cumulative=False, density=False, align="mid", orientation="vertical")


    # Axis
    plt.rc('text',usetex=False)
    plt.xlabel(r"$\omega$ ",\
               fontsize=16,color="black")
    plt.ylabel(r"$\mathrm{Events}$ $(\mathrm{not}\ \mathrm{normalized})$",\
               fontsize=16,color="black")

    # Boundary of y-axis
    ymax=(y11_WEIGHTS_0_weights).max()*1.1
    ymin=0 # linear scale
    #ymin=min([x for x in (y11_WEIGHTS_0_weights) if x])/100. # log scale
    plt.gca().set_ylim(ymin,ymax)

    # Log/Linear scale for X-axis
    plt.gca().set_xscale("linear")
    #plt.gca().set_xscale("log",nonpositive="clip")

    # Log/Linear scale for Y-axis
    plt.gca().set_yscale("linear")
    #plt.gca().set_yscale("log",nonpositive="clip")

    # Saving the image
    plt.savefig('../../HTML/MadAnalysis5job_0/selection_10.png')
    plt.savefig('../../PDF/MadAnalysis5job_0/selection_10.png')
    plt.savefig('../../DVI/MadAnalysis5job_0/selection_10.eps')

# Running!
if __name__ == '__main__':
    selection_10()
