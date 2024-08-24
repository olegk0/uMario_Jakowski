#!/bin/bash

for filename in ./*.bmp; do
    echo "$filename   $(basename $filename .bmp)"
    convert $filename -transparent magenta   $(basename $filename .bmp).png
done