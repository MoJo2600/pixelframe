SHELL := /bin/bash
.DEFAULT_GOAL := help

.PHONY: help
help:  ## help target to show available commands with information
	@grep -E '^[a-zA-Z_-]+:.*?## .*$$' $(MAKEFILE_LIST) |  awk 'BEGIN {FS = ":.*?## "}; {printf "\033[36m%-30s\033[0m %s\n", $$1, $$2}'

.PHONY: all
all: build-frontend build-firmware upload-firmware upload-fs

.PHONY: build-frontend
build-frontend:
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

# .PHONY: start-frontend
# start-frontend:  ## start backend
# 	docker-compose up

# # data/gifs64/* data/www/*
# img3.ffat: data/*
# 	# Get from https://github.com/marcmerlin/esp32_fatfsimage/
# 	# noota_3gffat.csv 0x2F0000 is 3008KB
# 	fatfsimage img3.ffat 3008 data/

# # data/gifs64/* data/www/*
# img12.ffat: data/*
# 	# Get from https://github.com/marcmerlin/esp32_fatfsimage/
# 	# ffat.csv 0xBEF000 is 12220KB
# 	fatfsimage img12.ffat 12220 data/

# flash: flash3

# flash3: img3.ffat
# 	# IF you aren't using a 1/3GB split, 0x110000 will need to be updated
# 	for baud in 921600 460800; do esptool.py --chip esp32 --port /dev/ttyUSB0 --baud $$baud write_flash 0x110000 img3.ffat  && exit; done

# flash12: img12.ffat
# 	# offset comes from espressif/esp32/tools/partitions/ffat.csv defined in espressif/esp32/boards.txt
# 	for baud in 921600 460800; do esptool.py --chip esp32 --port /dev/ttyUSB0 --baud $$baud write_flash 0x410000 img12.ffat && exit; done

