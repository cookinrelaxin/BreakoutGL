#!/usr/local/bin/zsh

./background.bin > b.ppm && convert b.ppm b.png && open -a Preview b.png
