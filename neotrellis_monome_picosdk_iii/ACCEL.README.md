# Accelerometer and Magnetometer Usage

Hardware used:

- [Adafruit Triple-axis Magnetometer - LIS2MDL](https://www.adafruit.com/product/4488)
- [Adafruit LSM6DS3TR-C 6-DoF Accel + Gyro IMU](https://www.adafruit.com/product/4503)

Event threshold is configurable in `device.cpp`:
```cpp
#define ACCEL_CHANGE_THRESHOLD 0.05f // Minimum change to trigger event
```

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

- **Acceleration units**: g (1g ≈ 9.81 m/s²)
- **Orientation units**: Degrees (-180° to +180° for roll/pitch, 0° to 360° for yaw)


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

## Monome Protocol Integration

When running in monome mode, accelerometer data is sent as tilt events using the standard monome protocol. The device sends tilt events automatically when acceleration changes significantly.

### Monome Tilt Events

The device sends tilt events with the following format:
- **Command**: `0x81` (tilt event)
- **Sensor number**: `0` (first tilt sensor)
- **X axis**: 16-bit signed integer (high byte, low byte)
- **Y axis**: 16-bit signed integer (high byte, low byte)  
- **Z axis**: 16-bit signed integer (high byte, low byte)

### Data Format

Accelerometer values are scaled from ±2g float values to 16-bit signed integers:
- **Scale factor**: 16384 (2^14)
- **Range**: -32768 to +32767
- **Resolution**: ~0.00012g per unit

### Example Implementation

In a monome application, you would handle tilt events like this:

```lua
-- Example in Lua (for monome applications)
-- This would be implemented in a Lua-based monome app framework

function handle_tilt_event(sensor, x_high, x_low, y_high, y_low, z_high, z_low)
    -- Reconstruct 16-bit signed integers from high and low bytes
    local x = bit32.bor(bit32.lshift(x_high, 8), x_low)
    local y = bit32.bor(bit32.lshift(y_high, 8), y_low)
    local z = bit32.bor(bit32.lshift(z_high, 8), z_low)
    
    -- Handle sign extension for negative values
    if x > 32767 then x = x - 65536 end
    if y > 32767 then y = y - 65536 end
    if z > 32767 then z = z - 65536 end
    
    print('Tilt sensor ' .. sensor .. ' X:' .. x .. ' Y:' .. y .. ' Z:' .. z)
    
    -- Convert back to g units (divide by 16384)
    local x_g = x / 16384.0
    local y_g = y / 16384.0 
    local z_g = z / 16384.0
    
    print('Acceleration (g) - X:' .. x_g .. ' Y:' .. y_g .. ' Z:' .. z_g)
    
    -- Use the values for your application
    -- Example: control a parameter based on tilt
    if x_g > 0.5 then
        print('Tilted right')
        updateParameter('pan', x_g)
    elseif x_g < -0.5 then
        print('Tilted left')
        updateParameter('pan', x_g)
    end
    
    if y_g > 0.5 then
        print('Tilted forward')
        updateParameter('tilt', y_g)
    elseif y_g < -0.5 then
        print('Tilted backward')
        updateParameter('tilt', y_g)
    end
    
    -- Use Z for pressure or intensity
    local intensity = math.floor((z_g + 2.0) / 4.0 * 15)  -- Map ±2g to 0-15
    updateParameter('intensity', intensity)
end
```
