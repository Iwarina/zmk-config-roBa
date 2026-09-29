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

Connect both halves by USB and double-tap reset on each to expose two
`XIAO-SENSE` bootloader drives. Identify which drive letter belongs to the left
and right half before running the script; both drives have the same label.
For example, if the left half is `P:` and the right half is `O:`:

```powershell
.\scripts\flash-roba.ps1 -Commit 5f1b3ec -LeftDrive P -RightDrive O
```

Add `-ResetSettings` only when the halves need their stored settings cleared.
The script flashes `settings_reset` to both halves first, then asks you to
double-tap reset on both halves again before flashing their normal images.
This erases Bluetooth bonds and the saved ZMK Studio keymap, so export anything
you need to keep first. Use `-ValidateOnly` to check the UF2 files without
touching the keyboard. `-Yes` skips the initial `FLASH` confirmation, but not
the physical bootloader step after a settings reset.
