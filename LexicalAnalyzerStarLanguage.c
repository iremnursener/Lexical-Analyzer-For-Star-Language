#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#define UNLIMITED 1000  // UNLIMITED is being defined as 1000. 

size_t lineNo = 1;



//Lexical Analyzer for STAR language

char skipSpace(FILE *inputFile, char ch);

int checkIfBracket(char ch);

char parseBracket(FILE *inputFile, FILE *outputFile, char ch);

int checkIfOperator(char ch);

char parseOperator(FILE *inputFile, FILE *outputFile, char ch);

char parseStringConstant(FILE *inputFile, FILE *outputFile, char ch);

int closeFilesAndExit(FILE *inputFile, FILE *outputFile, int code);

char parseWord(FILE *inputFile, FILE *outputFile, char ch);

char parseNumber(FILE *inputFile, FILE *outputFile, char ch);

int checkIfKeyword(char *word);

int checkIfSpace(char ch);


//MAIN
int main(int argc, char *argv[]) {
  
    char *fileName = "code.sta";

    FILE *inputFile = fopen(fileName, "r");


     if (inputFile == NULL) {
        printf("Can't open the input file!\n");
        return 1;
    }

    FILE *outputFile = fopen("code.lex", "w"); //yeni bir output fileı oluşturur

    if (outputFile == NULL) {
        printf("Can't create the output file!\n");
        fclose(inputFile);
        return 1;
    }

    char ch = fgetc(inputFile);

   while (ch != EOF) {
        if (isalpha(ch)) //alfabeden bir harf mi kontrol edildi
            ch = parseWord(inputFile, outputFile, ch);
        else if (isdigit(ch))
            ch = parseNumber(inputFile, outputFile, ch);
        else if (checkIfOperator(ch))
            ch = parseOperator(inputFile, outputFile, ch);
        else if (ch == '"')
            ch = parseStringConstant(inputFile, outputFile, ch);
        else if (checkIfBracket(ch))
            ch = parseBracket(inputFile, outputFile, ch);
        else if (checkIfSpace(ch))
            ch = skipSpace(inputFile, ch);
        else if (ch == ',') {
            fprintf(outputFile, "Comma\n");
            ch = fgetc(inputFile); // Bir sonraki karakteri al
        } else if (ch == '.') { //satır sonunu belirtir
            fprintf(outputFile, "EndOfLine\n");
            ch = fgetc(inputFile);
        } else {
            printf("Invalid character ' %c ' ! (at line %zu)\n", ch, lineNo);
            closeFilesAndExit(inputFile, outputFile, ch);
        }
    }

    closeFilesAndExit(inputFile, outputFile, 0);
}

int closeFilesAndExit(FILE *inputFile, FILE *outputFile, int code) { 
    fclose(inputFile);
    fclose(outputFile);
    if (code == 0) printf("Lexical analyzed successfully!\n");
    else remove("code.lex");

    exit(code);
}

char parseWord(FILE *inputFile, FILE *outputFile, char ch) { 
    char *word = (char *) malloc(sizeof(char) * (10 + 1));
    if (word == NULL) {
        printf("Memory allocation failed! (at line %zu)\n", lineNo);
        closeFilesAndExit(inputFile, outputFile, 1);
    }
    size_t size = 0;

    while (ch != EOF && (isalnum(ch) || ch == '_')) { //isalnum -harf veya rakam olmasını kontrol eder.
        if (size > 9) {
            printf("Identifier can't be longer than 10 characters! (at line %zu)\n", lineNo);
            closeFilesAndExit(inputFile, outputFile, 1);
        }
        word[size++] = tolower(ch);
        ch = fgetc(inputFile);
    }
    word[size] = '\0';

    if (checkIfKeyword(word)) fprintf(outputFile, "Keyword(%s)\n", word); //keyword olarak yazdırılır
    else fprintf(outputFile, "Identifier(%s)\n", word); //identifier olarak yazdırılır

    free(word);//artık bellek bloğunu kullanmayacağı için serbest bırakıldı.
    return ch;
}

char parseNumber(FILE *inputFile, FILE *outputFile, char ch) {
    char *word = (char *) malloc(sizeof(char) * (8 + 1));
    if (word == NULL) {
        printf("Memory allocation failed! (at line %zu)\n", lineNo);
        closeFilesAndExit(inputFile, outputFile, 1);
    }
    size_t size = 0;

    while (ch != EOF && isdigit(ch)) { //isdigit(ch) işlevini kullanarak sadece rakam karakterlerini kontrol eder.- kabul etmez
        if (size > 7 ) {
            printf("Integer size can't be  longer than 8 digits! (at line %zu)\n", lineNo);
            closeFilesAndExit(inputFile, outputFile, 1);
        }
        word[size++] = tolower(ch);
        ch = fgetc(inputFile);
    }

    if (isalpha(ch)) {
        printf("Invalid identifier! (at line %zu)\n", lineNo);
        closeFilesAndExit(inputFile, outputFile, 1);
    }

    word[size] = '\0';
    fprintf(outputFile, "IntConst(%s)\n", word);

    free(word);
    return ch;
}

int checkIfKeyword(char *word) {
    const char keywords[8][9] = { "int", "text", "is", "loop", "times", "read", "write", "newLine"}; // keywords sayısı-keyword maks karakter sayısı
    size_t size = sizeof(keywords) / sizeof(keywords[0]);

    for (size_t i = 0; i < size; i++) {
        if (strcmp(keywords[i], word) == 0) //strcmp işlevi kullanılarak, verilen kelime ile keywords dizisinin o anki elemanı karşılaştırılır.
            return 1;
    }
    return 0;
}

int checkIfSpace(char ch) { //spaceleri tanımlar
    return ch == ' ' || ch == '\n' || ch == '\t';
}

char skipSpace(FILE *inputFile, char ch) { //spaceleri atlar
    while (ch != EOF && checkIfSpace(ch)) {
        if (ch == '\n')
            lineNo++;
        ch = fgetc(inputFile);
    }
    return ch;
}

int checkIfBracket(char ch) { //bracketleri tanır
    return  ch == '{' || ch == '}';
}

char parseBracket(FILE *inputFile, FILE *outputFile, char ch) {
    if (ch == '{') fprintf(outputFile, "LeftCurlyBracket\n");
    else if (ch == '}') fprintf(outputFile, "RightCurlyBracket\n");
    
    

    ch = fgetc(inputFile);

    return ch;
}

int checkIfOperator(char ch) {  //operatorleri tanımlar
    return ch == '+' || ch == '-' || ch == '*' || ch == '/' || ch == ':';
}



char parseOperator(FILE *inputFile, FILE *outputFile, char ch) {
    char *operatorString = (char *) malloc(sizeof(char) * (2 + 1)); //malloc, "memory allocation,bellekte yer ayırdık.
    if (operatorString == NULL) { 
        printf("Memory allocation failed! (at line %zu)\n", lineNo);
        closeFilesAndExit(inputFile, outputFile, 1);
    }

    char first = ch, next = '\0'; // İlk karakteri first değişkenine atar ve operatorString dizisinin ilk elemanına kaydeder.
    operatorString[0] = first;  //

    if (first == ':') {
        ch = fgetc(inputFile);
        if (ch != '=') { //sıradaki karakter = mı bakılır değilse hata mesajı verir
            printf("Invalid operator ':' ! (at line %zu)\n", lineNo);
            closeFilesAndExit(inputFile, outputFile, 1);
        } else {
            operatorString[1] = ch;  // = ise sıraya eklenir
            operatorString[2] = '\0'; //sona null kondu
        }
        next = fgetc(inputFile); //sıradaki ch ile devam edildi

        if (checkIfOperator(next)) { //next operator değil ise hata verir
            printf("Invalid operator ' %c%c ' ! (at line %zu)\n", first, next, lineNo);
            closeFilesAndExit(inputFile, outputFile, 1);
        }
    } else if (first == '/') {
        next = fgetc(inputFile);
        if (next == '*') { //yorum satırı bulundu
            ch = fgetc(inputFile);
            while (ch != EOF) { // dosya sonuna kadar devam eder
                if (ch == '\n') lineNo++; //satır sayısını arttırır

                if (ch == '*') { // * işaretini görünce yorum satırının bitmesi gerekir
                    ch = fgetc(inputFile);
                    if (ch == '/') { // * dan sonra / gelirse yorum satırı biter
                        ch = fgetc(inputFile);
                        return ch;
                    }
                } else ch = fgetc(inputFile);
            }

            printf("End of file before terminating comment! (at line %zu)\n", lineNo); //yorum satırı kapanmadan dosya bitti
            closeFilesAndExit(inputFile, outputFile, 1);
        } else if (checkIfOperator(next)) {
            printf("Invalid operator ' %c%c ' ! (at line %zu)\n", first, next, lineNo);
            closeFilesAndExit(inputFile, outputFile, 1);
        } else
            operatorString[1] = '\0';

    } else if (first == '+') {
        ch = fgetc(inputFile);
        if (ch == '+') { // ++ bulundu
            operatorString[1] = ch;
            operatorString[2] = '\0';

            next = fgetc(inputFile);
            if (checkIfOperator(next)) {
                printf("Invalid operator ' %c%c ' ! (at line %zu)\n", first, next, lineNo);
                closeFilesAndExit(inputFile, outputFile, 1);
            }
        } else {
            operatorString[1] = '\0';
            next = ch;
            if (checkIfOperator(next)) {
                printf("Invalid operator ' %c%c ' ! (at line %zu)\n", first, next, lineNo);
                closeFilesAndExit(inputFile, outputFile, 1);
            }
        }
    } else if (first == '-') {
        ch = fgetc(inputFile);
        if (ch == '-') { // -- bulundu
            operatorString[1] = ch;
            operatorString[2] = '\0';

            next = fgetc(inputFile);
            if (checkIfOperator(next)) {
                printf("Invalid operator ' %c%c ' ! (at line %zu)\n", first, next, lineNo);
                closeFilesAndExit(inputFile, outputFile, 1);
            }
        } else if (isdigit(ch) != 0) { // -li ifade girişi engellendi
            printf("Negative values are not allowed! (at line %zu)\n", lineNo);
            closeFilesAndExit(inputFile, outputFile, 1);
        } else {
            next = ch;
            if (checkIfOperator(next)) {
                printf("Invalid operator ' %c%c ' ! (at line %zu)\n", first, next, lineNo);
                closeFilesAndExit(inputFile, outputFile, 1);
            }
            operatorString[1] = '\0';
        }
    } else {
        next = fgetc(inputFile);
        if (checkIfOperator(next)) {
            printf("Invalid operator ' %c%c ' ! (at line %zu)\n", first, next, lineNo);
            closeFilesAndExit(inputFile, outputFile, 1);
        }
        operatorString[1] = '\0';
    }
    fprintf(outputFile, "Operator(%s)\n", operatorString);

    free(operatorString);
    return next;
}

char parseStringConstant(FILE *inputFile, FILE *outputFile, char ch) {
    ch = fgetc(inputFile);

    char *stringConstant = (char *) malloc(sizeof(char) * (256 + 1)); //maks 256 karaktere izin verildi
    if (stringConstant == NULL) {
        printf("Memory allocation failed ! (at line %zu)\n", lineNo);
        closeFilesAndExit(inputFile, outputFile, 1);
    }
    size_t size = 0;

    while (ch != EOF && ch != '"') { //Döngü, ya dosyanın sonuna veya çift tırnak işareti karakterine ulaşana kadar devam eder.
        if (ch == '\n') lineNo++; //programın kaçıncı satırda olduğu takip edilir
        stringConstant[size++] = ch; //Okunan karakter (ch) stringConstant dizisine eklenir ve dizinin boyutu (size) bir arttırılır.
        if (size > 256) { // String 256 karakterden fazla ise hata mesajı yazdır ve işlemi sonlandır
            printf("String constant exceeds maximum length of 256 characters! (at line %zu)\n", lineNo);
            closeFilesAndExit(inputFile, outputFile, 1);
        }
        ch = fgetc(inputFile); //Bir sonraki karakter fgetc(inputFile) kullanılarak okunur ve ch değişkenine atanır.
    }
    stringConstant[size] = '\0';

    if (ch == EOF) {//döngü çift tırnak işaretiyle bitmeden dosyanın sonuna ulaşılırsa, bir hata mesajı yazdırılır 
        printf("End of file before terminating string constant! (at line %zu)\n", lineNo); 
        closeFilesAndExit(inputFile, outputFile, 1);
    }

    ch = fgetc(inputFile);   //fetches the next character from the input file using fgetc.
    fprintf(outputFile, "stringConstant(%s)\n", stringConstant);

    free(stringConstant);
    return ch;
}
