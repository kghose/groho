import pathlib
import argparse
import sys
from typing import List

from filelock import FileLock

import matplotlib.pyplot as plt
import matplotlib as mpl
import grohoviz.datalib as datalib

lock_file = "sim.lock"
manifest_file = "manifest.yml"


class ChartMaker:
    def __init__(self, datadir: pathlib.Path, plotting_file: pathlib.Path):
        self.datadir = datadir
        self.plotting_file = plotting_file
        self.datadir_last_changed = 0
        self.plotting_file_last_changed = 0

        self.trajectories = None
        self.ax = plt.subplot(111)
        self.reload_data()

    def poll(self):
        datadir_last_changed = (self.datadir / manifest_file).stat().st_mtime
        plotting_file_last_changed = self.plotting_file.stat().st_mtime
        if datadir_last_changed > self.datadir_last_changed:
            self.reload_data()
            self.datadir_last_changed = datadir_last_changed
        elif plotting_file_last_changed > self.plotting_file_last_changed:
            self.replot()
            self.plotting_file_last_changed = plotting_file_last_changed

    def reload_data(self):
        lock = FileLock(self.datadir / lock_file)
        with lock:
            self.trajectories = datalib.load_data(self.datadir)
            sys.stderr.write("Reloading data\n")
        self.replot()

    def replot(self):
        self.chart()
        plt.draw()
        sys.stderr.write("Replotting\n")

    def chart(self, targets: List[int] = None):
        self.ax.cla()
        for k, v in self.trajectories.items():
            if targets is not None:
                if k not in targets:
                    continue

            self.ax.plot(v.s[:, 0], v.s[:, 1], label=k)
        self.ax.set_aspect("equal")
