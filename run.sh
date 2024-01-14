#!/bin/zsh
echo "Start Script!"
echo "Reloading!"
find . | entr -s ./build.sh