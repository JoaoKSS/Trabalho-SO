#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <ctime>
#include <chrono>
#include <fstream> 

using namespace std;
typedef unsigned char byte;

// Estrutura que representa um nó na árvore de Huffman
struct NodeArvore {
    int frequencia;         
    byte c;                 
    NodeArvore* esquerda;   
    NodeArvore* direita;    
};

// Cria um novo nó da árvore de Huffman com os valores fornecidos
NodeArvore* novoNodeArvore(byte c, int frequencia, NodeArvore* esquerda, NodeArvore* direita) {
    NodeArvore* novo = new (nothrow) NodeArvore;
    if (!novo)
        return nullptr;
    novo->c = c;
    novo->frequencia = frequencia;
    novo->esquerda = esquerda;
    novo->direita = direita;
    return novo;
}

// Libera recursivamente a memória alocada para a árvore de Huffman
void FreeHuffmanTree(NodeArvore* n) {
    if (!n) return;
    FreeHuffmanTree(n->esquerda);
    FreeHuffmanTree(n->direita);
    delete n;
}

// Leitura em blocos para calcular a frequência de cada byte presente no arquivo
void getByteFrequency(ifstream &entrada, unsigned int* listaBytes) {
    const size_t BUFFER_SIZE = 262144; // 256 KB
    byte buffer[BUFFER_SIZE];
    size_t bytesRead;    
    // Inicializa o vetor de frequência com zeros
    memset(listaBytes, 0, 256 * sizeof(unsigned int));
    // Lê o arquivo em blocos e atualiza as frequências dos bytes lidos
    while (true) {
        entrada.read(reinterpret_cast<char*>(buffer), BUFFER_SIZE);
        bytesRead = entrada.gcount();
        if (bytesRead == 0)
            break;
        for (size_t i = 0; i < bytesRead; i++) {
            listaBytes[ buffer[i] ]++;
        }
    }
    // Volta o ponteiro do arquivo para o início
    entrada.clear();
    entrada.seekg(0, ios::beg);
}

// Função auxiliar que troca dois nós do heap utilizado no heapify e inserção
static void swapNode(NodeArvore** a, NodeArvore** b) {
    NodeArvore* temp = *a;
    *a = *b;
    *b = temp;
}

// Reorganiza o heap a partir do índice i para manter a propriedade do min-heap
static void heapify(NodeArvore* heap[], int size, int i) {
    int smallest = i;
    int left = 2 * i + 1;
    int right = 2 * i + 2;
    // Verifica se o filho da esquerda existe e é menor que o nó atual
    if (left < size && heap[left]->frequencia < heap[smallest]->frequencia)
        smallest = left;
    // Verifica se o filho da direita existe e é menor que o menor atual
    if (right < size && heap[right]->frequencia < heap[smallest]->frequencia)
        smallest = right;
    // Se o menor não for o nó atual, troca e continua o heapify recursivamente
    if (smallest != i) {
        swapNode(&heap[i], &heap[smallest]);
        heapify(heap, size, smallest);
    }
}

// Constrói o min-heap a partir do vetor heap
static void buildHeap(NodeArvore* heap[], int size) {
    int startIdx = (size / 2) - 1;
    for (int i = startIdx; i >= 0; i--) {
        heapify(heap, size, i);
    }
}

// Remove e retorna o nó com a menor frequência do heap
static NodeArvore* extractMin(NodeArvore* heap[], int* size) {
    if (*size <= 0)
        return NULL;
    NodeArvore* root = heap[0];
    heap[0] = heap[*size - 1];
    (*size)--;
    heapify(heap, *size, 0);
    return root;
}

// Insere um novo nó no heap e reorganiza para manter a propriedade de min-heap
static void insertHeap(NodeArvore* heap[], int* size, NodeArvore* node) {
    int i = *size;
    heap[i] = node;
    (*size)++;
    while (i != 0) {
        int parent = (i - 1) / 2;
        if (heap[parent]->frequencia <= heap[i]->frequencia)
            break;
        swapNode(&heap[i], &heap[parent]);
        i = parent;
    }
}

// Construção da Árvore de Huffman e retorna o ponteiro para a raiz da árvore de Huffman
NodeArvore* BuildHuffmanTree(unsigned int* listaBytes) {
    NodeArvore* heap[256];
    int heapSize = 0;
    // Cria um nó para cada caractere que aparece no arquivo (frequência > 0)
    for (int i = 0; i < 256; i++) {
        if (listaBytes[i] > 0) {
            NodeArvore* node = novoNodeArvore((byte)i, listaBytes[i], NULL, NULL);
            heap[heapSize++] = node;
        }
    } 
    if (heapSize == 0) return NULL;
    buildHeap(heap, heapSize);
    // Combina os nós de menor frequência até restar apenas a raiz
    while (heapSize > 1) {
        NodeArvore* nodeEsquerdo = extractMin(heap, &heapSize);
        NodeArvore* nodeDireito = extractMin(heap, &heapSize);
        NodeArvore* soma = novoNodeArvore('#', nodeEsquerdo->frequencia + nodeDireito->frequencia, nodeEsquerdo, nodeDireito);
        insertHeap(heap, &heapSize, soma);
    }
    return heap[0];
}

// Função recursiva para percorre a árvore de Huffman e atribui um código (sequência de '0' e '1')
void buildCodeTableHelper(NodeArvore* node, char* code, int depth, char* codeTable[256]) {
    // Se o nó atual for folha, armazena o código gerado
    if (!node->esquerda && !node->direita) {
        code[depth] = '\0';  // Finaliza a string
        codeTable[node->c] = new char[depth + 1];
        strcpy(codeTable[node->c], code);
        return;
    }
    // Se existir filho à esquerda, adiciona '0' ao código e continua a recursão
    if (node->esquerda) {
        code[depth] = '0';
        buildCodeTableHelper(node->esquerda, code, depth + 1, codeTable);
    }
    // Se existir filho à direita, adiciona '1' ao código e continua a recursão
    if (node->direita) {
        code[depth] = '1';
        buildCodeTableHelper(node->direita, code, depth + 1, codeTable);
    }
}

// Inicializa e constrói a Tabela de Códigos para cada caractere com base na árvore de Huffman.
void buildCodeTable(NodeArvore* root, char* codeTable[256]) {
    char code[256]; // Vetor armazenar o código temporariamente
    for (int i = 0; i < 256; i++) {
        codeTable[i] = NULL;
    }
    if (root)
        buildCodeTableHelper(root, code, 0, codeTable); // recursão para construir os códigos
}

// Função que realiza a compressão do arquivo de entrada utilizando a árvore de Huffman e gera um arquivo de saída compactado.
void CompressFile(const char* arquivoEntrada, const char* arquivoSaida) {
    auto inicio = chrono::high_resolution_clock::now(); 
    unsigned int listaBytes[256] = {0};
    // Abre o arquivo de entrada em modo binário para leitura
    ifstream entrada(arquivoEntrada, ios::binary);
    if (!entrada) {
        cerr << "Arquivo nao encontrado: " << arquivoEntrada << endl;
        exit(1);
    }
    // Abre o arquivo de saída em modo binário para escrita
    ofstream saida(arquivoSaida, ios::binary);
    if (!saida) {
        cerr << "Erro ao abrir arquivo de saida: " << arquivoSaida << endl;
        exit(1);
    }
    // Construção da tabela de frequência de bytes
    getByteFrequency(entrada, listaBytes);
    // Construção da árvore de Huffman com base nas frequências
    NodeArvore* raiz = BuildHuffmanTree(listaBytes);
    if (!raiz) {
        fprintf(stderr, "Erro: arvore de Huffman nao pode ser construída.\n");
        exit(1);
    }
    // Construção da tabela de códigos (mapa caractere -> código binário)
    char* codeTable[256];
    buildCodeTable(raiz, codeTable);
    // Criação do arquivo compactado
    // Reserva 1 byte para armazenar a quantidade de bits não utilizados no último byte
    byte unused_placeholder = 0;
    saida.write(reinterpret_cast<const char*>(&unused_placeholder), 1);
    // Grava a tabela de frequências no arquivo (necessária para a descompressão)
    saida.write(reinterpret_cast<const char*>(listaBytes), 256 * sizeof(unsigned int));

    unsigned int total_bits = 0;
    byte aux = 0; // agrupa bits

    // Definindo buffer de saída de 256 KB
    const size_t OUTPUT_BUFFER_SIZE = 262144; 
    byte outBuffer[OUTPUT_BUFFER_SIZE];
    size_t outBufferIndex = 0;

    // Lê o arquivo em blocos e escreve os bits correspondentes ao código de cada caractere
    const size_t BUFFER_SIZE = 262144;
    byte buffer[BUFFER_SIZE];
    size_t bytesRead;
    entrada.clear();
    entrada.seekg(0, ios::beg); // Retorna ao início do arquivo de entrada

    while (true) {
        // Lê um bloco de dados do arquivo de entrada para o buffer
        entrada.read(reinterpret_cast<char*>(buffer), BUFFER_SIZE);
        
        // Obtém o número real de bytes lidos 
        bytesRead = entrada.gcount();
        if (bytesRead == 0)
            break;
    
        // Percorre todos os bytes lidos do buffer
        for (size_t i = 0; i < bytesRead; i++) {
            byte c = buffer[i];  // Obtém o byte atual
            
            // Obtém o código binário correspondente ao byte na tabela de Huffman
            char* bits = codeTable[c]; 
    
            // Percorre cada bit do código binário do caractere
            for (int j = 0; bits[j] != '\0'; j++) {
                // Se o bit atual for '1', define o bit correspondente na variável auxiliar `aux`
                if (bits[j] == '1') {
                    aux |= (1 << (total_bits % 8)); // Define o bit na posição correspondente
                }
    
                total_bits++; 
                
                // Se tivermos acumulado 8 bits, adiciona o byte completo ao buffer de saída
                if (total_bits % 8 == 0) {
                    outBuffer[outBufferIndex++] = aux; // Adiciona o byte ao buffer de saída
                    // Se o buffer de saída estiver cheio, grava seu conteúdo no arquivo e reinicia o índice
                    if (outBufferIndex == OUTPUT_BUFFER_SIZE) {
                        saida.write(reinterpret_cast<const char*>(outBuffer), OUTPUT_BUFFER_SIZE);
                        outBufferIndex = 0;
                    }
                    aux = 0; 
                }
            }
        }
    }
    
    // Se o último byte não estiver completo, adiciona o byte ao buffer de saída
    if (total_bits % 8 != 0) {
        outBuffer[outBufferIndex++] = aux;
    }
    // Grava qualquer dado restante no buffer de saída para o arquivo
    if (outBufferIndex > 0) {
        saida.write(reinterpret_cast<const char*>(outBuffer), outBufferIndex);
    }
    // Calcula a quantidade de bits não utilizados no último byte
    byte unused = (total_bits % 8 == 0) ? 0 : (8 - (total_bits % 8));
    // Retorna ao início do arquivo de saída para atualizar o byte de bits não utilizados

    auto final = chrono::high_resolution_clock::now();
    chrono::duration<double> tempoGasto = final - inicio;
    
    saida.flush();
    saida.seekp(0, ios::beg);
    saida.write(reinterpret_cast<const char*>(&unused), 1);
 
    // Estatísticas de compressão
    entrada.clear();
    entrada.seekg(0, ios::end);
    double tamanhoEntrada = entrada.tellg();
    saida.flush();
    saida.seekp(0, ios::end);
    double tamanhoSaida = saida.tellp();

    cout << "Arquivo de entrada: " << arquivoEntrada << " (" << tamanhoEntrada << " bytes)" << endl;
    cout << "Arquivo de saida: " << arquivoSaida << " (" << tamanhoSaida << " bytes)" << endl;
    cout << "Tempo gasto: " << tempoGasto.count() << " s" << endl;
    if (tamanhoEntrada > 0)
        cout << "Taxa de compressao: " << static_cast<int>((100 * tamanhoSaida) / tamanhoEntrada) << "%" << endl;

    for (int i = 0; i < 256; i++) {
        if (codeTable[i] != NULL) {
            delete[] codeTable[i];
        }
    }
    FreeHuffmanTree(raiz);
    entrada.close();
    saida.close();
}

// Função que reconstrói o arquivo original a partir do arquivo compactado, utilizando a árvore de Huffman armazenada no cabeçalho.
void DecompressFile(const char* arquivoEntrada, const char* arquivoSaida) {
    auto inicio = chrono::high_resolution_clock::now(); 
    unsigned int listaBytes[256] = {0};
    // Abre o arquivo compactado para leitura em modo binário
    ifstream entrada(arquivoEntrada, ios::binary);
    if (!entrada.is_open()) {
        cerr << "Arquivo nao encontrado: " << arquivoEntrada << endl;
        exit(1);
    }
    // Abre o arquivo de saída para escrita em modo binário
    ofstream saida(arquivoSaida, ios::binary);
    if (!saida.is_open()) {
        cerr << "Erro ao abrir arquivo de saida: " << arquivoSaida << endl;
        exit(1);
    }
    // Lê o byte que indica a quantidade de bits não preenchidos no último byte
    byte unused;
    entrada.read(reinterpret_cast<char*>(&unused), 1);
    if (entrada.gcount() != 1) {
        cerr << "Erro ao ler o byte de lixo" << endl;
        return;
    }
    // Lê a tabela de frequência (vetor de 256 inteiros) armazenada no cabeçalho
    entrada.read(reinterpret_cast<char*>(listaBytes), 256 * sizeof(unsigned int));
    if (entrada.gcount() != 256 * (streamsize)sizeof(unsigned int)) {
        cerr << "Erro ao ler a tabela de frequências" << endl;
        return;
    }
    // Reconstrói a árvore de Huffman com base na tabela de frequências lida
    NodeArvore* raiz = BuildHuffmanTree(listaBytes);
    if (!raiz) {
        cerr << "Erro: arvore de Huffman nao pode ser reconstruida." << endl;
        exit(1);
    }
    // Calcula o total de bits dos dados comprimidos: total_bits = (número de bytes de dados * 8) - bits não utilizados
    entrada.clear();
    entrada.seekg(0, ios::end);
    long file_size = entrada.tellg();
    int header_size = 1 + 256 * sizeof(unsigned int);
    long data_bytes = file_size - header_size;
    unsigned long total_bits = data_bytes * 8 - unused;
    entrada.clear();
    entrada.seekg(header_size, ios::beg); // Retorna ao início dos dados comprimidos

    // Configura buffers para leitura (bits) e escrita (dados descomprimidos)
    const size_t INPUT_BUFFER_SIZE = 262144;
    byte inBuffer[INPUT_BUFFER_SIZE];
    size_t bytesRead = 0;
    size_t byteIndex = 0; // Índice do byte atual no inBuffer
    int bitIndex = 0; // Índice do bit atual dentro de inBuffer[byteIndex]
    unsigned long posicao = 0; // Contador de bits processados

    const size_t OUTPUT_BUFFER_SIZE = 262144;
    byte outBuffer[OUTPUT_BUFFER_SIZE];
    int outBufferIndex = 0;  

    // Lê o primeiro bloco do arquivo comprimido
    entrada.read(reinterpret_cast<char*>(inBuffer), INPUT_BUFFER_SIZE);
    bytesRead = entrada.gcount();
    byteIndex = 0;
    bitIndex = 0;

    NodeArvore* nodeAtual = raiz;
    // Percorre todos os bits do arquivo comprimido e reconstrói o arquivo original
    while (posicao < total_bits) {
        // Se todos os bytes do buffer foram processados, lê um novo bloco do arquivo
        if (byteIndex >= bytesRead) {
            entrada.read(reinterpret_cast<char*>(inBuffer), INPUT_BUFFER_SIZE);
            bytesRead = entrada.gcount();
            byteIndex = 0;
            bitIndex = 0;
            if (bytesRead == 0) break; // Caso inesperado de fim de arquivo
        }
        // Extrai o bit atual do buffer
        int bit = (inBuffer[byteIndex] >> bitIndex) & 1;
        bitIndex++;
        posicao++;
        // Se 8 bits já foram lidos deste byte, avança para o próximo byte
        if (bitIndex == 8) {
            bitIndex = 0;
            byteIndex++;
        }
        // Percorre a árvore de Huffman: vai para a direita se o bit for 1 ou para a esquerda se for 0
        nodeAtual = bit ? nodeAtual->direita : nodeAtual->esquerda;
        // Se o nó atual for folha, temos um caractere decodificado
        if (!nodeAtual->esquerda && !nodeAtual->direita) {
            outBuffer[outBufferIndex++] = nodeAtual->c;
            // Se o buffer de saída estiver cheio, escreve seu conteúdo no arquivo de saída
            if (outBufferIndex == (int)OUTPUT_BUFFER_SIZE) {
                saida.write(reinterpret_cast<const char*>(outBuffer), outBufferIndex);
                outBufferIndex = 0;
            }
            // Volta para a raiz para decodificar o próximo caractere
            nodeAtual = raiz;
        }
    }

    // Escreve qualquer dado restante no buffer de saída para o arquivo
    if (outBufferIndex > 0) {
        saida.write(reinterpret_cast<const char*>(outBuffer), outBufferIndex);
    }
    FreeHuffmanTree(raiz);

    auto final = chrono::high_resolution_clock::now();
    chrono::duration<double> tempoGasto = final - inicio;

    entrada.clear();
    entrada.seekg(0, ios::end);
    double tamanhoEntrada = entrada.tellg();
    saida.flush();
    saida.seekp(0, ios::end);
    double tamanhoSaida = saida.tellp();

    cout << "Arquivo de entrada: " << arquivoEntrada << " (" << tamanhoEntrada << " bytes)" << endl;
    cout << "Arquivo de saida: " << arquivoSaida << " (" << tamanhoSaida << " bytes)" << endl;
    cout << "Tempo gasto: " << tempoGasto.count() << " s" << endl;
    if (tamanhoEntrada > 0)
        cout << "Taxa de descompressao: " << static_cast<int>((100 * tamanhoSaida) / tamanhoEntrada) << "%" << endl;
    entrada.close();
    saida.close();
}
