IDF_TARGET ?= esp32c6
IDF_PATH ?= $(HOME)/esp/esp-idf-v5.5.3
IDF_COMMAND = . "$(IDF_PATH)/export.sh" >/dev/null && idf.py
PORT ?=
BAUD ?= 115200

.PHONY: build flash monitor flash-monitor clean set-target

build:
	$(IDF_COMMAND) build

flash:
	@test -n "$(PORT)" || (echo 'Indica el puerto serie: make flash PORT=/dev/cu.usbmodemXXXX' >&2; exit 2)
	$(IDF_COMMAND) -p "$(PORT)" -b $(BAUD) flash

monitor:
	@test -n "$(PORT)" || (echo 'Indica el puerto serie: make monitor PORT=/dev/cu.usbmodemXXXX' >&2; exit 2)
	$(IDF_COMMAND) -p "$(PORT)" -b $(BAUD) monitor

flash-monitor:
	@test -n "$(PORT)" || (echo 'Indica el puerto serie: make flash-monitor PORT=/dev/cu.usbmodemXXXX' >&2; exit 2)
	$(IDF_COMMAND) -p "$(PORT)" -b $(BAUD) flash monitor

clean:
	$(IDF_COMMAND) fullclean

set-target:
	$(IDF_COMMAND) set-target $(IDF_TARGET)
