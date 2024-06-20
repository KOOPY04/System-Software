#include <string.h>
#include <stdlib.h>
#include "2-optable.c"

#define    ADDR_SIMPLE           0x01 // addressing mode
#define    ADDR_IMMEDIATE        0x02
#define    ADDR_INDIRECT         0x04
#define    ADDR_INDEX            0x08

#define    LINE_EOF            (-1) // status
#define    LINE_COMMENT        (-2)
#define    LINE_ERROR          (0)
#define    LINE_CORRECT        (1)

typedef struct{
    char        symbol[LEN_SYMBOL]; // label
    char        op[LEN_SYMBOL]; // instuction
    char        operand1[LEN_SYMBOL]; // operand
    char        operand2[LEN_SYMBOL];
    unsigned    code; // opcode's number
    unsigned    fmt;
    unsigned    addressing;
}LINE;

typedef struct{ // for storing symbol and locctr
    char sym[LEN_SYMBOL];
    int locctr;
}SYMTAB;

typedef struct{
    int opcode;
    int reg1, reg2;
    int xbpe;
    int disp;
    int flag;
    char obj_x[20];
    int obj_c[20];
    int fmt;
}OBJCODE;

typedef struct{
    int loc;
    int half;
}MOD;

int process_line(LINE *line);

void init_LINE(LINE *line){
    line->symbol[0] = '\0';
    line->op[0] = '\0';
    line->operand1[0] = '\0';
    line->operand2[0] = '\0';
    line->code = 0x0;
    line->fmt = 0x0;
    line->addressing = ADDR_SIMPLE;
}

int process_line(LINE *line){
    char            buf[LEN_SYMBOL];
    int            c;
    int            state;
    int            ret;
    Instruction    *op;
    c = ASM_token(buf);
    if(c == EOF)
        return LINE_EOF;
    else if((c == 1) && (buf[0] == '\n'))
        return LINE_COMMENT;
    else if((c == 1) && (buf[0] == '.')){
        do{
            c = ASM_token(buf);
        }while((c != EOF) && (buf[0] != '\n'));
        return LINE_COMMENT;
    }
    else{
        init_LINE(line);
        ret = LINE_ERROR;
        state = 0;
        while(state < 8){
            switch(state){
                case 0:
                case 1:
                case 2:
                    op = is_opcode(buf); // 2
                    if((state < 2) && (buf[0] == '+')){    /* + */
                        line->fmt = FMT4;
                        state = 2;
                    }
                    else if(op != NULL){    /* INSTRUCTION */
                        strcpy(line->op, op->op);
                        line->code = op->code;
                        if(line->fmt != FMT4)
                            line->fmt = op->fmt & (FMT1 | FMT2 | FMT3);
                        state = 3;
                    }
                    else if(state == 0){  /* SYMBOL */
                        strcpy(line->symbol, buf);
                        state = 1;
                    }
                    else{        /* ERROR */
                        printf("ERROR at token %s\n", buf);
                        ret = LINE_ERROR;
                        state = 7;        /* skip following tokens in the line */
                    }
                    break;
                case 3:
                    if(line->fmt == FMT1 || line->code == 0x4C){ // RSUB    /* no operand needed */
                        if(c == EOF || buf[0] == '\n'){
                            ret = LINE_CORRECT;
                            state = 8;
                        }
                        else{    /* COMMENT */
                            ret = LINE_CORRECT;
                            state = 7;
                        }
                    }
                    else{
                        if(c == EOF || buf[0] == '\n'){
                            ret = LINE_ERROR;
                            state = 8;
                        }
                        else if(buf[0] == '@' || buf[0] == '#'){
                            line->addressing = (buf[0] == '#') ? ADDR_IMMEDIATE : ADDR_INDIRECT;
                            state = 4;
                        }
                        else{    /* get a symbol */
                            op = is_opcode(buf);
                            if(op != NULL){
                                printf("Operand1 cannot be a reserved word\n");
                                ret = LINE_ERROR;
                                state = 7;         /* skip following tokens in the line */
                            }
                            else{
                                strcpy(line->operand1, buf);
                                state = 5;
                            }
                        }
                    }
                    break;
                case 4:
                    op = is_opcode(buf);
                    if(op != NULL){
                        printf("Operand1 cannot be a reserved word\n");
                        ret = LINE_ERROR;
                        state = 7;        /* skip following tokens in the line */
                    }
                    else{
                        strcpy(line->operand1, buf);
                        state = 5;
                    }
                    break;
                case 5:
                    if(c == EOF || buf[0] == '\n'){
                        ret = LINE_CORRECT;
                        state = 8;
                    }
                    else if(buf[0] == ','){
                        state = 6;
                    }
                    else{    /* COMMENT */
                        ret = LINE_CORRECT;
                        state = 7;        /* skip following tokens in the line */
                    }
                    break;
                case 6:
                    if(c == EOF || buf[0] == '\n'){
                        ret = LINE_ERROR;
                        state = 8;
                    }
                    else{    /* get a symbol */
                        op = is_opcode(buf);
                        if(op != NULL){
                            printf("Operand2 cannot be a reserved word\n");
                            ret = LINE_ERROR;
                            state = 7;        /* skip following tokens in the line */
                        }
                        else{
                            if(line->fmt == FMT2){
                                strcpy(line->operand2, buf);
                                ret = LINE_CORRECT;
                                state = 7;
                            }
                            else if((c == 1) && (buf[0] == 'x' || buf[0] == 'X')){
                                line->addressing = line->addressing | ADDR_INDEX;
                                ret = LINE_CORRECT;
                                strcpy(line->operand2, buf);
                                state = 7;        /* skip following tokens in the line */
                            }
                            else{
                                printf("Operand2 exists only if format 2  is used\n");
                                ret = LINE_ERROR;
                                state = 7;        /* skip following tokens in the line */
                            }
                        }
                    }
                    break;
                case 7:    /* skip tokens until '\n' || EOF */
                    if(c == EOF || buf[0] =='\n')
                        state = 8;
                    break;
            }
            if(state < 8)
                c = ASM_token(buf);  /* get the next token */
        }
        return ret;
    }
}

int symbol(SYMTAB *st, int *count, LINE *line, int *current){ // symbol table
    int i;
    if(line->symbol[0] != '\0'){
        for(i=0; i<=(*count); i++){
            if(strcmp(line->symbol, (st + i)->sym) == 0){
                printf("Error.\n");
                return 1;
            }
        }
        // store the symbol and locctr to st[count]
        strcpy((st + (*count))->sym, line->symbol);
        (st + (*count))->locctr = *current;
        (*count)++;
    }
    return 0;
}

void loc(LINE *line, int *current){
    if(line->fmt == FMT0){
        if(strcmp(line->op, "WORD") == 0)
            *current += 3;
        else if(strcmp(line->op, "RESW") == 0)
            *current += 3 * atoi(line->operand1);
        else if(strcmp(line->op, "RESB") == 0)
            *current += atoi(line->operand1);
        else if(strcmp(line->op, "BYTE") == 0){
            if(line->operand1[0] == 'C')
                *current += strlen(line->operand1) - 3;
            else
                *current += 1;
        }
    }
    else if(line->fmt == FMT1)
        *current += 1;
    else if(line->fmt == FMT2)
        *current += 2;
    else if(line->fmt == FMT3)
        *current += 3;
    else if(line->fmt == FMT4)
        *current += 4;
}

int searchSYM(LINE *line, SYMTAB *st, int *count){
    int i;
    for(i=0; i<(*count); i++)
        if(strcmp(line->operand1, (st+i)->sym) == 0)
            return (st+i)->locctr;
    return -1;
}

int reg(char *type){
    if(*type == 'X')
        return 1;
    else if(*type == 'L')
        return 2;
    else if(*type == 'B')
        return 3;
    else if(*type == 'S')
        return 4;
    else if(*type == 'T')
        return 5;
    else if(*type == 'F')
        return 6;
    else
        return 0;
}

void negative(int disp){
    int a = 0, b = 0, k;
    char ch[4];
    char hex[] = "0123456789ABCDEF";
    while(disp){
        if(a == 0)
            b = 15 - (disp % 16) + 1;
        else
            b = 15 - (disp % 16);
        ch[a++] = hex[b];
        disp /= 16;
    }
    if(a == 1){
        ch[a++] = 'F';
        ch[a] = 'F';
    }
    else if(a == 2)
        ch[a] = 'F';
    ch[3] = '\0';
    for(k=strlen(ch)-1; k>=0; k--)
        printf("%c", ch[k]);
}

void print(OBJCODE *obj, int record, int current, int *count){
    printf("T^%06X^%02X^", record, current - record);
    int i, j;
    for(i=0; i<*count; i++){
        if((obj + i)->fmt == FMT0){
            if((obj + i)->flag == 0)
                printf("%06X", (obj + i)->opcode);
            if((obj + i)->flag == 1) // X
                printf("%s", (obj + i)->obj_x);
            else if((obj + i)->flag == 2){ // C
                for(j=1; j<=(obj+i)->obj_c[0]; j++)
                    printf("%X", (obj + i)->obj_c[j]);
            }
        }
        else if((obj + i)->fmt == FMT1) // _
            printf("%02X", (obj + i)->opcode);
        else if((obj + i)->fmt == FMT2) // _ _
            printf("%02X%X%X", (obj + i)->opcode, (obj + i)->reg1, (obj + i)->reg2);
        else if((obj + i)->fmt == FMT3){ // _ _ _
            printf("%02X%X", (obj + i)->opcode, (obj + i)->xbpe);
            if((obj + i)->disp < 0)
                negative(((obj + i)->disp *= -1));
            else
                printf("%03X", (obj + i)->disp);
        }
        else if((obj + i)->fmt == FMT4) // _ _ _ _
            printf("%02X%X%05X", (obj + i)->opcode, (obj + i)->xbpe, (obj + i)->disp);
        if(i == *count - 1)
            printf("\n");
        else
            printf("^");
    }
}

int main(int argc, char *argv[]){
    int i, j, k, c;
    int count = 0, current = 0, start = 0;
    int record = 0, temp, byteCtr = 0;
    int base = 0, baseFlag = 0;
    int symloc;
    int modCtr = 0;
    LINE line;
    SYMTAB st[1000];
    OBJCODE obj[1000];
    MOD mod[1000];
    if(ASM_open(argv[1]) == NULL)
        printf("File not found!\n");
    else{
        while((c = process_line(&line)) != LINE_EOF){ // OPCODE != 'END'
            if(c == LINE_ERROR)
                printf("Error.\n");
            else if(c == LINE_COMMENT)
                continue;
            else if(line.code == OP_START)
                start = current = atoi(line.operand1); // save the operand into start
            else{
                if(symbol(st, &count, &line, &current)) // error
                    break;
                loc(&line, &current);
            }
        }
        ASM_close();
    }
    if(ASM_open(argv[1]) == NULL)
        printf("File not found!\n");
    else{
        while((c = process_line(&line)) != LINE_EOF){
            if(line.code == OP_START){ // head record
                printf("H^%-6s^%06X^%06X\n", line.symbol, start, current);
                record = temp = current = start;
            }
            else{ // text record
                if(c != LINE_COMMENT){
                    loc(&line, &temp);
                    if((line.code == OP_RESW) || (line.code == OP_RESB) || (line.code == OP_END) || ((temp - record) > 30)){
                        // new text record
                        if(byteCtr > 0)
                            print(obj, record, current, &byteCtr);
                        record = current;
                        byteCtr = 0;
                    }
                    if((line.code != OP_RESW) && (line.code != OP_RESB)){
                        if(line.code == OP_BASE){
                            baseFlag = 1;
                            base = searchSYM(&line, st, &count);
                            if(base == -1)
                                base = atoi(line.operand1);
                        }
                        else if(line.code == OP_NOBASE)
                            baseFlag = 0;
                        else{
                            obj[byteCtr].fmt = line.fmt;
                            obj[byteCtr].opcode = line.code;
                            obj[byteCtr].flag = 0;
                            if(line.fmt == FMT0){
                                if(line.code == OP_WORD)
                                    obj[byteCtr].opcode = atoi(line.operand1);
                                else if(line.code == OP_BYTE){
                                    if(line.operand1[0] == 'X'){ // X''
                                        obj[byteCtr].flag = 1;
                                        for(i=2, j=0; i<strlen(line.operand1)-1; i++, j++)
                                            obj[byteCtr].obj_x[j] = line.operand1[i];
                                        obj[byteCtr].obj_x[j] = '\0';
                                    }
                                    else{ // C''
                                        obj[byteCtr].flag = 2;
                                        j = 1;
                                        for(i=2; i<strlen(line.operand1)-1; i++){
                                            obj[byteCtr].obj_c[j++] = line.operand1[i];
                                            obj[byteCtr].obj_c[0]++;
                                        }
                                    }
                                }
                            }
                            else if(line.fmt == FMT2){
                                obj[byteCtr].reg1 = reg(&line.operand1[0]);
                                obj[byteCtr].reg2 = reg(&line.operand2[0]);
                            }
                            else if(line.fmt == FMT3 || line.fmt == FMT4){
                                obj[byteCtr].xbpe = 0b0000;
                                symloc = searchSYM(&line, st, &count);
                                if(symloc != -1)
                                    obj[byteCtr].disp = symloc;
                                else{
                                    if(line.operand1[0] == '\0')
                                        obj[byteCtr].disp = 0;
                                    else
                                        obj[byteCtr].disp = atoi(line.operand1);
                                }
                                int add = line.addressing;
                                if(add == 9){
                                    add = line.addressing & ADDR_SIMPLE; // 1001 & 0011 -> 0001
                                    obj[byteCtr].xbpe = 0b1000;
                                }
                                if(add == ADDR_SIMPLE)
                                    obj[byteCtr].opcode = line.code + 3;
                                else if(add == ADDR_IMMEDIATE)
                                    obj[byteCtr].opcode = line.code + 1;
                                else
                                    obj[byteCtr].opcode = line.code + 2;
                                if(line.fmt == FMT3){
                                    if(symloc != -1){ // symbol found
                                        if((symloc - temp) < -2048 || (symloc - temp) > 2047){ // pc check
                                            if((symloc - base) < 0 || (symloc - base) > 4095){ // base check
                                                printf("Error.\n");
                                                return 0;
                                            }
                                            else{
                                                obj[byteCtr].xbpe |= 0b0100; // new hack -> |= 
                                                obj[byteCtr].disp -= base;
                                            }
                                        }
                                        else{
                                            obj[byteCtr].xbpe |= 0b0010;
                                            obj[byteCtr].disp -= temp;
                                        }
                                    }
                                }
                                else{
                                    obj[byteCtr].xbpe |= 0b0001;
                                    if(symloc != -1){
                                        mod[modCtr].loc = current + 1;
                                        mod[modCtr++].half = 5;
                                    }
                                }
                            }
                            byteCtr++;
                        }
                    }
                    current = temp;
                }
            }
        }
        ASM_close();
    }
    for(k=0; k<modCtr; k++) // modification record
        printf("M^%06X^%02X\n", mod[k].loc, mod[k].half);
    printf("E^%06X\n", start);
}
