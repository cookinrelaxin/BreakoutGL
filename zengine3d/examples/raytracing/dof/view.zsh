#!/usr/local/bin/zsh

./dof.bin > b.ppm && convert b.ppm b.png && open -a Preview b.png
