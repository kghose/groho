"""Code for loading data and processing it"""
import pathlib
import glob

import numpy as np
from scipy.spatial.transform import Rotation as Rot
from scipy import interpolate

rot = Rot.from_euler("x", -23.5, degrees=True)


class Trajectory:
    def __init__(self, t, s):
        self.t, self.s = t, s
        self.transforms = {}


def load_data(folder: pathlib.Path):
    dtype = np.dtype([("x", "f8"), ("y", "f8"), ("z", "f8"), ("t", "f8")])
    trajectories = {}
    for f in glob.glob(str(folder / "pos*.bin")):
        naif = int(pathlib.Path(f).name[3:-4])
        x = np.fromfile(f, dtype=dtype)
        if x.size:
            trajectories[naif] = Trajectory(
                t=x["t"],
                s=rot.apply(x.view(np.float64).reshape(x.shape + (-1,))[:, :3]),
            )
    return trajectories
