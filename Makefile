run:
	@echo "\033[1;32mCompiling...\033[0m"
	@arduino-cli compile --fqbn arduino:avr:nano .
	@echo "\033[1;32mUploading...\033[0m"
	@arduino-cli upload -p /dev/cu.usbserial-A5069RR4  --fqbn arduino:avr:nano .
	@echo "\033[1;32mMonitoring...\033[0m"
	@arduino-cli monitor -p /dev/cu.usbserial-A5069RR4 -b arduino:avr:nano -c 31250