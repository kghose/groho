# Example script to plot data

import sys
import pathlib
import glob

import numpy as np
from scipy.spatial.transform import Rotation as Rot
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D

rot = Rot.from_euler("x", -23.5, degrees=True)


def load_data(folder: pathlib.Path):
    dtype = np.dtype([("x", "f8"), ("y", "f8"), ("z", "f8")])
    trajectories = {}
    for f in glob.glob(str(folder / "pos*.bin")):
        naif = int(pathlib.Path(f).name[3:-4])
        x = np.fromfile(f, dtype=dtype)
        trajectories[naif] = rot.apply(x.view(np.float64).reshape(x.shape + (-1,)))
    return trajectories


def base_chart(trajectories: dict):
    for k, v in trajectories.items():
        plt.plot(v[:, 0], v[:, 1], label=k)
    plt.gca().set_aspect("equal")


def d3_chart(trajectories: dict):
    fig = plt.figure()
    ax = fig.add_subplot(111, projection="3d")
    for k, v in trajectories.items():
        ax.plot(v[:, 0], v[:, 1], v[:, 2])
    # plt.gca().set_aspect("equal")
    ax.set_xlabel("X axis")
    ax.set_ylabel("Y axis")
    ax.set_zlabel("Z axis")


def main():
    trajectories = load_data(pathlib.Path(sys.argv[1]))
    base_chart(trajectories)
    # d3_chart(trajectories)
    plt.show()


if __name__ == "__main__":
    main()
