        LDA     SPACE
        WD      OUTDEV
        WD      OUTDEV
        WD      OUTDEV
        LDA     #1

ROW     JSUB    WLOW
        ADD     #1
        COMP    #10
        JLT     ROW
        JSUB    NEWLINE

        LDT     #0

LOOP    RMO     T,A
        ADD     #1
        RMO     A,T
        COMP    #10
        JLT     MAIN
        J       END

MAIN    JSUB    WLOW
        LDS     #1

LOOP2   RMO     S,A
        MULR    T,A 
        COMP    #10
        JLT     LOWER

        RMO     A,B
        DIV     #10
        JSUB    WRITE
        MUL     #10
        SUBR    A,B
        RMO     B,A 
        JSUB    WRITE
        LDA     SPACE
        WD      OUTDEV
        J       COL

LOWER   JSUB    WLOW

COL     RMO     S,A 
        ADD     #1
        RMO     A,S
        COMP    #10
        JLT     LOOP2
        JSUB    NEWLINE
        J       LOOP

WRITE   STA     TEMP
        ADD     ZERO
        WD      OUTDEV
        LDA     TEMP
        RSUB

WLOW  STA     TEMP
        ADD     ZERO
        WD      OUTDEV

        LDA     SPACE
        WD      OUTDEV
        WD      OUTDEV
        LDA     TEMP
        RSUB

NEWLINE LDA     ENTER
        WD      OUTDEV
        RSUB

END     J       END

ZERO    WORD    0x30
SPACE   WORD    0x20
ENTER   WORD    0xA
TEMP    RESW    1
OUTDEV  BYTE    X'F2'