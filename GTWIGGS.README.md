# Changes by gtwiggs

- Update Pico board configuration for my hardware implementation
	- Set board size: 16 &times; 8
	- Set I2C pins on bus `I2C0`, SDA: `GP4`, SCL: `GP5`.
		This matches the Pico board config in [multitrellis_test](multitrellis_test/README.md).
- Set default LED color to a warm white.
- Add `.vscode` directory to preserve configuration

## References

- [Raspberry Pi Pico pinout](https://pico.pinout.xyz)
- I used the __Raspberry Pi Pico__ VSCode extension __New Project from Example__ to generate an [i2c_bus_scan](https://github.com/raspberrypi/pico-examples/blob/master/i2c/bus_scan/bus_scan.c) project which was helpful in verifying the board addresses after soldering the address jumpers.