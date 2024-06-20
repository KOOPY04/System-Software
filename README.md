HW1：

在 Device ‘F1’ 中有一字串由大小寫英文字母所組成（字串的結束字元為 ‘$’，其 ASCII 值為 0x24）。

請用 SIC 的指令集寫一組合語言程式，完成以下的功能：

請從 Device ‘F1’ 中讀取字串，如果所讀取的字元為數字，請將其依下列規則轉換並寫入 Device ‘F2’ 中 （如果所讀取的字元為英文字母，則不變，直接寫到 Device ‘F2’ 中）。

轉換規則：0 -> a, 1 -> b, 2 -> c, 3 -> d, 4 -> e, 5 -> f, 6 -> g, 7 -> h, 8 -> i, 9 -> j

HW2：

請用 SIC/XE 的指令集寫一組合語言程式，在 Device ‘F2’ 中印出如下的 9x9 乘法表，請滿足以下要求：

寫出印一個數字（Register A 的值）的副程式

以雙迴圈的方式來列印

你的輸出結果需要對齊

HW3：

請參考課堂中提供的程式 (1-token.c, 2-optable.c 及 3-asm_pass1_u.c) 及 Figure 2.4(a) 的演算法完成 SIC/XE Assembler 的 Pass 1。你的程式要完成：

如 Figure 2.2 秀出每一行組合語言程式的指令，並在前面印出 LOCCTR 的值(該行最後不需印出 Object Code ，印不印出註解均可)

印出 ASM Program 的長度

請在最後印出 SYMTAB 的 Symbol 及其對應值。

HW4：

請參考課堂中提供的程式 (1-token.c, 2-optable.c, 3-asm_pass1_u.c 及 Homework 3) 及 Figure 2.4(b) 的演算法完成 SIC/XE Assembler 的 Pass 2。

請參考 Figure 2.3 的規格產生如 Figure 2.3 的 Object Code。

能處理 SIC 的程式及定址模式，可以處理 SIC/XE 的程式及定址模式，可以產生 Figure 2.8 的 Modification Record。

loader：

請參考 Figure 3.11 的演算法，寫一個 Loader 並產生如 P.143 的 Load map。你的程式需滿足以下的格式：

Syntax: load <address> <file 1> < file 2> …

<address> 為 Starting Address (16進位表示)
  
Ex: load 1000 PROGA PROGB PROGC

PS: 你的程式要能處理 Figure 3.9 的 Object Program
