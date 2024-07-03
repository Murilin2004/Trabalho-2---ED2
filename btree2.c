#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "btree2.h" // Suponha que btree2.h contenha a definição da estrutura ArvoreB e NoArvoreB

// Função para criar uma árvore B inicializada
ArvoreB* criarArvoreB() {
    ArvoreB *arvore = (ArvoreB*)malloc(sizeof(ArvoreB));
    if (arvore == NULL) {
        perror("Erro ao alocar memória para a árvore B");
        return NULL;
    }
    arvore->raiz = criarNo(1); // Cria a raiz com indicação de ser uma folha
    return arvore;
}

// Função para criar um nó da árvore B com inicialização básica
NoArvoreB* criarNo(int folha) {
    NoArvoreB *no = (NoArvoreB*)malloc(sizeof(NoArvoreB));
    if (no == NULL) {
        perror("Erro ao alocar memória para o nó da árvore B");
        return NULL;
    }
    no->folha = folha;
    no->n = 0;
    for (int i = 0; i < 2 * T; i++) {
        no->C[i] = NULL;
    }
    return no;
}

// Função para criar o índice a partir de um arquivo CSV
void criarIndice(ArvoreB *arvore, const char *nomeArquivo) {
    FILE *arquivo = fopen(nomeArquivo, "r");
    if (arquivo == NULL) {
        perror("Erro ao abrir o arquivo");
        return;
    }

    char linha[200];
    int linhaNum = 0;

    // Lê cada linha do arquivo CSV
    while (fgets(linha, sizeof(linha), arquivo)) {
        char *nome = strtok(linha, ",");
        char *cpf = strtok(NULL, ",");
        char *curso = strtok(NULL, ",");
        char *matricula_str = strtok(NULL, ",");

        // Verifica se todos os campos foram lidos corretamente
        if (nome && cpf && curso && matricula_str) {
            Registro registro;
            strncpy(registro.nome, nome, sizeof(registro.nome) - 1);
            strncpy(registro.cpf, cpf, sizeof(registro.cpf) - 1);
            strncpy(registro.curso, curso, sizeof(registro.curso) - 1);
            registro.matricula = atoi(matricula_str);
            registro.linha = linhaNum;
            inserir(arvore, registro);
        }
        linhaNum++;
    }

    fclose(arquivo);
}

// Função para inserir um registro na árvore B
void inserir(ArvoreB *arvore, Registro registro) {
    NoArvoreB *r = arvore->raiz;
    if (r->n == 2 * T - 1) {
        NoArvoreB *s = criarNo(0); // Cria um novo nó não folha
        if (s == NULL) {
            perror("Erro ao criar novo nó nao folha");
            return;
        }
        arvore->raiz = s;
        s->C[0] = r;
        dividirFilho(s, 0);
        inserirNaoCheio(s, registro);
    } else {
        inserirNaoCheio(r, registro);
    }
}

// Função para inserir um registro em um nó não cheio da árvore B
void inserirNaoCheio(NoArvoreB *no, Registro registro) {
    int i = no->n - 1;

    if (no->folha) {
        // Move registros maiores para abrir espaço para o novo registro
        while (i >= 0 && registro.matricula < no->registros[i].matricula) {
            no->registros[i + 1] = no->registros[i];
            i--;
        }
        no->registros[i + 1] = registro;
        no->n++;
    } else {
        // Encontra o filho apropriado para inserção recursiva
        while (i >= 0 && registro.matricula < no->registros[i].matricula) {
            i--;
        }
        i++;
        if (no->C[i]->n == 2 * T - 1) {
            dividirFilho(no, i); // Divide o filho se estiver cheio
            if (registro.matricula > no->registros[i].matricula) {
                i++;
            }
        }
        inserirNaoCheio(no->C[i], registro);
    }
}

// Função para dividir um filho de um nó da árvore B
void dividirFilho(NoArvoreB *x, int i) {
    NoArvoreB *z = criarNo(x->C[i]->folha);
    NoArvoreB *y = x->C[i];
    z->n = T - 1;

    // Move metade dos registros de y para z
    for (int j = 0; j < T - 1; j++) {
        z->registros[j] = y->registros[j + T];
    }

    // Move os ponteiros dos filhos se y não for folha
    if (!y->folha) {
        for (int j = 0; j < T; j++) {
            z->C[j] = y->C[j + T];
        }
    }

    y->n = T - 1;

    // Move os ponteiros dos filhos de x para abrir espaço para z
    for (int j = x->n; j >= i + 1; j--) {
        x->C[j + 1] = x->C[j];
    }
    x->C[i + 1] = z;

    // Move os registros de x para abrir espaço para o registro mediano de y
    for (int j = x->n - 1; j >= i; j--) {
        x->registros[j + 1] = x->registros[j];
    }
    x->registros[i] = y->registros[T - 1];
    x->n++;
}

// Função para buscar um elemento na árvore B e retornar o número da linha
int buscarElemento(NoArvoreB *no, int matricula) {
    int i = 0;
    // Encontra a posição do registro na lista ordenada
    while (i < no->n && matricula > no->registros[i].matricula) {
        i++;
    }

    // Verifica se a matrícula foi encontrada
    if (i < no->n && matricula == no->registros[i].matricula) {
        return no->registros[i].linha;
    }

    // Caso não seja uma folha, continua a busca no filho apropriado
    if (no->folha) {
        printf("Matricula %d nao encontrada na arvore\n", matricula);
        return -1;
    } else {
        return buscarElemento(no->C[i], matricula);
    }
}

// Função para buscar um registro no arquivo pelo número da linha
void buscarNoArquivo(const char *nomeArquivo, int linha) {
    FILE *arquivo = fopen(nomeArquivo, "r");
    if (arquivo == NULL) {
        perror("Erro ao abrir o arquivo");
        return;
    }

    char buffer[200];
    int currentLine = 0;

    // Lê cada linha do arquivo até encontrar a linha desejada
    while (fgets(buffer, sizeof(buffer), arquivo)) {
        if (currentLine == linha) {
            buffer[strcspn(buffer, "\n")] = '\0';

            char *nome = strtok(buffer, ",");
            char *cpf = strtok(NULL, ",");
            char *curso = strtok(NULL, ",");
            char *matricula_str = strtok(NULL, ",");

            if (nome && cpf && curso && matricula_str) {
                printf("Nome: %s, CPF: %s, Curso: %s, Matricula: %s\n", nome, cpf, curso, matricula_str);
            } else {
                printf("Erro ao ler o registro na linha %d\n", currentLine);
            }
            break;
        }
        currentLine++;
    }

    if (currentLine != linha) {
        printf("Linha %d nao encontrada no arquivo\n", linha);
    }
    fclose(arquivo);
}

// Função para remover um registro da árvore B
void remover(ArvoreB *arvore, int matricula) {
    if (arvore->raiz == NULL) {
        printf("A arvore está vazia\n");
        return;
    }

    removerChave(arvore->raiz, matricula);

    // Verifica se a raiz ficou vazia após a remoção
    if (arvore->raiz->n == 0) {
        NoArvoreB *tmp = arvore->raiz;
        if (arvore->raiz->folha) {
            arvore->raiz = NULL;
        } else {
            arvore->raiz = arvore->raiz->C[0];
        }
        free(tmp);
    }
}

// Função para remover uma chave de um nó da árvore B
void removerChave(NoArvoreB *no, int matricula) {
    int idx = buscarChave(no, matricula);

    if (idx < no->n && no->registros[idx].matricula == matricula) {
        if (no->folha) {
            removerDeFolha(no, idx);
        } else {
            removerDeNaoFolha(no, idx);
        }
    } else {
        if (no->folha) {
            printf("A matricula %d nao esta na arvore\n", matricula);
            return;
        }

        int flag = (idx == no->n);

        if (no->C[idx]->n < T) {
            preencher(no, idx);
        }

        // Continua a remoção recursiva no filho apropriado
        if (flag && idx > no->n) {
            removerChave(no->C[idx - 1], matricula);
        } else {
            removerChave(no->C[idx], matricula);
        }
    }
}

// Função para remover de um nó folha da árvore B
void removerDeFolha(NoArvoreB *no, int idx) {
    for (int i = idx + 1; i < no->n; ++i) {
        no->registros[i - 1] = no->registros[i];
    }
    no->n--;
}

// Função para remover de um nó não folha da árvore B
void removerDeNaoFolha(NoArvoreB *no, int idx) {
    Registro k = no->registros[idx];

    // Verifica se o filho esquerdo tem pelo menos T chaves
    if (no->C[idx]->n >= T) {
        int pred = getPredecessor(no, idx);
        removerChave(no->C[idx], pred);
        no->registros[idx] = no->C[idx]->registros[no->C[idx]->n - 1];
    } else if (no->C[idx + 1]->n >= T) { // Verifica se o filho direito tem pelo menos T chaves
        int succ = getSucessor(no, idx);
        removerChave(no->C[idx + 1], succ);
        no->registros[idx] = no->C[idx + 1]->registros[0];
    } else {
        fundir(no, idx); // Se ambos têm menos que T chaves, funde-os
        removerChave(no->C[idx], k.matricula);
    }
}

// Função para preencher um nó filho que tem menos que T chaves
void preencher(NoArvoreB *no, int idx) {
    if (idx != 0 && no->C[idx - 1]->n >= T) {
        emprestarDoAnterior(no, idx);
    } else if (idx != no->n && no->C[idx + 1]->n >= T) {
        emprestarDoProximo(no, idx);
    } else {
        // Funde o nó com o seu irmão se ambos têm menos que T chaves
        if (idx != no->n) {
            fundir(no, idx);
        } else {
            fundir(no, idx - 1);
        }
    }
}

// Função para pegar emprestado de um nó anterior da árvore B
void emprestarDoAnterior(NoArvoreB *no, int idx) {
    NoArvoreB *filho = no->C[idx];
    NoArvoreB *irmao = no->C[idx - 1];

    // Move todos os registros e filhos do filho para abrir espaço para um novo registro
    for (int i = filho->n - 1; i >= 0; --i) {
        filho->registros[i + 1] = filho->registros[i];
    }

    if (!filho->folha) {
        for (int i = filho->n; i >= 0; --i) {
            filho->C[i + 1] = filho->C[i];
        }
    }

    filho->registros[0] = no->registros[idx - 1];

    if (!filho->folha) {
        filho->C[0] = irmao->C[irmao->n];
    }

    no->registros[idx - 1] = irmao->registros[irmao->n - 1];

    filho->n += 1;
    irmao->n -= 1;
}

// Função para pegar emprestado de um nó próximo da árvore B
void emprestarDoProximo(NoArvoreB *no, int idx) {
    NoArvoreB *filho = no->C[idx];
    NoArvoreB *irmao = no->C[idx + 1];

    // Move o registro do nó pai para o final do filho
    filho->registros[filho->n] = no->registros[idx];

    // Move o filho do irmão para o começo do filho se não for uma folha
    if (!filho->folha) {
        filho->C[filho->n + 1] = irmao->C[0];
    }

    no->registros[idx] = irmao->registros[0];

    for (int i = 1; i < irmao->n; ++i) {
        irmao->registros[i - 1] = irmao->registros[i];
    }

    if (!irmao->folha) {
        for (int i = 1; i <= irmao->n; ++i) {
            irmao->C[i - 1] = irmao->C[i];
        }
    }

    filho->n += 1;
    irmao->n -= 1;
}

// Função para fundir dois nós irmãos da árvore B
void fundir(NoArvoreB *no, int idx) {
    NoArvoreB *filho = no->C[idx];
    NoArvoreB *irmao = no->C[idx + 1];

    // Move o registro do nó pai para o final do filho
    filho->registros[T - 1] = no->registros[idx];

    // Move todos os registros e filhos do irmão para o filho
    for (int i = 0; i < irmao->n; ++i) {
        filho->registros[i + T] = irmao->registros[i];
    }

    if (!filho->folha) {
        for (int i = 0; i <= irmao->n; ++i) {
            filho->C[i + T] = irmao->C[i];
        }
    }

    // Move os registros e ponteiros do nó pai para preencher o espaço vazio
    for (int i = idx + 1; i < no->n; ++i) {
        no->registros[i - 1] = no->registros[i];
    }

    for (int i = idx + 2; i <= no->n; ++i) {
        no->C[i - 1] = no->C[i];
    }

    filho->n += irmao->n + 1;
    no->n--;

    free(irmao);
}

// Função para obter o predecessor de um nó na árvore B
int getPredecessor(NoArvoreB *no, int idx) {
    NoArvoreB *atual = no->C[idx];
    while (!atual->folha) {
        atual = atual->C[atual->n];
    }
    return atual->registros[atual->n - 1].matricula;
}

// Função para obter o sucessor de um nó na árvore B
int getSucessor(NoArvoreB *no, int idx) {
    NoArvoreB *atual = no->C[idx + 1];
    while (!atual->folha) {
        atual = atual->C[0];
    }
    return atual->registros[0].matricula;
}

// Função para buscar uma chave em um nó da árvore B
int buscarChave(NoArvoreB *no, int matricula) {
    int idx = 0;
    while (idx < no->n && matricula > no->registros[idx].matricula) {
        ++idx;
    }
    return idx;
}

// Função de exibição do menu principal da main.c
int exibirMenu() {
    int opcao;
    printf("\n--- Menu ---\n");
    printf("1. Criar indice\n");
    printf("2. Procurar elementos\n");
    printf("3. Remover registro\n");
    printf("4. Sair\n");
    printf("Escolha uma opcao: ");
    scanf("%d", &opcao);

    return opcao;
}

void tratarCriarIndice(ArvoreB *arvore, char nomeArquivo[100]) {
    printf("Digite o nome do arquivo para criar o indice: ");
    scanf("%s", nomeArquivo);
    criarIndice(arvore, nomeArquivo);
    printf("Indice criado a partir do arquivo %s.\n", nomeArquivo);
}

void tratarBuscarElemento(ArvoreB *arvore, const char nomeArquivo[100]) {
    int matricula;
    printf("Digite a matricula a ser buscada: ");
    scanf("%d", &matricula);

    // contagem de tempo consulta btree
    clock_t inicioBTree = clock();
    int linha = buscarElemento(arvore->raiz, matricula);
    clock_t fimBTree = clock();
    double tempoBTree = (double)(fimBTree - inicioBTree) / CLOCKS_PER_SEC;
    printf("Tempo de busca na B-Tree: %f segundos\n", tempoBTree);

    // contagem de tempo consulta arquivo
    clock_t inicioArquivo = clock();
    if (linha != -1) {
        buscarNoArquivo(nomeArquivo, linha);
        clock_t fimArquivo = clock();
        double tempoArquivo = (double)(fimArquivo - inicioArquivo) / CLOCKS_PER_SEC;
        printf("Tempo de busca no Arquivo: %f segundos\n", tempoArquivo);
    } else {
        printf("Matricula %d nao encontrada.\n", matricula);
    }
}

void tratarRemoverRegistro(ArvoreB *arvore) {
    int matricula;
    printf("Digite a matricula a ser removida: ");
    scanf("%d", &matricula);
    remover(arvore, matricula);
    printf("Matricula %d removida do indice.\n", matricula);
}

void sairPrograma() {
    printf("Saindo...\n");
    exit(0);
}