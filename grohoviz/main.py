import time
import argparse
import pathlib

import matplotlib.pyplot as plt

from grohoviz.publisher import Publisher


def main():
    parser = argparse.ArgumentParser(description="Plot simulation data")
    parser.add_argument("datadir", help="Directory with simulation data")
    parser.add_argument("plotfile", help="File with plotting instructions")
    args = parser.parse_args()

    publisher = Publisher(
        datadir=pathlib.Path(args.datadir), plotting_file=pathlib.Path(args.plotfile)
    )
    publisher.run()


if __name__ == "__main__":
    main()
