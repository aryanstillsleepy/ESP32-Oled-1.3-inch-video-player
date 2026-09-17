#include <Arduino.h>
#include <Wire.h>
#include <U8g2lib.h>
#include <FFat.h>

#define HEADER_SIZE 20

U8G2_SH1106_128X64_NONAME_F_HW_I2C oled(
  U8G2_R0,
  U8X8_PIN_NONE
);

File video;

// Video information read from video.bin
uint32_t frameCount;
uint32_t frameSize;
uint32_t fpsMilli;

float videoFPS;


// =====================================================
// READ 16-BIT VALUE
// =====================================================

uint16_t readUInt16(File &file) {

  uint8_t data[2];

  file.read(data, 2);

  return
    ((uint16_t)data[0]) |
    ((uint16_t)data[1] << 8);
}


// =====================================================
// READ 32-BIT VALUE
// =====================================================

uint32_t readUInt32(File &file) {

  uint8_t data[4];

  file.read(data, 4);

  return
    ((uint32_t)data[0]) |
    ((uint32_t)data[1] << 8) |
    ((uint32_t)data[2] << 16) |
    ((uint32_t)data[3] << 24);
}


// =====================================================
// SETUP
// =====================================================

void setup() {

  Serial.begin(115200);

  delay(1000);

  // OLED
  Wire.begin(8, 9);
  Wire.setClock(1000000);

  oled.begin();


  // ===================================================
  // MOUNT FFAT
  // ===================================================

  if (!FFat.begin()) {

    Serial.println("FFAT mount FAILED");

    while (true) {
      delay(1000);
    }
  }

  Serial.println("FFAT mounted!");


  // ===================================================
  // OPEN VIDEO
  // ===================================================

  video = FFat.open(
    "/video.bin",
    "r"
  );

  if (!video) {

    Serial.println("video.bin NOT FOUND");

    while (true) {
      delay(1000);
    }
  }

  Serial.println("video.bin opened!");


  // ===================================================
  // READ HEADER
  // ===================================================

  // Magic
  uint8_t magic[4];

  video.read(
    magic,
    4
  );

  if (
    magic[0] != 'O' ||
    magic[1] != 'L' ||
    magic[2] != 'E' ||
    magic[3] != 'D'
  ) {

    Serial.println("ERROR: Invalid video file");

    while (true) {
      delay(1000);
    }
  }


  // Width
  uint16_t width =
    readUInt16(video);


  // Height
  uint16_t height =
    readUInt16(video);


  // FPS * 1000
  fpsMilli =
    readUInt32(video);


  // Frame count
  frameCount =
    readUInt32(video);


  // Frame size
  frameSize =
    readUInt32(video);


  // Convert milli-FPS to FPS
  videoFPS =
    fpsMilli / 1000.0;


  // ===================================================
  // CHECK VIDEO
  // ===================================================

  Serial.println();
  Serial.println("========== VIDEO INFO ==========");

  Serial.print("Width: ");
  Serial.println(width);

  Serial.print("Height: ");
  Serial.println(height);

  Serial.print("FPS: ");
  Serial.println(videoFPS, 3);

  Serial.print("Frames: ");
  Serial.println(frameCount);

  Serial.print("Frame size: ");
  Serial.println(frameSize);

  Serial.print("File size: ");
  Serial.println(video.size());

  Serial.println("================================");
  Serial.println();


  // ===================================================
  // VALIDATE
  // ===================================================

  if (
    width != 128 ||
    height != 64 ||
    frameSize != 1024
  ) {

    Serial.println(
      "ERROR: Video format does not match OLED"
    );

    while (true) {
      delay(1000);
    }
  }


  if (frameCount == 0) {

    Serial.println(
      "ERROR: Video contains no frames"
    );

    while (true) {
      delay(1000);
    }
  }


  if (videoFPS <= 0) {

    Serial.println(
      "ERROR: Invalid FPS"
    );

    while (true) {
      delay(1000);
    }
  }


  // ===================================================
  // READY
  // ===================================================

  Serial.println("VIDEO READY!");

}


// =====================================================
// PLAY VIDEO
// =====================================================

void loop() {

  // Make sure we start exactly
  // at the first frame.

  video.seek(
    HEADER_SIZE
  );


  // ===================================================
  // FRAME TIMING
  // ===================================================

  // Convert FPS into milliseconds/frame.
  //
  // Example:
  //
  // 30 FPS = 33.333 ms
  // 15 FPS = 66.667 ms
  // 29.97 FPS = 33.367 ms

  float frameDelay =
    1000.0 / videoFPS;


  // ===================================================
  // PLAY ALL FRAMES
  // ===================================================

  for (
    uint32_t frame = 0;
    frame < frameCount;
    frame++
  ) {

    unsigned long start =
      millis();


    // -------------------------------------------------
    // Read frame
    // -------------------------------------------------

    size_t bytesRead =
      video.read(
        oled.getBufferPtr(),
        frameSize
      );


    // -------------------------------------------------
    // Check read
    // -------------------------------------------------

    if (
      bytesRead != frameSize
    ) {

      Serial.println(
        "ERROR: Frame read failed"
      );

      video.seek(
        HEADER_SIZE
      );

      break;
    }


    // -------------------------------------------------
    // Display frame
    // -------------------------------------------------

    oled.sendBuffer();


    // -------------------------------------------------
    // Maintain FPS
    // -------------------------------------------------

    unsigned long elapsed =
      millis() - start;


    if (
      elapsed < frameDelay
    ) {

      delay(
        (unsigned long)(
          frameDelay - elapsed
        )
      );
    }
  }


  // Video finished.
  // Loop automatically starts it again.
}
