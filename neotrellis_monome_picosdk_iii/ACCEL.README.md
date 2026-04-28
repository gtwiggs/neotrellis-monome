# Accelerometer and Magnetometer Usage

Hardware used:

- [Adafruit Triple-axis Magnetometer - LIS2MDL](https://www.adafruit.com/product/4488)
- [Adafruit LSM6DS3TR-C 6-DoF Accel + Gyro IMU](https://www.adafruit.com/product/4503)

## Read Values

In the iii Lua scripting mode, you can use the `accel_read()` function to get the current accelerometer values. This function returns three floating-point numbers representing the acceleration in the X, Y, and Z axes (in units of g, where 1g ≈ 9.81 m/s²).

```lua
-- Read accelerometer values
local x, y, z = accel_read()

-- Print the values (will appear in the REPL/serial output)
print("Accel X:", x, "Y:", y, "Z:", z)
```

You can also use `orientation_read()` to get the calculated orientation angles (roll, pitch, yaw) in degrees:

```lua
-- Read orientation values
local roll, pitch, yaw = orientation_read()

-- Print the orientation
print("Roll:", roll, "Pitch:", pitch, "Yaw:", yaw)
```

**Note**: Yaw requires a magnetometer to be present. If no magnetometer is detected, yaw will be 0.0.

## Event Notification

The accelerometer is polled every 50ms, and when the acceleration changes by more than 0.05g in any axis, it triggers the `event_accel` callback with the current acceleration values and calculated orientation.

```lua
-- Handle accelerometer / magnetometer events
function event_accel(ax, ay, az, roll, pitch, yaw)
    -- This function is called automatically when acceleration changes
    print("Accel: X=" .. ax .. " Y=" .. ay .. " Z=" .. az)
    print("Orientation: Roll=" .. roll .. " Pitch=" .. pitch .. " Yaw=" .. yaw)
    
    -- Example: Light up LEDs based on tilt
    if roll > 15 then
        grid_led(0, 0, 15)  -- Tilted right
    elseif roll < -15 then
        grid_led(15, 0, 15) -- Tilted left
    end
    
    if pitch > 15 then
        grid_led(0, 7, 15)  -- Tilted forward
    elseif pitch < -15 then
        grid_led(15, 7, 15) -- Tilted backward
    end
    
    -- Example: Use yaw for compass-like behavior
    if yaw > 0 and yaw < 180 then
        grid_led(7, 0, 15)  -- Facing north-east
    end
end
```

```lua
-- Use grid as a compass!
function event_accel(ax, ay, az, roll, pitch, yaw)
    -- Create a compass rose effect
    local sector = math.floor(yaw / 45)  -- 8 sectors
    grid_led_all(0)  -- Clear all LEDs
    
    -- Light up LED based on compass direction
    if sector == 0 then grid_led(7, 0, 15)     -- North
    elseif sector == 1 then grid_led(15, 1, 15) -- North-East
    elseif sector == 2 then grid_led(15, 7, 15) -- East
    elseif sector == 3 then grid_led(15, 13, 15) -- South-East
    elseif sector == 4 then grid_led(7, 15, 15) -- South
    elseif sector == 5 then grid_led(0, 13, 15) -- South-West
    elseif sector == 6 then grid_led(0, 7, 15)  -- West
    elseif sector == 7 then grid_led(0, 1, 15)  -- North-West
    end
    
    grid_refresh()
end
```

## Key Features

- **Accelerometer**: ±2g range, 104 Hz output data rate
- **Gyroscope**: 250 dps range (used internally for orientation)
- **Magnetometer**: 100 Hz output data rate (for yaw/compass heading)
- **Orientation**: Roll, pitch calculated from accelerometer; yaw from magnetometer
- **Event-driven**: Events fire automatically on significant movement (0.05g threshold)
- **50ms polling**: Fast enough for responsive interactions

## Technical Details

- **Acceleration units**: g (1g ≈ 9.81 m/s²)
- **Orientation units**: Degrees (-180° to +180° for roll/pitch, 0° to 360° for yaw)
- **Event threshold**: 0.05g minimum change to trigger events
- **I2C addresses**: Accelerometer/Gyroscope (0x6A), Magnetometer (0x1E)

## Error Handling

Both `accel_read()` and `orientation_read()` will return `nil` if the sensors are not detected or fail to initialize:

```lua
local x, y, z = accel_read()
if x then
    print("Accelerometer working:", x, y, z)
else
    print("Accelerometer not available")
end

local roll, pitch, yaw = orientation_read()
if roll then
    print("Orientation available:", roll, pitch, yaw)
else
    print("Orientation not available")
end
```

## Key features:

- **Event-driven**: No need to constantly poll - events fire automatically on significant movement
- **Threshold-based**: Only triggers when acceleration changes by more than 0.05g to avoid noise
- **50ms polling**: Fast enough for responsive interactions
- **6 parameters**: ax, ay, az (acceleration) + roll, pitch, yaw (orientation)
- **Units**: Acceleration in g (±2g range), orientation in degrees

The event system works just like `event_grid` does for button presses - define the function and it gets called automatically when the accelerometer detects movement.

Event threshold is configurable in `device.cpp`:
```cpp
#define ACCEL_CHANGE_THRESHOLD 0.05f // Minimum change to trigger event
```
