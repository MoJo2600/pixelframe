SHELL := /bin/bash
.DEFAULT_GOAL := help

.PHONY: help
help:  ## help target to show available commands with information
	@grep -E '^[a-zA-Z_-]+:.*?## .*$$' $(MAKEFILE_LIST) |  awk 'BEGIN {FS = ":.*?## "}; {printf "\033[36m%-30s\033[0m %s\n", $$1, $$2}'

.PHONY: all
all: build-frontend build-firmware upload-firmware upload-fs

.PHONY: build-frontend
build-frontend: # build web frontend and prepare data directory for FS build
	cd web && ./build.sh && cd ..
	rm -Rf ./data/css
	rm -Rf ./data/js
	rm -Rf ./data/favicon.ico
	rm -Rf ./data/index.html
	cp -r  ./web/dist/* ./data/
	find ./data/ -name "*.css" -exec rm -rf {} \;
	find ./data/ -name "*.css.br" -exec rm -rf {} \;
	find ./data/ -name "*.js" -exec rm -rf {} \;
	find ./data/ -name "*.js.br" -exec rm -rf {} \;
	find ./data/ -name "*.map" -exec rm -rf {} \;
	find ./data/ -name "*.html.br" -exec rm -rf {} \;

## pio related
.PHONY: build-firmware
build-firmware:
	platformio run

.PHONY: upload-firmware
upload-firmware:
	platformio run -t upload

.PHONY: upload-fs
upload-fs:
	platformio run -t uploadfs
