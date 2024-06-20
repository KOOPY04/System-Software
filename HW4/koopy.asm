EX1     START   4096
        LDX    #3
        LDT    #ALPHA
        STT     BETA
        LDA     ALPHA,X
        ADDR    T,A
        ADD    @BETA
        STA     BETA
ALPHA   WORD    1
        WORD    2
BETA    RESW    1