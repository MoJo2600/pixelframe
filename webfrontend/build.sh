#!/bin/sh
echo Building mojo2600/pixelframe:build

docker build --build-arg NODE_ENV="production" -t mojo2600/pixelframe:build .

docker container create --name extract mojo2600/pixelframe:build
#find ../data -mindepth 1 ! -regex '^.*homie.*' -delete
rm -Rf ../data/*
docker container cp extract:/app/dist/. ../data/
docker container rm -f extract
