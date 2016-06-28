#!/usr/local/bin/zsh

./bvh.bin > b.ppm && convert b.ppm b.png && open -a Preview b.png
