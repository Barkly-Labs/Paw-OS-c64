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

The DTMF generator configures both SID voice envelopes, enables triangle-wave
output on both voices, and uses paired NTSC SID tuning words. `FAST` uses a
shorter tone and gap. The frequency table is calibrated for a nominal NTSC C64;
PAL machines need a PAL-specific tuning table. Frame-based timing and output
level should be verified on the target C64/SID revision and in the chosen VICE
audio configuration.
