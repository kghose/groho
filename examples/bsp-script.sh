#!/bin/bash
# Script to pull .bsp files from NASA
set -ex

planet_path="https://naif.jpl.nasa.gov/pub/naif/generic_kernels/spk/planets/"

satellite_path="https://naif.jpl.nasa.gov/pub/naif/generic_kernels/spk/satellites/"
files=(mar097.bsp jup310.bsp sat375.bsp nep081.bsp ura111.bsp plu055.bsp)

for f in ${files[@]} 
do
    curl -o ${f} ${satellite_path}${f}
done