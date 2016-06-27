#!/usr/local/bin/zsh

./dialectric_materials.bin > b.ppm && convert b.ppm b.png && open -a Preview b.png
