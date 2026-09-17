# Oled-1.3-inch-video-player
Video convertor and player for Oled 1.3 inch screen (16:9) aspect ratio

# ESP32 OLED Video Player

A video player built around an ESP32-S3 and a 128x64 SH1106 OLED display.

The project converts normal video files into a compact binary format and plays them directly from the ESP32's FFAT filesystem.

## Hardware

- ESP32-S3
- 128x64 SH1106 OLED
- I2C connection
- Arduino framework

## How It Works

The project has two main parts:

### 1. Video Converter

The Python converter takes a normal video file and converts it into `video.bin`.


video.mp4
    ↓
Python Video Converter
    ↓
video.bin

The converter:

-Resizes the video to 128x64
-Crops the video to the correct aspect ratio
-Converts frames to grayscale
-Applies Floyd-Steinberg dithering
-Converts each frame into OLED-compatible bytes
-Preserves the video's original FPS
-Stores the number of frames in the file header

# # # 2. ESP32 Video Player

The ESP32 reads video.bin from its FFAT filesystem and sends each frame to the SH1106 OLED.

video.bin
    ↓
ESP32 FFAT
    ↓
Frame data
    ↓
SH1106 OLED

Video Format

The current format uses:

| Property        | Value                |
| --------------- | -------------------- |
| OLED resolution | 128x64               |
| Frame size      | 1024 bytes           |
| Header size     | 20 bytes             |
| FPS             | Stored in video file |
| Frame count     | Stored in video file |
| Storage         | FFAT                 |

Each frame requires:
128 × 64 ÷ 8 = 1024 bytes

## Project Structure

### `converter/`

Contains the Python video conversion tool.

- `video_converter.py` — converts video files into `video.bin`
- `requirements.txt` — Python dependencies

### `ESP32_Player/`

Contains the Arduino ESP32 video player.

- `ESP32_Player.ino` — ESP32 playback firmware
- `data/` — contains `video.bin` before uploading the FFAT filesystem

### Root Files

- `README.md` — project documentation
- `LICENSE` — MIT License
- `.gitignore` — prevents generated video files from being uploaded

  # # # Creating a Video
Place the source video next to the Python converter:

### Creating a Video

1. Place the source video in the `converter/` folder.
2. Make sure the file is named `video.mp4`.
3. Run `video_converter.py`.
4. The converter creates `video.bin`.
5. Move `video.bin` into the `ESP32_Player/data/` folder.
6. Build and upload the FFAT filesystem to the ESP32.

The generated `video.bin` is not stored in the GitHub repository.

# # # Run
Bash:
'python video_converter.py'

The converter will create:

The generated video.bin should then be placed inside:

ESP32_Player/data/

# # # Uploading the Video

The ESP32 uses a separate FFAT filesystem upload.

After placing video.bin in the data folder:

Build the FFAT filesystem.
Upload the FFAT filesystem to the ESP32.
Upload the ESP32 player firmware when the player code changes.

The video does not need to be compiled into the firmware.

# # # Changing Videos

To play a different video:

video.mp4
    ↓
Run video_converter.py
    ↓
New video.bin
    ↓
ESP32_Player/data/video.bin
    ↓
Build FFAT
    ↓
Upload FFAT

# # # Current Tested Configuration

The system has been tested with:

ESP32-S3
128x64 SH1106 OLED
30 FPS video
7000 frames
Approximately 152 seconds of video
7 MB video.bin

The full 7000 -frame video was successfully converted, uploaded to FFAT, detected by the ESP32, and played back. Full Limitations are on the available FFAT storage.
