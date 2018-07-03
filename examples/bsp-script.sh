#!/bin/bash
# Script to pull .bsp files from NASA
set -e

download() {
    local path=$1
    shift
    local files=($@)
    for f in ${files[@]} 
    do
        if [ ! -f ${f} ]; then
            curl -o ${f} ${path}${f}
        else
            echo "${f} present"
        fi
    done
}

path="https://naif.jpl.nasa.gov/pub/naif/generic_kernels/spk/planets/"
files=(de430.bsp)
download ${path} ${files[@]}

path="https://naif.jpl.nasa.gov/pub/naif/generic_kernels/spk/satellites/"
files=(mar097.bsp jup310.bsp sat375.bsp nep081.bsp ura111.bsp plu055.bsp)
download ${path} ${files[@]}

path="ftp://ssd.jpl.nasa.gov/pub/eph/small_bodies/asteroids_de430/"
files=(ast343de430.bsp)
download ${path} ${files[@]}