# Plotting library.

import sys
import pathlib
import glob
from typing import List

import numpy as np
from numpy.lib.recfunctions import structured_to_unstructured
from scipy.spatial.transform import Rotation as Rot
from scipy import interpolate

import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D

rot = Rot.from_euler("x", -23.5, degrees=True)


def load_data(folder: pathlib.Path):
    dtype = np.dtype([("x", "f8"), ("y", "f8"), ("z", "f8"), ("t", "f8")])
    trajectories = {}
    for f in glob.glob(str(folder / "pos*.bin")):
        naif = int(pathlib.Path(f).name[3:-4])
        x = np.fromfile(f, dtype=dtype)
        if x.size:
            trajectories[naif] = {
                "s": rot.apply(x.view(np.float64).reshape(x.shape + (-1,))[:, :3]),
                "t": x["t"],
            }
    return trajectories


def print_stats(trajectories: dict):
    for k, v in trajectories.items():
        print(f"{k}: {v['t'].size} points: {v['t'][0]} to {v['t'][-1]} ")


def chart(trajectories: dict):
    for k, v in trajectories.items():
        plt.plot(v["s"][:, 0], v["s"][:, 1], label=k)
    plt.gca().set_aspect("equal")


def chart_ref_to_linear(trajectories: dict, ref: int):
    ref_traj = trajectories[ref]
    refx = ref_traj["s"][:, 0]
    refy = ref_traj["s"][:, 1]
    fx = interpolate.interp1d(ref_traj["t"], refx)
    fy = interpolate.interp1d(ref_traj["t"], refy)

    for k, v in trajectories.items():
        if k == ref:
            continue
        if abs(k) / 10 <= 1:
            # Don't draw bary centers
            continue

        try:
            refxnew = fx(v["t"])
            refynew = fy(v["t"])
            plt.plot(v["s"][:, 0] - refxnew, v["s"][:, 1] - refynew)
        except:
            pass
    plt.gca().set_aspect("equal")


def chart_ref_to(trajectories: dict, ref: int, targets: List[int] = None, dt=None):

    ref_traj = trajectories[ref]
    refx = ref_traj["s"][:, 0]
    refy = ref_traj["s"][:, 1]
    fx = interpolate.splrep(ref_traj["t"], refx)
    fy = interpolate.splrep(ref_traj["t"], refy)

    if dt is not None:
        t = np.arange(ref_traj["t"][0], ref_traj["t"][-1], dt)
        refxnew = interpolate.splev(t, fx)
        refynew = interpolate.splev(t, fy)

    for k, v in trajectories.items():
        if k == ref:
            continue
        if abs(k) / 10 <= 1:
            continue

        if targets is not None:
            if k not in targets:
                continue

        try:
            if dt is not None:

                nfx = interpolate.splrep(v["t"], v["s"][:, 0])
                nfy = interpolate.splrep(v["t"], v["s"][:, 1])
                xnew = interpolate.splev(t, nfx)
                ynew = interpolate.splev(t, nfy)

                x = xnew - refxnew
                y = ynew - refynew
            else:
                refxnew = interpolate.splev(v["t"], fx)
                refynew = interpolate.splev(v["t"], fy)
                x = v["s"][:, 0] - refxnew
                y = v["s"][:, 1] - refynew

            plt.plot(x, y)
        except Exception as e:
            print(e)
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
    print_stats(trajectories)
    # chart(trajectories)
    chart_ref_to(trajectories, 301, targets=[301, -1000], dt=300)
    plt.show()


if __name__ == "__main__":
    main()
