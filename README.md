# OneKnob

**One-Knob Compressor / Expander**

A dead-simple dynamics plugin with a single knob. Turn left for expansion, turn right for compression. Center position is bypass.

![Fletcher Audio](https://img.shields.io/badge/Fletcher-Audio-blue)
![macOS](https://img.shields.io/badge/macOS-AU%20%7C%20VST3-green)
![License](https://img.shields.io/badge/license-Proprietary-red)

## Features

- **Single Knob Control** - Left = Expand, Right = Compress, Center = Bypass
- **Smooth Transition** - Seamlessly blend between expansion and compression
- **Visual Feedback** - Color-coded glow shows current mode (green/pink)
- **Zero Latency** - Real-time processing with no delay
- **Colorful Samba-Inspired UI** - Vibrant carnival aesthetic

## How It Works

| Knob Position | Effect |
|---------------|--------|
| Full Left (-100%) | Maximum expansion - quiet gets quieter |
| Center (0%) | Bypass - no processing |
| Full Right (+100%) | Maximum compression - loud gets quieter |

## Installation

### macOS

1. Download the latest release
2. Copy `OneKnob.component` to `~/Library/Audio/Plug-Ins/Components/`
3. Copy `OneKnob.vst3` to `~/Library/Audio/Plug-Ins/VST3/`
4. Restart your DAW

## Quick Start

1. Insert OneKnob on a track or bus
2. Turn the knob right to compress peaks
3. Turn the knob left to expand dynamics
4. Use the center position for A/B comparison

## Building from Source

### Requirements

- [JUCE Framework](https://juce.com/) (v7.0+)
- Xcode 14+ (macOS)
- C++17 compatible compiler

### Build Steps

```bash
git clone https://github.com/ianfletcher314/oneknob.git
cd oneknob
cd Builds/MacOSX
xcodebuild -scheme "OneKnob - AU" -configuration Release build
```

## Documentation

See [USAGE.md](USAGE.md) for detailed usage instructions.

## Author

**Fletcher Audio**
- GitHub: [@ianfletcher314](https://github.com/ianfletcher314)

## License

Proprietary - All rights reserved.

---

*Part of the Fletcher Audio plugin suite*
