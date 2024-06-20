READ    TD      INDEV
        JEQ     READ
        RD      INDEV
        J       COMPARE
        J       READ

COMPARE COMP    EOF
        JEQ     END
        COMP    ZERO
        JLT     WRITE
        COMP    NINE
        JGT     WRITE
        JSUB    CONVERT

WRITE   WD     	OUTDEV
        J    	READ

END     RSUB

CONVERT SUB     ZERO
        ADD     CONVERT_WORD
        JSUB    WRITE
        RSUB

INDEV       BYTE    X'F1'
OUTDEV      BYTE    X'F2'
EOF         WORD    0x24
ZERO        WORD    0x30
NINE        WORD    0x39
CONVERT_WORD WORD   0x61