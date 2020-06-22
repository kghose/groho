import pathlib
import argparse
import sys
from typing import List

from filelock import FileLock

import matplotlib.pyplot as plt
import matplotlib as mpl
import grohoviz.datalib as datalib
import grohoviz.plotlib as plotlib

lock_file = "sim.lock"
manifest_file = "manifest.yml"


class ChartMaker:
    def __init__(self, datadir: pathlib.Path, plotting_file: pathlib.Path):
        self.datadir = datadir
        self.plotting_file = plotting_file
        self.datadir_last_changed = 0
        self.plotting_file_last_changed = 0

        self.trajectories = None
        # self.ax = plt.subplot(111)
        self.atlas = plotlib.Atlas(self.plotting_file)

    def poll(self):
        datadir_last_changed = (self.datadir / manifest_file).stat().st_mtime
        plotting_file_last_changed = self.plotting_file.stat().st_mtime

        should_reload_data = datadir_last_changed > self.datadir_last_changed
        should_reload_plotfile = (
            plotting_file_last_changed > self.plotting_file_last_changed
        )
        should_replot = False

        if should_reload_data:
            self.reload_data()
            self.datadir_last_changed = datadir_last_changed
            should_replot = True

        if should_reload_plotfile:
            self.atlas.reload_plotfile()
            self.plotting_file_last_changed = plotting_file_last_changed
            should_replot = True

        if should_replot:
            self.replot()

    def reload_data(self):
        lock = FileLock(self.datadir / lock_file)
        with lock:
            self.trajectories = datalib.load_data(self.datadir)
            sys.stderr.write("Reloading data\n")

    def replot(self):
        self.atlas.plot(self.trajectories)
        sys.stderr.write("Replotting\n")

    # def chart(self, targets: List[int] = None):
    #     self.ax.cla()
    #     for k in self.trajectories.bodies():
    #         if targets is not None:
    #             if k not in targets:
    #                 continue
    #         p = self.trajectories.get(k)
    #         # p = self.trajectories.get(k, 301)
    #         # p = self.trajectories.get(k, 301, 3600)
    #         if p is not None:
    #             self.ax.plot(p.x, p.y, label=k)
    #     self.ax.set_aspect("equal")
