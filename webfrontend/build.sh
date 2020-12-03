#!/bin/sh
echo Building mojo2600/pixelframe:build

docker build --build-arg NODE_ENV="production" -t mojo2600/pixelframe:build .

docker container create --name extract mojo2600/pixelframe:build
#find ../data -mindepth 1 ! -regex '^.*homie.*' -delete
docker container cp extract:/app/dist/. ./dist/
docker container rm -f extract
