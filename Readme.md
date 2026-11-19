# PAW OS — C64 communications terminal

PAW OS is a small Commodore 64 terminal with memory information, SID sound,
PHONE/DTMF utilities, a MODEM command scaffold, and optional applications.
PAW Synth is launched from the existing terminal and returns to it when closed.

## Commands

- `HELP` — command list
- `MEM` — memory information
- `ABOUT` — PAW OS information and theme
- `CLS` — redraw the terminal
- `SYNTH` — open PAW Synth
<<<<<<< HEAD
- `DISK` / `DISK 8` — show device status / select IEC device 8–11
- `DIR` or `FILES` — open the PAW Files directory browser
- `PWD` / `CD ..` — show the disk device's root context
- `TYPE README` — page through a sequential text file
- `DELETE filename` — ask for confirmation, then scratch a file
- `RENAME old new` — rename a file on the active device
- `LOAD`, `RUN`, `SAVE`, `COPY` — report the safe-operation boundary
=======
>>>>>>> b5800ffa93ad7f862755c01629bb8cbec9eae056
- `PHONE` — telephone command help
- `DTMF 5551234` — generate DTMF tones through SID voices 1 and 2
- `DTMFTEST` — play all 16 DTMF keypad pairs through the SID
- `DIAL 555-1234 FAST` — same SID tone output with shorter tone/gap timing
- `MODEM` — dial-up modem hardware requirements
- `MODEM DIAL 5551234` — display the `ATDT` request; it is not transmitted

## Build

Install cc65 (including `cl65`) and GNU Make, then run `make` from the project
directory. The C64 PRG target is `build/pawos.prg`. Run `make clean` before a
full rebuild. The archived `build/pawos.prg` is the original supplied binary;
it could not be regenerated in the current environment because cc65 is not
installed. To test a new build in VICE, launch it with a C64 model, for example
`x64sc build/pawos.prg`.

## PAW Synth

At the `PAW>` prompt, enter `SYNTH`. The app has a native 40×25 PETSCII
interface. Press ESC to leave immediately, or press `/` and enter `EXIT` at
the `SYNTH>` prompt. Both return to the PAW OS terminal.

### Keyboard controls

| Key | Action |
| --- | --- |
| A W S E D F T G Y H U J | Play C, C#, D, D#, E, F, F#, G, G#, A, A#, B |
| Z / X | Lower / raise octave (octaves 2–6) |
| SPACE | Stop the current note |
| Arrow keys | Select a sound setting and adjust its value |
| `/` then `EXIT` | Leave through the `SYNTH>` command prompt |
| ESC | Exit PAW Synth |

The KERNAL character input used by this app reports key presses, not key
release events. Notes therefore stay gated until SPACE, another note, or ESC;
this is a deliberate, visible trigger-and-stop behavior rather than claimed
held-key detection. Polling is non-blocking between short frame waits.

### Joystick port 2

UP/DOWN selects a setting, LEFT/RIGHT changes it, and FIRE plays C in the
current octave. Keyboard controls remain active while joystick input is used.

### Sound controls

PAW Synth uses SID voice 3. It supports triangle, sawtooth, pulse, and noise
waveforms, NTSC pitch tuning from octaves 2–6, pulse width, ADSR envelope,
volume, and a basic low-pass filter. Settings are selected with the arrow keys
or joystick directions. The selected line is highlighted; LEFT/RIGHT adjusts
the value. The SID's shared volume and filter controls are saved on entry and
restored on exit, as are voice 3 registers. Voices 1 and 2 remain available to
the existing PHONE/DTMF routines.

The SID has three hardware voices. Synth uses voice 3 and does not provide
polyphonic keyboard playback. Frequency values are calibrated for an NTSC SID;
PAL pitch should be tuned separately. The app doesn't provide a key-release
event, MIDI, or audio recording.

## Hardware and current implementation boundary

The PHONE/DTMF commands generate audible DTMF on the C64 SID output. They do
not seize a phone line, detect dial tone, or place a call by themselves. Use an
appropriate isolated, approved telephone interface or acoustic coupler; never
connect SID audio directly to telephone wiring.

The MODEM command currently provides the command/UI scaffold only. Actual
dial-up data calls need a serial interface for the C64 (for example, a
compatible user-port RS-232 adapter or SwiftLink/Turbo232-style interface) and
an external modem. The serial backend is intentionally not claimed as working
until the exact adapter and modem are known. The modem, not the SID, performs
carrier negotiation and data transfer.

<<<<<<< HEAD
## PAW Disk and PAW Files

PAW Disk uses cc65's Commodore KERNAL/IEC file interfaces (`cbm.h`), DOS
directory/status channels, and the device's command channel. The default IEC
device is 8. `DISK 8` through `DISK 11` select a device for the current PAW OS
session; the choice is held in PAW Disk's own in-memory state and resets to 8
after reboot.

`DIR` and `FILES` open the same native 40-column file browser. It displays
entries returned by the disk device, their Commodore file type and block
count, and available free blocks. The browser shows the first 18 entries and
supports keyboard UP/DOWN, ENTER or T to view a SEQ file, D to delete after a
Y/N confirmation, and Q or ESC to return to PAW OS. Joystick port 2 UP/DOWN
selects entries and FIRE attempts to view the selected SEQ file. Keyboard
controls remain available. Device status and file-operation responses are
read from the DOS status channel.

Commodore 1541 DOS does not provide general nested directories. PAW Disk keeps
the current context at `/` as a virtual display label only; `CD` and `CD ..`
do not claim to change a drive-side directory. `TYPE` and browser viewing are
for SEQ files, with paged output and Q/ESC to stop. Filename operations accept
a conservative subset of names up to 16 characters: letters, digits, spaces
inside the name, `_`, `-`, and `.`. They reject DOS punctuation and wildcards.
The command-line RENAME form uses single-token names.

`LOAD` and `RUN` are deliberately disabled: loading a PRG at its embedded
address or jumping into it could overwrite the resident OS, KERNAL vectors, or
runtime state. `SAVE` has no defined PAW-owned memory region to save, and a
safe multi-drive/staged `COPY` implementation is not included. The commands
report these limits rather than pretending to perform the operations. These
features need a documented cooperative application memory map and tested
drive-specific copy behavior before they can safely be enabled.

The directory buffer is limited to 18 entries to keep memory use bounded.
VICE and physical-drive behavior still need testing with an attached disk
image or IEC drive. The current build environment did not include cc65 or VICE,
so the PRG in the archive remains the supplied snapshot binary; it does not
contain the new disk subsystem.

=======
>>>>>>> b5800ffa93ad7f862755c01629bb8cbec9eae056
The DTMF generator configures both SID voice envelopes, enables triangle-wave
output on both voices, and uses paired NTSC SID tuning words. `FAST` uses a
shorter tone and gap. The frequency table is calibrated for a nominal NTSC C64;
PAL machines need a PAL-specific tuning table. Frame-based timing and output
level should be verified on the target C64/SID revision and in the chosen VICE
audio configuration.
