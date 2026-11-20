; Return 1 if IRQs were enabled at entry (6502 P.I flag clear), else 0.
.export _paw_puppy_egg_irq_enabled
.segment "CODE"
_paw_puppy_egg_irq_enabled:
    php
    pla
    and #$04
    beq @enabled
    lda #$00
    rts
@enabled:
    lda #$01
    rts
