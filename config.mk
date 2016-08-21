ESP_ROOT = /Users/cezarsa/code/esp/esp8266/
UPLOAD_PORT = /dev/cu.wchusbserial1410
BOARD = nodemcu
INCLUDE_VARIANT = nodemcu
UPLOAD_SPEED = 230400

clang_complete:
	echo $(CPP_COM) | tr " " "\n" | egrep "^-D|^-I" | sort > .clang_complete

static:
	scp -r static/* pi.casa:/var/www/html/blink/

.PHONY: $(.PHONY) static
