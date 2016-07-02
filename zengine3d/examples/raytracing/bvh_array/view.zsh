#!/usr/local/bin/zsh

./bvh_array.bin > b.ppm && convert b.ppm b.png && open -a Preview b.png
