# Example script to plot data

import sys
import pathlib
import glob

import numpy as np
import matplotlib.pyplot as plt


def load_data(folder: pathlib.Path):
    dtype = np.dtype([("x", "f8"), ("y", "f8"), ("z", "f8")])
    trajectories = {}
    for f in glob.glob(str(folder / "pos*.bin")):
        naif = int(pathlib.Path(f).name[3:-4])
        trajectories[naif] = np.fromfile(f, dtype=dtype)
    return trajectories


def base_chart(trajectories: dict):
    for k, v in trajectories.items():
        plt.plot(v["x"], v["y"], label=k)
    plt.gca().set_aspect("equal")


def main():
    trajectories = load_data(pathlib.Path(sys.argv[1]))
    base_chart(trajectories)
    plt.show()


if __name__ == "__main__":
    main()
