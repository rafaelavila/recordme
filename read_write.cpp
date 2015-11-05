#include "read_write.h"
#include <stdio.h>
#include <stdlib.h>

void saveRectification(char nome[50], int hr, int min, int durH, int durM, int qtdVideos) {

    FILE *file;
    file = fopen("controle.txt", "a");

    if (file) {
        fprintf(file, "*********** %s ***********\n", nome);
        fprintf(file, "\nHora: %02d:%02d\n", hr, min);
        fprintf(file, "Duração: %02d:%02d\n", durH, durM);
        fprintf(file, "Vídeos: %d \n\n", qtdVideos);
    }

    fclose(file);
}

bool loadRectification(float *aImg){
    FILE *arq = fopen("retificacao","rb");

    if (arq != NULL)
    {
        int leitura[8];
        fread(leitura, 1, sizeof(float) * 8, arq);
        for(int i = 0; i < 8; i++){
//            aImg->at(i) = leitura[i];
        }
        fclose(arq);        
        return true;
    }
    return false;
}
