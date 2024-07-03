#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "btree2.h"

int main() {
    int opcao;
    ArvoreB *arvore = criarArvoreB();
    char nomeArquivo[100];

    while (1) {
        opcao = exibirMenu();

        switch (opcao) {
            case 1:
                tratarCriarIndice(arvore, nomeArquivo);
                break;
            case 2:
                tratarBuscarElemento(arvore, nomeArquivo);
                break;
            case 3:
                tratarRemoverRegistro(arvore);
                break;
            case 4:
                sairPrograma();
                break;
            default:
                printf("Opção invalida! Tente novamente.\n");
        }
    }
    return 0;
}