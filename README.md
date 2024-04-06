# KD 23MTS
Oscilloscope built with STML476RG and a 4" LCD with ili9488 driver.

## Project status: in construction.

## Project goals:
  - 2 channells
  - FFT
  - RMS, peak-to-peak, frequency measurements

## Connections

- LCD
  | STM pin | Component pin |
  |---------|---------------|
  | PA6     | TFT_MISO      |
  | PC9     | TFT_LED       |
  | PB3     | TFT_SCK       |
  | PA7     | TFT_MOSI      |
  | PC4     | TFT_DC/RS     |
  | PA10    | TFT_RESET     |
  | PC8     | TFT_CS        |


- Touchscreen
  | STM pin | Component pin |
  |---------|---------------|
  | PC2     | Touch_MISO    |
  | PC3     | Touch_MOSI    |
  | PB10    | Touch_SCK     |
  | PB12    | Touch_CS      |

- Rotary encoder
  | STM pin | Component pin |
  |---------|---------------|
  | PA9     | ENC_A         |
  | PA9     | ENC_B         |
  | PC0     | ENC_BTN       |

## Used tools: 
<img align="center" height="64" src="images/logos/STM32CubeIDE.png">

## Authors:
- Dominik Pluta
- Kamil Winnicki
