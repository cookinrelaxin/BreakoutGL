#!/usr/local/bin/zsh

./positionable_camera.bin > b.ppm && convert b.ppm b.png && open -a Preview b.png
