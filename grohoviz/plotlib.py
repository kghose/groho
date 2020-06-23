import sys

import yaml
import matplotlib.pyplot as plt

try:
    from yaml import CLoader as Loader
except ImportError:
    from yaml import Loader


class Chart:
    def __init__(self, fig, subplot, targets=None, ref=None, dt=None):
        self.fig = fig
        self.subplot, self.ax, self._axis_properties = None, None, None
        self.targets, self.ref, self.dt = targets, ref, dt
        self.update_description(subplot, targets, ref, dt)

    def __del__(self):
        self.ax.remove()

    def update_description(self, subplot, targets=None, ref=None, dt=None):
        self.subplot = subplot
        self.targets, self.ref, self.dt = targets, ref, dt

        if self.ax is not None:
            self.ax.remove()
        self.ax = self.fig.add_subplot(self.subplot)

    def replot(self, trajectories):
        if self._axis_properties is not None:
            self._axis_properties["lim"] = {
                "x": self.ax.get_xlim(),
                "y": self.ax.get_ylim(),
            }
        self.ax.cla()

        for k in trajectories.bodies():
            if self.targets is not None:
                if k not in self.targets:
                    continue
            p = trajectories.get(k, self.ref, self.dt)
            if p is not None:
                self.ax.plot(p.x, p.y, label=k)
        self.ax.set_aspect("equal")

        if self._axis_properties is None:
            self._axis_properties = {}
            self._axis_properties["lim"] = {
                "x": self.ax.get_xlim(),
                "y": self.ax.get_ylim(),
            }
        else:
            self.ax.set_xlim(self._axis_properties["lim"]["x"])
            self.ax.set_ylim(self._axis_properties["lim"]["y"])
        plt.draw()


# For now each chart will appear in the same figure, in the future we might
# be able to
# TODO: Cleverly retain figure and axes properties
# TODO: how to get Home button to zoom out to extents
class Atlas:
    def __init__(self, plotfile):
        self.plotfile = plotfile
        self.fig = None
        self.charts = {}

    def update_description(self):
        with open(self.plotfile, "r") as f:
            desc = yaml.load(f, Loader=Loader)

        if self.fig is None:
            self.fig = plt.figure(**desc.get("fig", {}))
        self.fig.set_size_inches(
            desc.get("fig", {}).get("figsize", [5.5, 2.8]), forward=True
        )

        for name, chart_desc in desc.get("panels", {}).items():
            if name in self.charts:
                self.charts[name].update_description(**chart_desc)
            else:
                self.charts[name] = Chart(fig=self.fig, **chart_desc)

        new_names = set(desc.get("panels", {}).keys())
        for name in list(self.charts.keys()):
            if name not in new_names:
                self.charts.pop(name)

    def replot(self, trajectories):
        for k, chart in self.charts.items():
            chart.replot(trajectories)
