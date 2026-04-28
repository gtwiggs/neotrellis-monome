# Accelerometer Usage

## Read Values

In the iii Lua scripting mode, you can use the `accel_read()` function to get the current accelerometer values. This function returns three floating-point numbers representing the acceleration in the X, Y, and Z axes (in units of g, where 1g ≈ 9.81 m/s²).

Here's how to use it in a Lua script:

```lua
-- Read accelerometer values
local x, y, z = accel_read()

-- Print the values (will appear in the REPL/serial output)
print("Accel X:", x, "Y:", y, "Z:", z)

-- You can use these values for various purposes, such as:
-- - Detecting tilt/orientation
-- - Triggering events based on movement
-- - Creating interactive effects based on device motion

-- Example: Light up LEDs based on tilt
if x > 0.5 then
    grid_led(0, 0, 15)  -- Light LED if tilted right
elseif x < -0.5 then
    grid_led(15, 0, 15) -- Light LED if tilted left
end
```

The accelerometer is configured for ±2g range with 104 Hz output data rate. Values will be between approximately -2.0 and +2.0 (though in practice, you'll see values around -1.0 to +1.0 for normal device orientations).

Note that `accel_read()` will return `nil` if the accelerometer is not detected or fails to initialize during startup. You can check for this in your script:

```lua
local x, y, z = accel_read()
if x then
    -- Accelerometer is working
    print("Accel:", x, y, z)
else
    print("Accelerometer not available")
end
```

# Accelerometer Event Notification

## How it works:

The accelerometer is polled every 50ms, and when the acceleration changes by more than 0.05g in any axis, it triggers the `event_accel` callback with the current X, Y, Z values.

## Usage in Lua scripts:

```lua
-- Define the accelerometer event handler
function event_accel(x, y, z)
    -- This function is called automatically when acceleration changes
    print("Acceleration changed: X=" .. x .. " Y=" .. y .. " Z=" .. z)
    
    -- Example: Light up LEDs based on tilt
    if x > 0.3 then
        grid_led(0, 0, 15)  -- Tilt right
    elseif x < -0.3 then
        grid_led(15, 0, 15) -- Tilt left
    end
    
    if y > 0.3 then
        grid_led(0, 7, 15)  -- Tilt forward
    elseif y < -0.3 then
        grid_led(15, 7, 15) -- Tilt backward
    end
end

-- You can still manually read values anytime with:
local x, y, z = accel_read()
```

## Key features:

- **Event-driven**: No need to constantly poll - events fire automatically on significant movement
- **Threshold-based**: Only triggers when acceleration changes by more than 0.05g to avoid noise
- **50ms polling**: Fast enough for responsive interactions
- **Same units**: X, Y, Z values are in g units (±2g range)

The event system works just like `event_grid` for button presses - define the function and it gets called automatically when the accelerometer detects movement.

Event threshold is configurable in `device.cpp`: 
```cpp
#define ACCEL_CHANGE_THRESHOLD 0.05f // Minimum change to trigger event
```
