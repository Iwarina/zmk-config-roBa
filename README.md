# zmk-config-roBa

<img src="keymap-drawer/roBa.svg" >

## Trackball sensitivity

On the MOUSE layer, the left-hand W/S pair adjusts pointer speed and the E/D
pair adjusts ball scrolling speed. W and E increase sensitivity; S and D
decrease it. Each has five independent steps: 50%, 75%, 100%, 125%, and 150%.
Both start at 100% of the current 800-CPI pointer and 1/16 scroll settings.
The encoder is unchanged. Adjustments reset to 100% when the keyboard reboots.

ZMK Studio stores key bindings on the keyboard. If the MOUSE layer was edited
there, flashing firmware alone may not replace its four saved bindings. Assign
the `Trackball Sensitivity` behavior to W/E/S/D in Studio with parameters
`Pointer, Faster` / `Scroll, Faster` / `Pointer, Slower` /
`Scroll, Slower`, respectively. This preserves the rest of the saved keymap.
