#!/usr/local/bin/zsh

./multiple_spheres.bin > b.ppm && convert b.ppm b.png && open -a Preview b.png
