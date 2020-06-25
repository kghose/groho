import sys

import yaml
import matplotlib.pyplot as plt
from matplotlib.widgets import Slider

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
        self.ax.get_xaxis().set_visible(False)
        self.ax.get_yaxis().set_visible(False)
        self._axis_properties.write_to(self.ax)

    def replot(self, trajectories, t):
        self._axis_properties.read_from(self.ax)
        self.ax.cla()

        for k in trajectories.bodies():
            if self.targets is not None:
                if k not in self.targets:
                    continue
            p = trajectories.get(k, self.ref, self.dt)
            if p is not None:
                n1 = max(0, int(p.x.size * t) - 1)

                self.ax.plot(p.x[:n1], p.y[:n1])
                self.ax.text(p.x[n1], p.y[n1], f"{k}", c="0.75", size=9)
                self.ax.plot(p.x[n1:], p.y[n1:], alpha=0.1)

        self.ax.axhline(0, c="0.75", ls=":")
        self.ax.axvline(0, c="0.75", ls=":")
        self.ax.set_aspect("equal", adjustable="datalim")
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

        self.trajectories = None

        self.t_slider = None
        self.t = 1.0

    def update_data(self, trajectories):
        self.trajectories = trajectories

    def update_description(self):
        with open(self.plotfile, "r") as f:
            desc = yaml.load(f, Loader=Loader)

        if self.fig is None:
            self.fig = plt.figure(**desc.get("fig", {}))
            self.fig.canvas.mpl_connect("button_press_event", self.reset)

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

        plt.subplots_adjust(
            top=1, bottom=0.03, right=1, left=0, wspace=0.01, hspace=0.01
        )

        if self.t_slider is None:
            ax_slider = plt.axes([0.0, 0.0, 1.0, 0.03], facecolor="0.35")
            self.t_slider = Slider(
                ax_slider, "Time", 0.0, 1.0, valinit=1.0, valstep=1e-4, fc="yellow"
            )
            self.t_slider.on_changed(self.replot)

    def replot(self, t=None):
        self.t = t or self.t
        for k, chart in self.charts.items():
            chart.replot(self.trajectories, self.t)

    def reset(self, event):
        if event.dblclick:
            for k, chart in self.charts.items():
                if event.inaxes == chart.ax:
                    chart.reset()
                    break
