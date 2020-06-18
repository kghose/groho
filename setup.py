import pathlib
from datetime import datetime
from setuptools import setup, find_packages

name = "grohoviz"
version = "2020.06.16"
now = datetime.utcnow()
desc_path = pathlib.Path("Readme.md")
long_description = desc_path.open("r").read()

setup(
    name=name,
    version=version,
    packages=[name],
    entry_points={"console_scripts": [f"grohoviz = {name}.main:main"]},
    platforms=["POSIX", "MacOS", "Windows"],
    python_requires=">=3.7.0",
    install_requires=["matplotlib>=3.1.1", "filelock", "watchdog"],
    author="Kaushik Ghose",
    maintainer="Kaushik Ghose",
    maintainer_email="kaushik.ghose@gmail.com",
    author_email="kaushik.ghose@gmail.com",
    description="Program to plot charts for groho simulations",
    url="https://github.com/kghose/groho",
    long_description=long_description,
    long_description_content_type="text/markdown",
    include_package_data=True,
    license="Copyright (c) {} Kaushik Ghose".format(now.year),
    classifiers=[
        "Development Status :: 5 - Production/Stable",
        "Intended Audience :: Developers",
        "License :: OSI Approved :: MIT",
        "Natural Language :: English",
        "Operating System :: POSIX",
        "Programming Language :: Python :: 3.7",
        "Programming Language :: Python :: 3 :: Only",
    ],
    keywords="space physics-simulation",
)
