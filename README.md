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
