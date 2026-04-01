def selection_94():

    # Library import
    import numpy
    import matplotlib
    import matplotlib.pyplot   as plt
    import matplotlib.gridspec as gridspec

    # Library version
    matplotlib_version = matplotlib.__version__
    numpy_version      = numpy.__version__

    # Histo binning
    xBinning = numpy.linspace(-3.2,3.2,33,endpoint=True)

    # Creating data sequence: middle of each bin
    xData = numpy.array([-3.1,-2.9000000000000004,-2.7,-2.5,-2.3000000000000003,-2.1,-1.9000000000000001,-1.7000000000000002,-1.5,-1.3,-1.1,-0.8999999999999999,-0.7000000000000002,-0.5,-0.2999999999999998,-0.10000000000000009,0.10000000000000009,0.2999999999999998,0.5,0.7000000000000002,0.9000000000000004,1.0999999999999996,1.2999999999999998,1.5,1.7000000000000002,1.9000000000000004,2.1000000000000005,2.3,2.5,2.7,2.9000000000000004,3.1000000000000005])

    # Creating weights for histo: y95_PHI_0
    y95_PHI_0_weights = numpy.array([0.185374,0.726048,0.4788827,0.4943306,0.4016436,0.5561219,0.4943306,0.4479871,0.3707479,0.5097784,0.4170914,0.633361,0.4325392,0.6797045,0.4634349,0.4479871,0.3553001,0.4325392,0.5252262,0.3861957,0.5715697,0.6024654,0.4479871,0.3707479,0.5715697,0.633361,0.3553001,0.5561219,0.6179132,0.4325392,0.5870175,0.2626131])

    # Creating a new Canvas
    fig   = plt.figure(figsize=(8.75,6.25),dpi=80)
    frame = gridspec.GridSpec(1,1)
    pad   = fig.add_subplot(frame[0])

    # Creating a new Stack
    pad.hist(x=xData, bins=xBinning, weights=y95_PHI_0_weights,\
             label="$testset$", histtype="stepfilled", rwidth=1.0,\
             color="#5954d8", edgecolor="#5954d8", linewidth=1, linestyle="solid",\
             bottom=None, cumulative=False, density=False, align="mid", orientation="vertical")


    # Axis
    plt.rc('text',usetex=False)
    plt.xlabel(r"$\phi$ $[ b_{1} ]$ ",\
               fontsize=16,color="black")
    plt.ylabel(r"$\mathrm{Events}$ $(\mathrm{not}\ \mathrm{normalized})$",\
               fontsize=16,color="black")

    # Boundary of y-axis
    ymax=(y95_PHI_0_weights).max()*1.1
    ymin=0 # linear scale
    #ymin=min([x for x in (y95_PHI_0_weights) if x])/100. # log scale
    plt.gca().set_ylim(ymin,ymax)

    # Log/Linear scale for X-axis
    plt.gca().set_xscale("linear")
    #plt.gca().set_xscale("log",nonpositive="clip")

    # Log/Linear scale for Y-axis
    plt.gca().set_yscale("linear")
    #plt.gca().set_yscale("log",nonpositive="clip")

    # Saving the image
    plt.savefig('../../HTML/MadAnalysis5job_0/selection_94.png')
    plt.savefig('../../PDF/MadAnalysis5job_0/selection_94.png')
    plt.savefig('../../DVI/MadAnalysis5job_0/selection_94.eps')

# Running!
if __name__ == '__main__':
    selection_94()
