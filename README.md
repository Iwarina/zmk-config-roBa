# zmk-config-roBa

<img src="keymap-drawer/roBa.svg" >

## Trackball sensitivity

On the MOUSE layer, the left-hand W/S pair adjusts pointer speed and the E/D
pair adjusts ball scrolling speed. W and E increase sensitivity; S and D
decrease it. Each has five independent steps: 50%, 75%, 100%, 125%, and 150%.
The pointer starts at the 800-CPI sensor's 100% step. This is 80% of the
previous 1000-CPI pointer setting, not 80% of 800 CPI. The scroll starts at
100% of its current 1/16 setting. The encoder is unchanged. Adjustments reset
to these defaults when the keyboard reboots.

ZMK Studio stores key bindings on the keyboard. If the MOUSE layer was edited
there, flashing firmware alone may not replace its four saved bindings. Assign
the `Trackball Sensitivity` behavior to W/E/S/D in Studio with parameters
`Pointer, Faster` / `Scroll, Faster` / `Pointer, Slower` /
`Scroll, Slower`, respectively. This preserves the rest of the saved keymap.

## Flash both halves

The bootloader does not identify which half is connected. Put **only the named
half** into bootloader mode (connect by USB and double-tap reset), then run the
corresponding command. The script detects the single `XIAO-SENSE` drive and
rejects zero or multiple matching drives. No drive letters are needed:

```powershell
.\scripts\flash-roba.ps1 -Commit 5f1b3ec -Stage FlashLeft
.\scripts\flash-roba.ps1 -Commit 5f1b3ec -Stage FlashRight
```

To clear stored settings on both halves, first run these two stages, entering
bootloader mode on the named half before each command. Then re-enter the
bootloader on each half and run the two normal firmware commands above:

```powershell
.\scripts\flash-roba.ps1 -Commit 5f1b3ec -Stage ResetLeft -ResetSettings
.\scripts\flash-roba.ps1 -Commit 5f1b3ec -Stage ResetRight -ResetSettings
```

Settings reset erases Bluetooth bonds and the saved ZMK Studio keymap, so
export anything you need first. Re-pair Bluetooth after both normal images
are installed. Use `-ValidateOnly` to check UF2 files without touching the
keyboard. `-Yes` skips the `FLASH` prompt when you have already confirmed the
physical half. The old `-LeftDrive` and `-RightDrive` workflow remains
available when both bootloader drives are mounted at once.
