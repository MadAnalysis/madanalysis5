def selection_27():

    # Library import
    import numpy
    import matplotlib
    import matplotlib.pyplot   as plt
    import matplotlib.gridspec as gridspec

    # Library version
    matplotlib_version = matplotlib.__version__
    numpy_version      = numpy.__version__

    # Histo binning
    xBinning = numpy.linspace(1.0,100.0,51,endpoint=True)

    # Creating data sequence: middle of each bin
    xData = numpy.array([1.99,3.9699999999999998,5.95,7.93,9.91,11.89,13.87,15.85,17.83,19.81,21.79,23.77,25.75,27.73,29.71,31.69,33.67,35.65,37.63,39.61,41.589999999999996,43.57,45.55,47.53,49.51,51.49,53.47,55.45,57.43,59.41,61.39,63.37,65.35,67.33,69.31,71.29,73.27,75.25,77.23,79.21,81.19,83.17,85.15,87.13,89.11,91.09,93.07,95.05,97.03,99.01])

    # Creating weights for histo: y28_GAMMA_0
    y28_GAMMA_0_weights = numpy.array([4.031884,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0])

    # Creating a new Canvas
    fig   = plt.figure(figsize=(8.75,6.25),dpi=80)
    frame = gridspec.GridSpec(1,1)
    pad   = fig.add_subplot(frame[0])

    # Creating a new Stack
    pad.hist(x=xData, bins=xBinning, weights=y28_GAMMA_0_weights,\
             label="$testset$", histtype="stepfilled", rwidth=1.0,\
             color="#5954d8", edgecolor="#5954d8", linewidth=1, linestyle="solid",\
             bottom=None, cumulative=False, density=False, align="mid", orientation="vertical")


    # Axis
    plt.rc('text',usetex=False)
    plt.xlabel(r"$\gamma$ $[ e ]$ ",\
               fontsize=16,color="black")
    plt.ylabel(r"$\mathrm{N.}\ \mathrm{of}\ e$ $(\mathrm{not}\ \mathrm{normalized})$",\
               fontsize=16,color="black")

    # Boundary of y-axis
    ymax=(y28_GAMMA_0_weights).max()*1.1
    ymin=0 # linear scale
    #ymin=min([x for x in (y28_GAMMA_0_weights) if x])/100. # log scale
    plt.gca().set_ylim(ymin,ymax)

    # Log/Linear scale for X-axis
    plt.gca().set_xscale("linear")
    #plt.gca().set_xscale("log",nonpositive="clip")

    # Log/Linear scale for Y-axis
    plt.gca().set_yscale("linear")
    #plt.gca().set_yscale("log",nonpositive="clip")

    # Saving the image
    plt.savefig('../../HTML/MadAnalysis5job_0/selection_27.png')
    plt.savefig('../../PDF/MadAnalysis5job_0/selection_27.png')
    plt.savefig('../../DVI/MadAnalysis5job_0/selection_27.eps')

# Running!
if __name__ == '__main__':
    selection_27()
