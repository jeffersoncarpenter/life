# life.exe is a keyboard-controlled Game of Life simulator.

## Interactive commands

Interactive commands cause a visible effect as soon as you press a
key.

Additionally, almost all interactive commands can be prefixed with a
number, in which case they will repeat that many times.  For instance

    50 <left arrow>

will move the selected square 50 squares to the left and

    100 <Enter>

will step the game forward 100 steps.

The `Escape` key can be used to cancel a numeric prefix argument
entirely, and `Backspace` can be used to delete the most
recently-added number.

### Motion

Left Arrow: Move left.
Right Arrow: Move right.
Up Arrow: Move up.
Down Arrow: Move down.

### Editing

Space: Toggle selected square.

### Selecting Regions

Selected regions can be used with extended commands.  The selected
region can be abandoned by pressing the `Escape` key.  (A region of
one square is still considered a region.)

Shift+Left Arrow: Select region leftward.
Shift+Right Arrow: Select region rightward.
Shift+Up Arrow: Select region upward.
Shift+Down Arrow: Select region downward.

### Camera

Page Up: Zoom In.
Page Down: Zoom Out.

Ctrl+Left Arrow: Move camera left.
Ctrl+Right Arrow: Move camera right.
Ctrl+Up Arrow: Move camera up.
Ctrl+Down Arrow: Move camera down.

Home: Re-center camera on selected square.

The camera will not scroll past the edge of the field.

### Stepping Forward

Enter: Step the simulation forward.  Also, output the time it took to
step forward the entered number of generations.

## Extended Commands

Extended commands can be entered by typing a `:` character, typing the
command, and then pressing `Enter`.

### Exiting the simulator.

    :q

### Clearing the field.

    :c

This will clear the entire field, or the selected region if a region
is selected.

### Saving and Loading

    :s <filename>
    :w <filename>

Saves the board's current state into the entered file.

    :l <filename>
    :o <filename>

Opens a saved file.

### Creating a new board

    :n <number> <number>

Destroys the current field and creates a new field of the given width
and height.

### Copying and pasting

    :y <name>

If a region is selected, copies it into a named register and selects
that register for pasting.  The `Space` key is used to paste the
contents of the register.  The `Escape` key can be used to de-select
the named register and return to normal editing mode.

    :y <name>

If no region is selected or if a named register is selected, selects
the specified named register.

### Playing the game forward

    :p

Toggles playback mode, in which the game steps forward at a stately
pace.
