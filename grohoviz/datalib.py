"""Code for loading trajectory data and rotating, interpolating and translating it"""
import pathlib
import glob
from typing import List, Dict

import numpy as np
from scipy.spatial.transform import Rotation as Rot
from scipy import interpolate

rot = Rot.from_euler("x", -23.5, degrees=True)


class SplRep:
    def __init__(self, x, y, z=None):
        self.x, self.y, self.z = x, y, z


class PathT:
    def __init__(self, x, y, z=None, t=None):
        self.x, self.y, self.z, self.t = x, y, z, t


class Trajectories:
    def __init__(self):
        self._t_range = None
        self._trajectories: Dict[(int, int, float), PathT] = {}
        self._splrep = {}
        self._a_interp_to_b = {}
        self._a_interp_to_dt = {}
        self._t_dt = {}

        self._refpath = {}
        self._interp_path = {}

    def get_t(self, t_frac):
        return t_frac * (self._t_range[1] - self._t_range[0]) + self._t_range[0]

    def get_t_span(self):
        return self._t_range[1] - self._t_range[0]

    def bodies(self):
        return set(k[0] for k in self._trajectories.keys())

    def set(self, target, patht: PathT):
        self._trajectories[(target, None, None)] = patht

    def get(self, target, ref=None, dt=None):
        if (target, ref, dt) in self._trajectories:
            return self._trajectories[(target, ref, dt)]
        elif ref is None and dt is None:
            raise RuntimeError(f"No trace called {target}")
        else:
            path = self._compute_path(target, ref, dt)
            self._trajectories[(target, ref, dt)] = path
            return path

    def _compute_path(self, target, ref, dt):
        if dt is None:
            tgt = self._trajectories[(target, None, None)]
            ref = self._get_a_interp_to_b(ref, target)
            t = tgt.t
        else:
            tgt = self._get_a_interp_to_dt(target, dt)
            ref = self._get_a_interp_to_dt(ref, dt)
            t = self._t_dt[dt]

        if tgt is None or ref is None:
            return None
        else:
            return PathT(x=tgt.x - ref.x, y=tgt.y - ref.y, t=t)

    def _get_a_interp_to_b(self, ref, target):
        if (ref, target) not in self._a_interp_to_b:
            ref_splrep = self._get_splrep(ref)
            t = self._trajectories[(target, None, None)].t
            if ref_splrep is None:
                self._a_interp_to_b[(ref, target)] = None
            else:
                self._a_interp_to_b[(ref, target)] = PathT(
                    x=interpolate.splev(t, ref_splrep.x),
                    y=interpolate.splev(t, ref_splrep.y),
                    t=t,
                )
        return self._a_interp_to_b[(ref, target)]

    def _get_a_interp_to_dt(self, ref, dt):
        if (ref, dt) not in self._a_interp_to_dt:
            ref_pt = self._trajectories[(ref, None, None)]
            if dt not in self._t_dt:
                self._t_dt[dt] = np.arange(ref_pt.t[0], ref_pt.t[-1], dt)
            t = self._t_dt[dt]

            ref_splrep = self._get_splrep(ref)
            if ref_splrep is None:
                self._a_interp_to_dt[(ref, dt)] = None
            else:
                self._a_interp_to_dt[(ref, dt)] = PathT(
                    x=interpolate.splev(t, ref_splrep.x),
                    y=interpolate.splev(t, ref_splrep.y),
                    t=t,
                )
        return self._a_interp_to_dt[(ref, dt)]

    def _get_splrep(self, _id):
        if _id not in self._splrep:
            _id_pt = self._trajectories[(_id, None, None)]
            try:
                self._splrep[_id] = SplRep(
                    x=interpolate.splrep(_id_pt.t, _id_pt.x),
                    y=interpolate.splrep(_id_pt.t, _id_pt.y),
                )
            except TypeError as e:
                self._splrep[_id] = None

        return self._splrep[_id]


def load_data(folder: pathlib.Path):
    dtype = np.dtype([("x", "f8"), ("y", "f8"), ("z", "f8"), ("t", "f8")])
    trajectories = Trajectories()
    for f in glob.glob(str(folder / "pos*.bin")):
        naif = int(pathlib.Path(f).name[3:-4])
        x = np.fromfile(f, dtype=dtype)
        trajectories._t_range = (x["t"][0], x["t"][-1])
        if x.size:
            s = rot.apply(x.view(np.float64).reshape(x.shape + (-1,))[:, :3])
            trajectories.set(naif, PathT(x=s[:, 0], y=s[:, 1], z=s[:, 2], t=x["t"]))
    return trajectories
