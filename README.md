# MPEG-TS Parser

A C++ implementation of an MPEG Transport Stream (TS) parsing only audio for now.

## Features

- **TS Packet Header Parsing**: Complete parsing of Transport Stream packet headers including sync byte, PID, continuity counter, and adaptation field control
- **Adaptation Field Support**: Full parsing of adaptation fields with PCR/OPCR timestamps and stuffing bytes
- **PES Packet Assembly**: Automatic reassembly of Packetized Elementary Stream (PES) packets from TS packets
- **Audio Stream Extraction**: Working audio extraction functionality - outputs `.mp2` files from specified PIDs

## Usage

```bash
./TS-PARSER input_file.ts
```

The parser will analyze the transport stream and extract audio data for PID 136 (configurable in source).

## Build

```bash
mkdir build
cd build
cmake ..
cmake --build .
```

Requires C++17 compiler and CMake 3.10+.

## Output

- Console output with detailed packet information
- Audio files saved as `PID{number}.mp2`

## Helpful sources about MPEG

For detailed information about MPEG Transport Stream format:
- [ITU-T H.222.0 Specification](https://www.itu.int/rec/T-REC-H.222.0-201410-S/en)
- [PES Header Format](https://dvd.sourceforge.net/dvdinfo/pes-hdr.html)

## Project Structure

- `tsTransportStream.h/cpp` - Core TS and PES parsing classes
- `tsCommon.h` - Common utilities and byte swapping functions
- `TS_parser.cpp` - Main application
- `CMakeLists.txt` - Build configuration
