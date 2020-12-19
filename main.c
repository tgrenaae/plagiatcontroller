#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void runcontrol(void);
char *loadfile(char *fp, size_t *size);
size_t calcarray(FILE *fp);
char **preprocessing(char *txt, size_t txtsize, int *sentencecount);


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
    
    for(int i = 0; i < scount1; i++){
        printf("#%d:\n%s\n", i+1, sentences1[i]);
    }
    for(int i = 0; i < scount2; i++){
        printf("#%d:\n%s\n", i+1, sentences2[i]);
    }
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

