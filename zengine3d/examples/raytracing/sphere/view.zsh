#!/usr/local/bin/zsh

./sphere.bin > b.ppm && convert b.ppm b.png && open -a Preview b.png
