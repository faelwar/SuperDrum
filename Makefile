run:
	arduino-cli compile --fqbn arduino:avr:nano .
	arduino-cli upload -p /dev/cu.usbserial-A5069RR4  --fqbn arduino:avr:nano .
	arduino-cli monitor -p /dev/cu.usbserial-A5069RR4 -b arduino:avr:nano -c 31250