def selection_110():

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

    # Creating weights for histo: y111_PHI_0
    y111_PHI_0_weights = numpy.array([0.6951523,1.019557,0.911422,0.911422,0.8650785,0.7414958,1.220379,1.220379,0.9732133,0.9423176,0.9423176,0.8805263,0.8496306,0.9886611,0.8805263,0.9732133,0.8496306,1.019557,0.8496306,0.9886611,0.8341828,0.9577655,1.0659,0.8959741,1.035005,0.9732133,1.050452,1.158587,1.127692,1.174035,0.9732133,0.9268698])

    # Creating a new Canvas
    fig   = plt.figure(figsize=(8.75,6.25),dpi=80)
    frame = gridspec.GridSpec(1,1)
    pad   = fig.add_subplot(frame[0])

    # Creating a new Stack
    pad.hist(x=xData, bins=xBinning, weights=y111_PHI_0_weights,\
             label="$testset$", histtype="stepfilled", rwidth=1.0,\
             color="#5954d8", edgecolor="#5954d8", linewidth=1, linestyle="solid",\
             bottom=None, cumulative=False, density=False, align="mid", orientation="vertical")


    # Axis
    plt.rc('text',usetex=False)
    plt.xlabel(r"$\phi$ $[ j ]$ ",\
               fontsize=16,color="black")
    plt.ylabel(r"$\mathrm{N.}\ \mathrm{of}\ j$ $(\mathrm{not}\ \mathrm{normalized})$",\
               fontsize=16,color="black")

    # Boundary of y-axis
    ymax=(y111_PHI_0_weights).max()*1.1
    ymin=0 # linear scale
    #ymin=min([x for x in (y111_PHI_0_weights) if x])/100. # log scale
    plt.gca().set_ylim(ymin,ymax)

    # Log/Linear scale for X-axis
    plt.gca().set_xscale("linear")
    #plt.gca().set_xscale("log",nonpositive="clip")

    # Log/Linear scale for Y-axis
    plt.gca().set_yscale("linear")
    #plt.gca().set_yscale("log",nonpositive="clip")

    # Saving the image
    plt.savefig('../../HTML/MadAnalysis5job_0/selection_110.png')
    plt.savefig('../../PDF/MadAnalysis5job_0/selection_110.png')
    plt.savefig('../../DVI/MadAnalysis5job_0/selection_110.eps')

# Running!
if __name__ == '__main__':
    selection_110()
