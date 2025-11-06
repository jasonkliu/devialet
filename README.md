# Devialet Speaker IR Controller

M5StickC Plus2 remote control for Devialet Phantom speakers via IR and WiFi.

## Features

- **IR Remote Control**: LG TV remote compatibility (volume up/down, mute)
- **Physical Buttons**: Volume control via M5StickC buttons
- **Auto-Discovery**: Finds Devialet speakers via mDNS at startup
- **Synchronized Control**: Uses lowest speaker volume to keep speakers in sync
- **Instant Feedback**: Color-coded volume display updates immediately
- **No Polling**: Only queries speakers at startup to avoid spamming requests

## Display

Large, easy-to-read format showing speaker volumes:

```
L: 50%
R: 55%

(192.168.1.10, 192.168.1.11)
```

- **L**: Left speaker
- **R**: Right speaker  
- **M**: Mono speaker
- **Colors**: Grey (0-34%), Orange (35-69%), Red (70-100%)
- **IPs**: Debug info in small grey text at bottom

## Hardware

### Required Parts

- **[M5StickC Plus2](https://www.amazon.com/dp/B0F3XQ22XS)** - ESP32 IoT controller with display, IR, and WiFi
- **[IR Receiver Module](https://www.amazon.com/dp/B087ZRD3LH)** - KY-022/TL1838/VS1838B universal IR sensor
- **[Dupont Wires](https://www.amazon.com/dp/B01EV70C78)** - Male-to-female jumper cables for connections
- **LG TV Remote** (or compatible IR remote)

### Wiring

Connect IR receiver to M5StickC Plus2:
- IR OUT → GPIO 26
- IR VCC → 3.3V
- IR GND → GND

## Setup

1. Clone the repository
2. Copy `include/Secrets.h.example` to `include/Secrets.h`
3. Edit `include/Secrets.h` and add your WiFi credentials
4. Build and upload with PlatformIO: `pio run -t upload`

**Note:** `Secrets.h` is gitignored to protect your credentials.

## Controls

- **Button A**: Volume Up (+5%)
- **Button B**: Volume Down (-5%)
- **IR Remote**: Volume Up/Down/Mute

## Code Structure

```
include/
  ButtonHandler.h  - Input handling
  Connectivity.h   - WiFi & mDNS
  DevialetAPI.h    - HTTP API client
  Display.h        - Screen rendering
  IRReceiver.h     - IR decoding
  Logger.h         - Serial logging
  Speaker.h        - Speaker management
src/
  main.cpp         - Main program
```

Clean, minimal, header-only architecture with strong encapsulation.

## License

MIT
