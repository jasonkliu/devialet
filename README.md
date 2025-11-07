# Devialet Speaker IR Controller

Production-ready M5StickC Plus2 remote control for Devialet Phantom speakers.

## Features

- **IR Remote**: Samsung TV remotes or custom IR remotes
- **Physical Buttons**: M5StickC Plus2 volume control
- **Auto-Discovery**: mDNS finds all Devialet speakers
- **Multi-Speaker**: Commands sent to all speakers simultaneously
- **Instant UI**: <50ms response with optimistic updates
- **Mute Toggle**: Press once to mute, twice to restore volume
- **Power Saving**: 5-second auto-sleep
- **Configurable**: Adjustable repeat rate and debug logging

## Display

```
L: 60%
R: 65%
```

Color-coded: Grey (0-34%), Orange (35-69%), Red (70-100%)

## Hardware

- [M5StickC Plus2](https://www.amazon.com/dp/B0F3XQ22XS) (~$35)
- [IR Receiver](https://www.amazon.com/dp/B087ZRD3LH) (~$5) - TL1838/VS1838B
- [Dupont Wires](https://www.amazon.com/dp/B01EV70C78) (~$5)
- TV Remote (Samsung supported)

**Wiring**: IR OUT→GPIO33, VCC→3.3V, GND→GND

## Setup

1. Install [VS Code](https://code.visualstudio.com/) + [PlatformIO](https://platformio.org/install/ide?install=vscode)
2. Copy `include/Secrets.h.example` → `include/Secrets.h` and add WiFi credentials
3. Connect device via USB
4. `Cmd+Shift+B` to build and upload

## Configuration

**Custom Remote**: Set `DEBUG_LOGGING 1` in `Logger.h`, monitor serial, note hex codes, add to `RawIR` namespace in `IRReceiver.h`

**Repeat Rate**: Edit `BUTTON_REPEAT_INTERVAL_MS` in `IRReceiver.h` (300ms=fast, 500ms=balanced, 1000ms=slow)

## Architecture

- Non-blocking: 300ms HTTP timeout per request
- Optimistic UI: instant display, verified after POST
- Throttled repeats: prevents rapid command spam
- Header-only: modular design

## License

MIT
