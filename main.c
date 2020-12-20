#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

typedef struct plagiat{
    char text[1000];
    char match_text[1000];
    int line;
    int match_line;
    int word;
    int match_word;
} plagiat;

void runcontrol(void);
char *loadfile(char *fp, size_t *size);
size_t calcarray(FILE *fp);
char **preprocessing(char *txt, size_t txtsize, int *sentencecount);
plagiat *verbatim(char **sentences1, int scount1, char** sentences2, int scount2, int *verbplag);
plagiat *cryptic(char **sentences1, int scount1, char **sentences2, int scount2, int *crypplag);
static int editDist (const char * string_one, const char * string_two);
void evaluation(plagiat *verbatims, int verbplag, plagiat *cryptics, int crypplag);

int main(void){
    runcontrol();
    return 0;
}

void runcontrol(void){
    char *fp1 = "./lotr-org.txt";
    char *fp2 = "./lotr-plag.txt";

    size_t sizefp1 = 0;
    size_t sizefp2 = 0;
    char *txt1 = loadfile(fp1, &sizefp1);
    char *txt2 = loadfile(fp2, &sizefp2);
    
    int scount1 = 0;
    int scount2 = 0;
    char **sentences1 = preprocessing(txt1, sizefp1, &scount1);
    char **sentences2 = preprocessing(txt2, sizefp2, &scount2);
    
    int verbplag = 0;
    plagiat *verbatims = verbatim(sentences1, scount1, sentences2, scount2, &verbplag);

    int crypplag = 0;
    plagiat *cryptics = cryptic(sentences1, scount1, sentences2, scount2, &crypplag);

    evaluation(verbatims, verbplag, cryptics, crypplag);
    return;
}

char *loadfile(char *fp, size_t *arrsize){
    FILE *file = fopen(fp, "r");
    if(file == NULL){
        printf("fail");
        exit(EXIT_FAILURE);
    }

    *arrsize = calcarray(file);
    char *arrtxt = (char *) malloc(*arrsize * sizeof(char));
    if(arrtxt == NULL){
        printf("fail2");
        exit(EXIT_FAILURE);
    }

    for(int i = 0; i <= (int) arrsize; i++){
        fscanf(file, "%c", &arrtxt[i]);
    }
    arrtxt[*arrsize] = '\0';

    return arrtxt;
}

size_t calcarray(FILE *fp){
    size_t start_pos = ftell(fp);
    fseek(fp, 0, SEEK_END);
    size_t size_of_arr = ftell(fp);
    fseek(fp, start_pos, SEEK_SET);
    return size_of_arr;
}

char **preprocessing(char *txt, size_t txtsize, int *sentencecount){
    int scount = 0;
    int usedchar = 0;
    int sensize[100];

    for(int i = 0; i <= (int) txtsize; i++){
        if(txt[i] == '.'){
            txt[i] = '\0';
            //printf("usedchars is %d\n", usedchar);
            sensize[scount++] = i - usedchar;
            usedchar = i;
        }
    }
    *sentencecount = scount;
    char **sentences = malloc((scount) * sizeof(char *));
    if(sentences == NULL){
        printf("fail3");
        exit(EXIT_FAILURE);
    }

    for(int i = 0; i < scount; i++){
        sentences[i] = (char *) malloc(sensize[i] * sizeof(char));
        if(sentences[i] == NULL){
            printf("fail4");
            exit(EXIT_FAILURE);
        }
    }

    // usedchar = 0;
    scount = 0;
    char temp_arr[1000];
    int j = 0;

    for(int i = 0; i < (int) txtsize; i++){
        temp_arr[j++] = txt[i];
        if(txt[i] == '\0'){
            strncpy(sentences[scount++], temp_arr, j);
            j = 0;
            i++;
        }
    }



    // for(int i = 0; i < scount; i++){
    //     printf("scount is %d\nusedchar is %d\n", scount2, usedchar);
    //     printf("sensize is %d\n", sensize[i]);
    //     strncpy(sentences[scount2], &txt[usedchar], sensize[scount2]);
    //     usedchar = usedchar + sensize[scount2];
    //     scount2++;
    // }

    return sentences;
}

plagiat *verbatim(char **sentences1, int scount1, char** sentences2, int scount2, int *verbplag){
    int verbi[100];
    int verbj[100];

    for(int i = 0; i < scount1; i++){
        for(int j = 0; j < scount2; j++){
            if(strlen(sentences1[i]) == strlen(sentences2[j])){
                int charcount = 0;
                for(int k = 0; k < (int) strlen(sentences1[i]); k++){
                    if(sentences1[i][k] == sentences2[j][k]){
                        charcount++;
                    }
                }
                if(charcount == (int) strlen(sentences1[i])){
                    verbi[*verbplag] = i;
                    verbj[*verbplag] = j;
                    *verbplag = *verbplag + 1;
                }
            }
        }
    }
    plagiat *verbatims = (plagiat *) malloc(*verbplag * sizeof(plagiat));
    if(verbatims == NULL){
        printf("fail5\n");
        exit(EXIT_FAILURE);
    }

    for(int i = 0; i < *verbplag; i++){
        strcpy(verbatims[i].text, sentences1[verbi[i]]);
        strcpy(verbatims[i].match_text, sentences2[verbj[i]]);
        verbatims[i].line = verbi[i]+1;
        verbatims[i].match_line = verbj[i]+1;
    }
    return verbatims;
}

plagiat *cryptic(char **sentences1, int scount1, char **sentences2, int scount2, int *crypplag){
    int crypi[100];
    int crypj[100];
    for(int i = 0; i < scount1; i++){
        for(int j = 0; j < scount2; j++){            
            int levenshtein = editDist(sentences1[i], sentences2[j]);
            if(levenshtein == 2 || (levenshtein < 6 && levenshtein != 0)){
                crypi[*crypplag] = i;
                crypj[*crypplag] = j;
                *crypplag = *crypplag + 1;
            }

        }
        
    }

    plagiat *cryptics = malloc(*crypplag * sizeof(plagiat));
    if(cryptics == NULL){
        printf("fail6");
        exit(EXIT_FAILURE);
    }

    for(int i = 0; i < *crypplag; i++){
        strcpy(cryptics[i].text, sentences1[crypi[i]]);
        strcpy(cryptics[i].match_text, sentences2[crypj[i]]);
        cryptics[i].line = crypi[i]+1;
        cryptics[i].match_line = crypj[i]+1;
    }


    return cryptics;
}

static int editDist (const char * string_one, const char * string_two)
{
    int len1 = strlen(string_one);
    int len2 = strlen(string_two);

    int matrix[len1 + 1][len2 + 1];
    int i;
    for (i = 0; i <= len1; i++) {
        matrix[i][0] = i;
    }
    for (i = 0; i <= len2; i++) {
        matrix[0][i] = i;
    }
    for (i = 1; i <= len1; i++) {
        int j;
        char c1 = string_one[i-1];
        for (j = 1; j <= len2; j++) {

            char c2 = string_two[j-1];
            if (c1 == c2) {
                matrix[i][j] = matrix[i-1][j-1];
            }
            else {
                int delete = matrix[i-1][j] + 1;
                int insert = matrix[i][j-1] + 1;
                int substitute = matrix[i-1][j-1] + 1;
                int minimum = delete;
                if (insert < minimum) {
                    minimum = insert;
                }
                if (substitute < minimum) {
                    minimum = substitute;
                }
                matrix[i][j] = minimum;
            }
        }
    }
    return matrix[len1][len2];
}

void evaluation(plagiat *verbatims, int verbplag, plagiat *cryptics, int crypplag){
    printf("------------VERBATIM found %d matches:----------------\n", verbplag);
    for(int i = 0; i < verbplag; i++){
        printf("verbatim #%d:\n'%s' on line %d\n", i+1, verbatims[i].text, verbatims[i].line);
        printf("'%s' on line %d\n\n", verbatims[i].match_text, verbatims[i].match_line);        
    }

    printf("------------ CRYPTIC found %d matches:----------------\n", crypplag);
    for(int i = 0; i < crypplag; i++){
        printf("cryptic #%d:\n'%s' on line %d\n", i+1, cryptics[i].text, cryptics[i].line);
        printf("'%s' on line %d\n\n", cryptics[i].match_text, cryptics[i].match_line);        
    }
    return;
}