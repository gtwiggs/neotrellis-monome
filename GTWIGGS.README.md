# Changes by gtwiggs

- Update pico board configuration for my hardware implementation
	- Set board size: 8 &mul; 16
	- Set I2C pins: `I2C0` SDA: `GP4` SCL: `GP5`
		This matches the config in [`multitrellis_test`](.\multitrellis_test). 
- Add `.vscode` directory to preserve configuration

## References

- [Raspberry Pi Pico pinout](https://pico.pinout.xyz)