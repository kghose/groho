import sys
import datetime

from_ts = datetime.datetime.utcfromtimestamp
OFFSET = datetime.datetime(2000, 1, 1, 12) - datetime.datetime(1970, 1, 1)

import yaml
import matplotlib.pyplot as plt
from matplotlib.widgets import Slider
import numpy as np

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
    def __init__(self, fig, subplot, plane="xy", targets=None, ref=None, dt=None):
        self.fig = fig
        self.subplot = None
        self.plane = None
        self.ax = None
        self._axis_properties = AxisProperties()

        self.targets, self.ref, self.dt = targets, ref, dt
        self.update_description(subplot, plane, targets, ref, dt)

    def __del__(self):
        self.ax.remove()

    def update_description(self, subplot, plane="xy", targets=None, ref=None, dt=None):
        self.subplot = subplot
        self.plane = plane
        self.targets, self.ref, self.dt = targets, ref, dt

        if self.ax is not None:
            self._axis_properties.read_from(self.ax)
            self.ax.remove()

        self.ax = self.fig.add_subplot(self.subplot)
        self.ax.get_xaxis().set_visible(False)
        self.ax.get_yaxis().set_visible(False)
        self._axis_properties.write_to(self.ax)

    def replot(self, trajectories, bodies, t):
        self._axis_properties.read_from(self.ax)
        self.ax.cla()

        for k in trajectories.bodies():
            if self.targets is not None:
                if k not in self.targets:
                    continue
            p = trajectories.get(k, self.ref, self.dt)
            if p is not None:
                n1 = max(0, np.searchsorted(p.t, t))
                # n0 = max(0, np.searchsorted(p.t, t - 360000))
                n0 = max(0, n1 - 1000)
                if self.plane == "xy":
                    x, y = p.x, p.y
                else:
                    x, y = p.x, p.z

                self.ax.plot(x[n0:n1], y[n0:n1], ls="none", marker="s", ms=1)
                self.ax.text(x[n1], y[n1], f"{k}", c="0.75", size=9)
                self.ax.plot(x[n1], y[n1], ".")
                # self.ax.plot(p.x[n1:], p.y[n1:], alpha=0.1)

        if self.ref is not None:
            center_body = bodies.get("bodies", {}).get(self.ref, {})
            self.ax.add_artist(
                plt.Circle(
                    (0, 0), center_body.get("r", 0), ec="k", ls="--", color="none"
                )
            )
            self.ax.text(
                0,
                0,
                center_body.get("name", f"{self.ref}"),
                c="k",
                size=9,
                ha="center",
                va="center",
            )

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
        self.bodies = {}

        self.t_slider = None
        self.t_frac = 1.0

    def update_data(self, trajectories, bodies):
        self.trajectories = trajectories
        self.bodies = bodies

    def update_description(self, desc):
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
            valstep = 1.0 / self.trajectories.get_t_span()
            ax_slider = plt.axes([0.1, 0.0, 0.7, 0.028], facecolor="0.35")
            self.t_slider = Slider(
                ax_slider, "Time", 0.0, 1.0, valinit=1.0, valstep=valstep, fc="yellow"
            )
            self.t_slider.label.set_size(7)
            self.t_slider.valtext.set_size(7)
            self.t_slider.on_changed(self.replot)

    def replot(self, t_frac=None):
        self.t_frac = t_frac or self.t_frac
        t = self.trajectories.get_t(self.t_frac)
        for k, chart in self.charts.items():
            chart.replot(self.trajectories, self.bodies, t)

        ts = from_ts(t) + OFFSET
        self.t_slider.valtext.set_text(ts)

    def reset(self, event):
        if event.dblclick:
            for k, chart in self.charts.items():
                if event.inaxes == chart.ax:
                    chart.reset()
                    break
