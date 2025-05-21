#include "livros.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Funções auxiliares para manipulação de cabeçalho e nós

// Propósito - Escreve o cabeçalho no arquivo binário.
// Pré-Condições - O arquivo deve estar aberto em modo de escrita e o cabeçalho
// deve ser válido. Pós-Condições - O cabeçalho é escrito no início do arquivo.
void escreve_cabecalho(FILE *arq, cabecalho *cab) {
  fseek(arq, 0, SEEK_SET);
  fwrite(cab, sizeof(cabecalho), 1, arq);
}

// Propósito - Lê o cabeçalho do arquivo binário.
// Pré-Condições - O arquivo deve estar aberto em modo de leitura.
// Pós-Condições - Um ponteiro para o cabeçalho é retornado, que contém os dados
// lidos do arquivo.
cabecalho *le_cabecalho(FILE *arq) {
  cabecalho *cab = (cabecalho *)malloc(sizeof(cabecalho));
  if (cab == NULL) {
    perror("Erro ao alocar memória");
    exit(EXIT_FAILURE);
  }
  fseek(arq, 0, SEEK_SET);
  fread(cab, sizeof(cabecalho), 1, arq);
  return cab;
}

// Propósito - Lê um nó do arquivo na posição especificada.
// Pré-Condições - O arquivo deve estar aberto e a posição deve ser válida.
// Pós-Condições - Um ponteiro para o nó lido é retornado.
no *ler_no(FILE *arq, int pos) {
  no *x = (no *)malloc(sizeof(no));
  if (x == NULL) {
    perror("Erro ao alocar memória");
    exit(EXIT_FAILURE);
  }
  fseek(arq, sizeof(cabecalho) + pos * sizeof(no), SEEK_SET);
  fread(x, sizeof(no), 1, arq);
  return x;
}

// Propósito - Escreve um nó no arquivo na posição especificada.
// Pré-Condições - O arquivo deve estar aberto e o nó deve ser válido.
// Pós-Condições - O nó é escrito na posição especificada do arquivo.
void escreve_no(FILE *arq, no *x, int pos) {
  fseek(arq, sizeof(cabecalho) + pos * sizeof(no), SEEK_SET);
  fwrite(x, sizeof(no), 1, arq);
}

// Propósito - Inicializa o arquivo binário com um cabeçalho vazio.
// Pré-Condições - O arquivo deve estar aberto em modo de escrita.
// Pós-Condições - O cabeçalho vazio é escrito no arquivo.
void criar_arvore_vazia(FILE *arq_bin) {
  cabecalho *cab = (cabecalho *)malloc(sizeof(cabecalho));
  cab->pos_raiz = -1;
  cab->pos_topo = 0;
  cab->pos_livre = -1;
  escreve_cabecalho(arq_bin, cab);
  free(cab);
}

// Propósito - Insere um livro na árvore binária, reaproveitando registros
// livres. Pré-Condições - O arquivo deve estar aberto e o livro deve ser
// válido. Pós-Condições - O livro é inserido na árvore e o cabeçalho é
// atualizado.
void insere(FILE *arq, Livro *info) {
  cabecalho *cab = le_cabecalho(arq);
  int pos;
  no novo_no;

  novo_no.info = *info;
  novo_no.esq = -1;
  novo_no.dir = -1;

  if (cab->pos_livre != -1) {
    pos = cab->pos_livre;
    no *livre_no = ler_no(arq, pos);
    cab->pos_livre = livre_no->esq;
    free(livre_no);
  } else {
    pos = cab->pos_topo++;
  }

  if (cab->pos_raiz == -1) {
    cab->pos_raiz = pos;
  } else {
    int pai_pos = cab->pos_raiz;
    no *pai = ler_no(arq, pai_pos);
    while (1) {
      if (info->codigo < pai->info.codigo) {
        if (pai->esq == -1) {
          pai->esq = pos;
          escreve_no(arq, pai, pai_pos);
          free(pai);
          break;
        }
        pai_pos = pai->esq;
      } else {
        if (pai->dir == -1) {
          pai->dir = pos;
          escreve_no(arq, pai, pai_pos);
          free(pai);
          break;
        }
        pai_pos = pai->dir;
      }
      free(pai);
      pai = ler_no(arq, pai_pos);
    }
  }
  escreve_no(arq, &novo_no, pos);
  escreve_cabecalho(arq, cab);
  free(cab);
}

// Propósito - Encontra o menor nó em uma subárvore.
// Pré-Condições - O arquivo deve estar aberto e a posição deve ser válida.
// Pós-Condições - A posição do menor nó encontrado é retornada.
int encontrar_minimo(FILE *arq, int pos) {
  no *nodo = ler_no(arq, pos);
  while (nodo->esq != -1) {
    pos = nodo->esq;
    free(nodo);
    nodo = ler_no(arq, pos);
  }
  free(nodo);
  return pos;
}

// Propósito - Remove um nó recursivamente da árvore.
// Pré-Condições - O arquivo deve estar aberto, a posição e o código do livro
// devem ser válidos. Pós-Condições - O nó correspondente é removido e a posição
// do nó retornado.
int remover_recursivo(FILE *arq, int pos, int codigo, cabecalho *cab) {
  if (pos == -1)
    return -1;

  no *nodo = ler_no(arq, pos);

  if (codigo < nodo->info.codigo) {
    nodo->esq = remover_recursivo(arq, nodo->esq, codigo, cab);
  } else if (codigo > nodo->info.codigo) {
    nodo->dir = remover_recursivo(arq, nodo->dir, codigo, cab);
  } else {
    int temp;
    if (nodo->esq == -1) {
      temp = nodo->dir;
      nodo->esq = cab->pos_livre;
      cab->pos_livre = pos;
      escreve_no(arq, nodo, pos);
      free(nodo);
      return temp;
    } else if (nodo->dir == -1) {
      temp = nodo->esq;
      nodo->esq = cab->pos_livre;
      cab->pos_livre = pos;
      escreve_no(arq, nodo, pos);
      free(nodo);
      return temp;
    } else {
      int menor_pos = encontrar_minimo(arq, nodo->dir);
      no *menor = ler_no(arq, menor_pos);
      nodo->info = menor->info;
      nodo->dir = remover_recursivo(arq, nodo->dir, menor->info.codigo, cab);
      escreve_no(arq, nodo, pos);
      free(menor);
    }
  }

  escreve_no(arq, nodo, pos);
  free(nodo);
  return pos;
}

// Propósito - Remove um livro da árvore pelo código.
// Pré-Condições - O arquivo deve estar aberto e o código do livro deve ser
// válido. Pós-Condições - O livro é removido da árvore e o cabeçalho
// atualizado.
void remover(FILE *arq, int codigo) {
  cabecalho *cab = le_cabecalho(arq);
  cab->pos_raiz = remover_recursivo(arq, cab->pos_raiz, codigo, cab);
  escreve_cabecalho(arq, cab);
  free(cab);
}

// Propósito - Busca um livro pelo código e imprime seus dados.
// Pré-Condições - O arquivo deve estar aberto e o código do livro deve ser
// válido. Pós-Condições - Os dados do livro são impressos ou uma mensagem de
// não encontrado é exibida.
void imprimir_dados_livro(FILE *arq, int codigo) {
  cabecalho *cab = le_cabecalho(arq);
  int pos = cab->pos_raiz;
  no *nodo;

  while (pos != -1) {
    nodo = ler_no(arq, pos);
    if (codigo == nodo->info.codigo) {
      printf("Código: %d\nTítulo: %s\nAutor: %s\nEditora: %s\nEdição: %d\nAno: "
             "%d\nPreço: R$%.2f\nEstoque: %d\n",
             nodo->info.codigo, nodo->info.titulo, nodo->info.autor,
             nodo->info.editora, nodo->info.edicao, nodo->info.ano,
             nodo->info.preco, nodo->info.estoque);
      free(nodo);
      free(cab);
      return;
    }
    pos = (codigo < nodo->info.codigo) ? nodo->esq : nodo->dir;
    free(nodo);
  }

  printf("Livro não encontrado.\n");
  free(cab);
}

// Propósito - Lista todos os livros in-ordem.
// Pré-Condições - O arquivo deve estar aberto e a posição deve ser válida.
// Pós-Condições - Os livros são impressos em ordem crescente.
void listar_in_ordem(FILE *arq, int pos) {
  if (pos == -1)
    return;

  no *nodo = ler_no(arq, pos);
  listar_in_ordem(arq, nodo->esq);
  printf("Código: %d\nTítulo: %s\n\n", nodo->info.codigo, nodo->info.titulo);
  listar_in_ordem(arq, nodo->dir);

  free(nodo);
}

// Função para listar todos os livros
// Propósito - Inicia a listagem de todos os livros.
// Pré-Condições - O arquivo deve estar aberto.
// Pós-Condições - Todos os livros são listados em ordem
void listar_todos(FILE *arq) {
  cabecalho *cab = le_cabecalho(arq);
  listar_in_ordem(arq, cab->pos_raiz);
  free(cab);
}

// Função recursiva para calcular o total de nós na árvore
// Propósito - Conta o total de nós presentes na árvore binária.
// Pré-Condições - O arquivo deve estar aberto e a posição deve ser válida.
// Pós-Condições - O total é atualizado no ponteiro `total`.
void calcular_total_recursivo(FILE *arq, int pos, int *total) {
  if (pos == -1)
    return;
  no *nodo = ler_no(arq, pos);
  (*total)++;
  calcular_total_recursivo(arq, nodo->esq, total);
  calcular_total_recursivo(arq, nodo->dir, total);
  free(nodo);
}

// Função para calcular o total de livros cadastrados
// Propósito - Calcula o total de livros cadastrados.
// Pré-Condições - O arquivo deve estar aberto e a posição deve ser válida.
// Pós-Condições - O total de livros cadastrados é impresso.
void calcular_total(FILE *arq) {
  cabecalho *cab = le_cabecalho(arq);
  int total = 0;
  calcular_total_recursivo(arq, cab->pos_raiz, &total);
  printf("Total de livros cadastrados: %d\n", total);
  free(cab);
}

// Função para imprimir a lista de registros livres
// Propósito - Exibe a lista de registros livres disponíveis no arquivo.
// Pré-Condições - O arquivo deve estar aberto.
// Pós-Condições - A lista de registros livres é impressa na saída padrão.
void imprimir_lista_livres(FILE *arq) {
  cabecalho *cab = le_cabecalho(arq);
  int pos = cab->pos_livre;

  if (pos == -1) {
    printf("Nenhum registro livre disponível.\n");
  } else {
    printf("Lista de registros livres:\n");
    while (pos != -1) {
      printf("Posição: %d\n", pos);
      no *nodo = ler_no(arq, pos);
      pos = nodo->esq;
      free(nodo);
    }
  }

  free(cab);
}

// Função para remover espaços em branco do início e do fim da string
// Propósito - Elimina espaços em branco desnecessários em uma string.
// Pré-Condições - A string deve ser válida e não nula.
// Pós-Condições - A string é modificada para não ter espaços em branco no
// início e no fim.
void trim(char *str) {
  char *end;

  // Remove espaços em branco do início
  while (isspace((unsigned char)*str))
    str++;

  if (*str == 0) // Verifica se a string está vazia
    return;

  // Remove espaços em branco do final
  end = str + strlen(str) - 1;
  while (end > str && isspace((unsigned char)*end))
    end--;

  // Escreve o novo caractere nulo final
  *(end + 1) = 0;
}

// Função para carregar dados de um arquivo texto para o arquivo binário
// Propósito - Lê os dados de um arquivo de texto e os insere na árvore binária.
// Pré-Condições - O arquivo binário deve estar aberto e o arquivo de texto deve
// existir. Pós-Condições - Os dados dos livros são carregados no arquivo
// binário.
void carregar_arquivo(FILE *arq_bin, const char *nome_arquivo) {
  FILE *arq_txt = fopen(nome_arquivo, "r");
  if (!arq_txt) {
    perror("Erro ao abrir o arquivo de texto");
    return;
  }

  Livro livro;
  char linha[256];
  char *campo;

  // Lê o arquivo linha por linha
  while (fgets(linha, sizeof(linha), arq_txt)) {
    // Tokeniza a linha para cada campo do livro
    campo = strtok(linha, ";");
    if (campo) {
      livro.codigo = atoi(campo);
    }

    campo = strtok(NULL, ";");
    if (campo) {
      strncpy(livro.titulo, campo, sizeof(livro.titulo) - 1);
      trim(livro.titulo);
    }

    campo = strtok(NULL, ";");
    if (campo) {
      strncpy(livro.autor, campo, sizeof(livro.autor) - 1);
      trim(livro.autor);
    }

    campo = strtok(NULL, ";");
    if (campo) {
      strncpy(livro.editora, campo, sizeof(livro.editora) - 1);
      trim(livro.editora);
    }

    campo = strtok(NULL, ";");
    if (campo) {
      livro.edicao = atoi(campo);
    }

    campo = strtok(NULL, ";");
    if (campo) {
      livro.ano = atoi(campo);
    }

    campo = strtok(NULL, ";");
    if (campo) {
      // Substitui a vírgula por ponto se necessário
      for (char *p = campo; *p; p++) {
        if (*p == ',') {
          *p = '.';
        }
      }
      livro.preco = atof(campo); // Converte a string para float
    }

    campo = strtok(NULL, ";");
    if (campo) {
      livro.estoque = atoi(campo);
    }

    // Insere o livro na árvore binária
    insere(arq_bin, &livro);
  }

  fclose(arq_txt);
  printf("Dados carregados do arquivo %s com sucesso.\n", nome_arquivo);
}

// Função recursiva para buscar um livro por título
// Propósito - Realiza a busca de um livro pelo título na árvore binária.
// Pré-Condições - O arquivo deve estar aberto e a posição do nó deve ser
// válida. Pós-Condições - O livro é encontrado e suas informações são
// impressas.
void busca_por_titulo_rec(FILE *arq, int pos_no, const char *titulo) {
  // Caso base: se a posição do nó for -1, estamos em um nó folha
  if (pos_no == -1) {
    return;
  }

  // Ler o nó na posição atual
  no *atual = ler_no(arq, pos_no);

  // Verificar se o título corresponde
  if (strcmp(atual->info.titulo, titulo) == 0) {
    printf("Livro encontrado:\n");
    printf("Código: %d\n", atual->info.codigo);
    printf("Título: %s\n", atual->info.titulo);
    printf("Autor: %s\n", atual->info.autor);
    printf("Editora: %s\n", atual->info.editora);
    printf("Edição: %d\n", atual->info.edicao);
    printf("Ano: %d\n", atual->info.ano);
    printf("Preço: R$%.2f\n", atual->info.preco);
    printf("Estoque: %d\n", atual->info.estoque);
    printf("\n");
  }

  // Continuar a busca na subárvore apropriada
  if (strcmp(titulo, atual->info.titulo) < 0) {
    busca_por_titulo_rec(arq, atual->esq, titulo); // Esquerda
  } else {
    busca_por_titulo_rec(arq, atual->dir, titulo); // Direita
  }

  // Libera a memória do nó lido
  free(atual);
}

// Função para buscar um livro por título
// Propósito - Inicia a busca de um livro pelo título na árvore binária.
// Pré-Condições - O arquivo deve estar aberto.
// Pós-Condições - A busca é realizada e as informações do livro são impressas
// se encontrado.
void busca_por_titulo(FILE *arq, const char *titulo) {
  // Ler o cabeçalho para obter a posição da raiz
  cabecalho cab;
  fseek(arq, 0, SEEK_SET);
  fread(&cab, sizeof(cabecalho), 1, arq);

  // Função recursiva interna para fazer a busca na árvore
  busca_por_titulo_rec(arq, cab.pos_raiz, titulo);
}

// Função recursiva para buscar livros por autor
// Propósito - Realiza a busca de livros pelo autor na árvore binária.
// Pré-Condições - O arquivo deve estar aberto e a posição do nó deve ser
// válida. Pós-Condições - Os livros do autor são encontrados e seus títulos são
// impressos.
void busca_por_autor_rec(FILE *arq, int pos_no, const char *autor) {
  // Caso base: se a posição do nó for -1, estamos em um nó folha
  if (pos_no == -1) {
    return;
  }

  // Ler o nó na posição atual
  no *atual = ler_no(arq, pos_no);

  // Verificar se o autor corresponde
  if (strcmp(atual->info.autor, autor) == 0) {
    printf("Título: %s\n", atual->info.titulo);
  }

  // Continuar a busca na subárvore esquerda e direita
  busca_por_autor_rec(arq, atual->esq, autor); // Esquerda
  busca_por_autor_rec(arq, atual->dir, autor); // Direita

  // Libera a memória do nó lido
  free(atual);
}

// Função para buscar livros por autor
// Propósito - Inicia a busca de livros pelo autor na árvore binária.
// Pré-Condições - O arquivo deve estar aberto.
// Pós-Condições - A busca é realizada e os títulos dos livros do autor são
// impressos.
void busca_por_autor(FILE *arq, const char *autor) {
  // Ler o cabeçalho para obter a posição da raiz
  cabecalho cab;
  fseek(arq, 0, SEEK_SET);
  fread(&cab, sizeof(cabecalho), 1, arq);

  // Função recursiva interna para fazer a busca na árvore
  busca_por_autor_rec(arq, cab.pos_raiz, autor);
}

// Função para imprimir a árvore por níveis (level-order)
// Propósito - Exibe todos os livros em nível da árvore binária.
// Pré-Condições - O arquivo deve estar aberto.
// Pós-Condições - Os livros são impressos na saída padrão em níve
void imprimir_por_niveis(FILE *arq) {
  // Lê o cabeçalho para encontrar a posição da raiz
  cabecalho *cab = le_cabecalho(arq);
  if (cab->pos_raiz == -1) {
    printf("A árvore está vazia.\n");
    return;
  }

  // Cria uma fila para armazenar as posições dos nós
  int fila[1000]; // Exemplo de tamanho fixo
  int inicio = 0, fim = 0;

  // Insere a raiz na fila
  fila[fim++] = cab->pos_raiz;

  while (inicio < fim) {
    int nivel_count = fim - inicio; // Contagem de nós no nível atual

    // Processa todos os nós do nível atual
    for (int i = 0; i < nivel_count; i++) {
      int pos = fila[inicio++];
      no *n = ler_no(arq, pos);

      // Imprime o código do livro
      printf("%d ", n->info.codigo);

      // Adiciona os filhos do nó atual à fila, se existirem
      if (n->esq != -1)
        fila[fim++] = n->esq;
      if (n->dir != -1)
        fila[fim++] = n->dir;

      // Libera a memória do nó lido
      free(n);
    }
    printf("\n"); // Quebra de linha para o próximo nível
  }

  // Libera o cabeçalho
  free(cab);
}

// Função recursiva auxiliar para imprimir o formato da árvore
// Propósito - Imprime a estrutura da árvore binária em um formato legível,
// exibindo a chave e as subárvores esquerda e direita. Pré-Condições - O
// arquivo deve estar aberto e a posição do nó deve ser válida. Pós-Condições -
// A estrutura da árvore é impressa na saída padrão.
void imprimir_no_formatado(FILE *arq, int pos) {
  if (pos == -1) {
    // Subárvore vazia
    printf("[ ]");
    return;
  }

  no *nodo = ler_no(arq, pos);

  if (nodo != NULL) {
    // Imprime o formato [chave, [esq, dir]]
    printf("[%d, ", nodo->info.codigo);

    // Subárvore à esquerda
    imprimir_no_formatado(arq, nodo->esq);
    printf(", ");

    // Subárvore à direita
    imprimir_no_formatado(arq, nodo->dir);

    printf("]");
  }
}

// Função para imprimir a árvore formatada como lista
// Propósito - Inicia a impressão da árvore em formato de lista.
// Pré-Condições - O arquivo deve estar aberto.
// Pós-Condições - Os livros são impressos na saída padrão em formato de lista.
void imprimir_arvore_formatada_como_lista(FILE *arq) {
  cabecalho *cab = le_cabecalho(arq);
  if (cab == NULL || cab->pos_raiz == -1) {
    printf("[ ]\n"); // Árvore vazia
    return;
  }

  imprimir_no_formatado(arq, cab->pos_raiz);
  printf("\n");
}
