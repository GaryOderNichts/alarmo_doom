# Alarmo Doom
Doom for the Nintendo Alarmo.

## Building
- Make sure to clone the repo with all submodules.
- Enter the AES key and IV in the `key.py` file.
- Copy the shareware `doom1.wad` to the `data` directory.
- Run `make`.

## Usage
- Follow the building instructions above.
- Hold down the confirm, back and notification button on the Alarmo at the same time.
- While holding all three buttons down, plug in the USB cable to your PC.  
  The dial button on top of the Alarmo should light up red and a drive should appear on the PC.
- Copy the `a.bin` to the newly appeared drive.
- Copy the `MarkFile` to the drive.

The alarmo should disconnect from the PC and doom should run after a few seconds.

## Credits
- https://github.com/floppes/stm32doom for the doom port.
- https://github.com/ghidraninja/game-and-watch-doom for the ff from memory implementation.
- https://github.com/richgel999/miniz for compression.
