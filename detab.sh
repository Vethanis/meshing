#!/bin/bash
find *h *.cpp *.glsl -type f -exec sed -i.orig 's/\t/    /g' {} +
