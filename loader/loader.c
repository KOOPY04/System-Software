#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void address(char temp1[5], char temp2[5], int flag){
    int tmp1 = 0, tmp2 = 0;
    char hex[] = "0123456789ABCDEF";
    int count = 3;
    int i, j;
    for(i=3; i>=0; i--){
        if(temp1[i] >= 65 || temp1[i] <= 70){
            for(j = 0; j < strlen(hex); j++){
                if(temp1[i] == hex[j])
                    tmp1 = j;
            }
        }
        if(temp2[i] >= 65 || temp2[i] <= 70){
            for(j = 0; j < strlen(hex); j++){
                if(temp2[i] == hex[j])
                    tmp2 = j;
            }
        }
        if(tmp1 + tmp2 >= 16){
            temp1[i - 1]++;
            temp1[count--] = (tmp1 + tmp2) % 16 + 48;
        }
        else{
            if(tmp1 + tmp2 >= 10)
                temp1[count--] = tmp1 + tmp2 + 55;
            else
                temp1[count--] = tmp1 + tmp2 + 48;
        }
        temp1[4] = '\0';
    }
    if(flag == 0)
        printf("%s", temp1);
    else
        printf("%s\n", temp1);
}

int main(int argc, char **argv){
    int i, j = 0, k, count, flag;
    char line[50];
    char start[5], progLen[5], temp[5], addr[5];
    if (argc < 4)
    {
        printf("Usage: %s load <address> <file 1> <file 2> <file 3>\n", argv[0]);
        return 1;
    }
    strcpy(start, argv[2]);
    printf("\nControl\tSymbol\n");
    printf("section\tname\tAddress\tLength\n");
    printf("-------------------------------\n");
    for(k=2; k<argc; k++){
        FILE *fp = fopen(argv[k], "r");
        while(fscanf(fp, "%[^\n]\n", line) != EOF){
            if(line[0] == 'H'){ // head record
                j = 0;
                for(i=2; i<8; i++) // section name
                    printf("%c", line[i]);
                printf("\t\t");
                count = 3;
                if(k == 2) // starting address
                    printf("%s", start);
                else
                    address(start, progLen, flag = 0);
                j = 0;
                for(i=18; i<22; i++) // length
                    progLen[j++] = line[i];
                progLen[j] = '\0';
                printf("\t%s\n", progLen);
            }
            else if(line[0] == 'D'){ // define record
                int start_i = 2;
                j = 0;
                while(start_i < strlen(line)){
                    printf("\t");
                    for(i=start_i; i<start_i+6; i++) // symbol name
                        printf("%c", line[i]);
                    printf("\t");
                    j = 0;
                    i++;
                    for(i=start_i+9; i<start_i+13; i++)
                        addr[j++] = line[i];
                    addr[j] = '\0';
                    j = 0;
                    strcpy(temp, start);
                    address(temp, addr, flag = 1); // calculation
                    start_i = i + 1;
                }
            }
            else{ // R, T, E...
                fclose(fp);
                break;
            }
        }
    }
    printf("\n");
    return 0;
}