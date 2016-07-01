#!/usr/local/bin/zsh

./cl_gl.bin > b.ppm && convert b.ppm b.png && open -a Preview b.png
