import sys

import yaml
import matplotlib.pyplot as plt

try:
    from yaml import CLoader as Loader
except ImportError:
    from yaml import Loader


class AxisProperties:
    def __init__(self):
        self._axis_properties = None

    def read_from(self, ax):
        if self._axis_properties is not None:
            self._axis_properties["lim"] = {
                "x": ax.get_xlim(),
                "y": ax.get_ylim(),
            }

    def write_to(self, ax):
        if self._axis_properties is not None:
            ax.set_xlim(self._axis_properties["lim"]["x"])
            ax.set_ylim(self._axis_properties["lim"]["y"])

    def restore_or_set(self, ax):
        if self._axis_properties is None:
            self._axis_properties = {}
            self.read_from(ax)
        else:
            self.write_to(ax)


class Chart:
    def __init__(self, fig, subplot, targets=None, ref=None, dt=None):
        self.fig = fig
        self.subplot = None
        self.ax = None
        self._axis_properties = AxisProperties()

        self.targets, self.ref, self.dt = targets, ref, dt
        self.update_description(subplot, targets, ref, dt)

    def __del__(self):
        self.ax.remove()

    def update_description(self, subplot, targets=None, ref=None, dt=None):
        self.subplot = subplot
        self.targets, self.ref, self.dt = targets, ref, dt

        if self.ax is not None:
            self._axis_properties.read_from(self.ax)
            self.ax.remove()

        self.ax = self.fig.add_subplot(self.subplot)
        self._axis_properties.write_to(self.ax)

    def replot(self, trajectories):
        self._axis_properties.read_from(self.ax)
        self.ax.cla()

        for k in trajectories.bodies():
            if self.targets is not None:
                if k not in self.targets:
                    continue
            p = trajectories.get(k, self.ref, self.dt)
            if p is not None:
                self.ax.plot(p.x, p.y, label=k)

        self.ax.set_aspect("equal")
        self._axis_properties.restore_or_set(self.ax)
        plt.draw()

    def reset(self):
        self.ax.autoscale()


# For now each chart will appear in the same figure
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

        self.fig.canvas.mpl_connect("button_press_event", self.reset)

    def replot(self, trajectories):
        for k, chart in self.charts.items():
            chart.replot(trajectories)
        plt.tight_layout()

    def reset(self, event):
        if event.dblclick:
            for k, chart in self.charts.items():
                if event.inaxes == chart.ax:
                    chart.reset()
                    break
