import time
import argparse
import pathlib

import grohoviz.chartmaker


def main():
    parser = argparse.ArgumentParser(description="Plot simulation data")
    parser.add_argument("datadir", help="Directory with simulation data")
    parser.add_argument("plotfile", help="File with plotting instructions")
    args = parser.parse_args()

    chart_maker = grohoviz.chartmaker.ChartMaker(
        datadir=pathlib.Path(args.datadir), plotting_file=pathlib.Path(args.plotfile)
    )
    chart_maker.start()

    try:
        while True:
            time.sleep(1)
    except KeyboardInterrupt:
        chart_maker.stop()

    # trajectories = plotlib.load_data(pathlib.Path(sys.argv[1]))
    # plotlib.print_stats(trajectories)
    # plotlib.chart(trajectories, targets=[399, 2000001, 2000002])
    # # chart(trajectories, targets=[2000016])
    # # chart_ref_to(trajectories, 399, targets=[301, -1000, -2000], dt=60)
    # plotlib.plt.show()


if __name__ == "__main__":
    main()
