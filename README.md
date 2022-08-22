# COFFEE ROASTER AUTOMATION SYSTEM WIRING

## OLED 1.8" TFT Display
1. VCC (BLUE)   -> 5V
2. GND (GREEN)  -> GND
3. CS (YELLOW)  -> General Pin (D) 44
4. RST (ORANGE) -> General Pin (D) 26
5. A0 (RED)     -> General Pin (D) 45
6. SDA (BROWN)  -> 52
7. SCK (BLACK)  -> 51
8. LED (WHITE)  -> 3.3V

## Keypad
1. 28, 30, 32, 34 -> ROWS
2. 36, 38, 40, 42 -> COLUMS

## MLX90614 - Thermal Sensor
1. SCL  -> SCL
2. SDA  -> SDA
3. VCC  -> 3.3V
4. GND  -> GND

## RELAY - Solenoid Valve
1. Signal   -> 22
2. VCC      -> 5V
3. GND      -> GND

## BUZZER
1. Signal   -> 24
2. VCC      -> 5V
3. GND      -> GND

## Interrupt Button
1. Signal   -> 2
2. GND      -> GND