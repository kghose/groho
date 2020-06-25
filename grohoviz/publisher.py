import pathlib
import argparse
import sys
from typing import List

from filelock import FileLock

import matplotlib.pyplot as plt
import matplotlib as mpl
import matplotlib.animation as animation

import grohoviz.datalib as datalib
import grohoviz.plotlib as plotlib

lock_file = "sim.lock"
manifest_file = "manifest.yml"


class Publisher:
    def __init__(self, datadir: pathlib.Path, plotting_file: pathlib.Path):
        self.datadir = datadir
        self.plotting_file = plotting_file
        self.datadir_last_changed = 0
        self.plotting_file_last_changed = 0

        self.trajectories = None
        self.atlas = plotlib.Atlas(self.plotting_file)
        self._animator = None
        self.poll()

    def run(self):
        self._animator = animation.FuncAnimation(
            self.atlas.fig, self.poll, interval=10, blit=False, cache_frame_data=False
        )
        plt.show()

    def poll(self, frameNum=None):
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
            self.atlas.update_description()
            self.plotting_file_last_changed = plotting_file_last_changed
            should_replot = True

        if should_replot:
            self.replot()

    def reload_data(self):
        lock = FileLock(self.datadir / lock_file)
        with lock:
            self.atlas.update_data(datalib.load_data(self.datadir))
            sys.stderr.write("Reloading data\n")

    def replot(self):
        self.atlas.replot()
        sys.stderr.write("Replotting\n")
