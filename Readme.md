# PAW OS — C64 communications terminal

## Commands

- `HELP` — command list
- `PHONE` — telephone command help
- `DTMF 5551234` — generate DTMF tones through SID voices 1 and 2
- `DIAL 555-1234 FAST` — same SID tone output with shorter tone/gap timing
- `MODEM` — dial-up modem hardware requirements
- `MODEM DIAL 5551234` — displays the `ATDT` request, but does not transmit it yet

## Build

From this directory, run `make`. The output is `build/pawos.prg`.

## Hardware and current implementation boundary

The PHONE/DTMF commands generate audible DTMF on the C64 SID output. They do
not seize a phone line, detect dial tone, or place a call by themselves. Use an
appropriate isolated, approved telephone interface or acoustic coupler; never
connect the SID output directly to a telephone line.

The MODEM command currently provides the command/UI scaffold only. Actual
dial-up data calls need a serial interface for the C64 (for example, a
compatible user-port RS-232 adapter or SwiftLink/Turbo232-style interface) and
an external modem. The serial backend is intentionally not claimed as working
until the exact adapter and modem are known. The modem, not the SID, performs
carrier negotiation and data transfer.

The DTMF timing uses frame waits and is a starting point for testing; actual
tone duration and level should be verified on the target C64/SID revision.
