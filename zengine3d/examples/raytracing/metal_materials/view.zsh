#!/usr/local/bin/zsh

./metal_materials.bin > b.ppm && convert b.ppm b.png && open -a Preview b.png
