import sys

import matplotlib.pyplot as plt


# For now no insets
class Chart:
    def __init__(self, targets=None, ref=None, dt=None):
        self.fig = None
        self.ax = None
        self.targets, self.ref, self.dt = targets, ref, dt

    def plot(self, trajectories):
        lim_x, lim_y = None, None

        if self.ax is None:
            self.fig = plt.figure()
            self.ax = plt.subplot(111)
        else:
            lim_x, lim_y = self.ax.get_xlim(), self.ax.get_ylim()

        self.ax.cla()
        for k in trajectories.bodies():
            if self.targets is not None:
                if k not in self.targets:
                    continue
            p = trajectories.get(k, self.ref, self.dt)
            if p is not None:
                self.ax.plot(p.x, p.y, label=k)
        self.ax.set_aspect("equal")
        if lim_x is not None:
            self.ax.set_xlim(lim_x)
            self.ax.set_ylim(lim_y)
        plt.draw()


# TODO: retain axes if axes name is repeated
# TODO: how to get Home button to zoom out to extents
class Atlas:
    def __init__(self, plotfile):
        self.plotfile = plotfile
        self.charts = {name: chart for name, chart in parse_plotfile(plotfile)}

    def reload_plotfile(self):
        self.charts = {name: chart for name, chart in parse_plotfile(self.plotfile)}

    def plot(self, trajectories):
        for k, chart in self.charts.items():
            chart.plot(trajectories)


# Line is
# name, ref, dt, targets ....
def parse_plotfile(plotfile):
    for line in open(plotfile, "r").readlines():
        if line.startswith("#"):
            continue
        name, targets, ref, dt = None, None, None, None
        parts = line.strip().split(",")
        if len(parts) < 4:
            continue
        name = parts[0] or "main"
        ref = int(parts[1]) if parts[1] != "" else None
        dt = float(parts[2]) if parts[2] != "" else None
        targets = [int(p) for p in parts[3:] if p != ""] or None
        sys.stderr.write(f"{name}, {ref}, {dt}, {targets}\n")
        yield name, Chart(targets=targets, ref=ref, dt=dt)
