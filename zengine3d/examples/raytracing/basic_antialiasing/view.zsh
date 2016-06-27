#!/usr/local/bin/zsh

./basic_antialiasing.bin > b.ppm && convert b.ppm b.png && open -a Preview b.png
