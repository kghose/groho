import time
import argparse
import pathlib

import matplotlib.pyplot as plt

import grohoviz.chartmaker


def main():
    parser = argparse.ArgumentParser(description="Plot simulation data")
    parser.add_argument("datadir", help="Directory with simulation data")
    parser.add_argument("plotfile", help="File with plotting instructions")
    args = parser.parse_args()

    chart_maker = grohoviz.chartmaker.ChartMaker(
        datadir=pathlib.Path(args.datadir), plotting_file=pathlib.Path(args.plotfile)
    )

    try:
        while True:
            plt.pause(0.01)
            chart_maker.poll()
    except KeyboardInterrupt:
        pass


if __name__ == "__main__":
    main()
