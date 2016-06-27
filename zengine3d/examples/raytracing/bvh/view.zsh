#!/usr/local/bin/zsh

./balls.bin > b.ppm && convert b.ppm b.png && open -a Preview b.png
