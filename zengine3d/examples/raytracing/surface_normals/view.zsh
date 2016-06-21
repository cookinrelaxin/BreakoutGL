#!/usr/local/bin/zsh

./surface_normals.bin > b.ppm && convert b.ppm b.png && open -a Preview b.png
