#!/usr/local/bin/zsh

./parallel.bin > b.ppm && convert b.ppm b.png && open -a Preview b.png
