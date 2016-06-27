#!/usr/local/bin/zsh

./diffuse_materials.bin > b.ppm && convert b.ppm b.png && open -a Preview b.png
