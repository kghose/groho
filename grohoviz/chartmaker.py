import pathlib
import argparse
import sys
from typing import List

from watchdog.observers import Observer
from filelock import FileLock

import matplotlib.pyplot as plt
import matplotlib as mpl
import grohoviz.datalib as datalib

lock_file = "sim.lock"
manifest_file = "manifest.yml"


class EventHandler:
    def __init__(self, path: pathlib.Path, callback):
        self.path = path
        self.callback = callback

    def dispatch(self, event):
        if self.path.is_dir() or (event.src_path == str(self.path)):
            self.callback()


class FileObserver:
    def __init__(self, path: pathlib.Path, callback):
        self.path = path
        self.callback = callback
        self.observer = Observer()
        self.eh = EventHandler(path, callback)
        self.watcher = self.observer.schedule(
            self.eh, str(self.path.parent) if self.path.is_file() else str(self.path)
        )

    def start(self):
        self.observer.start()

    def stop(self):
        self.observer.stop()
        self.observer.join()


class ChartMaker:
    def __init__(self, datadir: pathlib.Path, plotting_file: pathlib.Path):
        self.datadir = datadir
        self.plotting_file = plotting_file
        self.trajectories = None
        self.ax = plt.subplot(111)
        self.reload_data()
        self.data_observer = FileObserver(datadir / manifest_file, self.reload_data)
        self.plot_observer = FileObserver(plotting_file, self.replot)

    def start(self):
        self.data_observer.start()
        self.plot_observer.start()

    def stop(self):
        self.data_observer.stop()
        self.plot_observer.stop()

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
