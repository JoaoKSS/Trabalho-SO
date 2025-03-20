#ifndef HUFFMAN_H
#define HUFFMAN_H

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <ctime>
#include <fstream>

// Tipo de dado para representar um byte
typedef unsigned char byte;

// Estrutura da árvore de Huffman
struct NodeArvore {
    int frequencia;
    byte c;
    NodeArvore* esquerda;
    NodeArvore* direita;
};

// Declaração das funções utilizadas na compressão e descompressão

// Cria um novo nó da árvore de Huffman
NodeArvore* novoNodeArvore(byte c, int frequencia, NodeArvore* esquerda, NodeArvore* direita);

// Calcula a frequência de cada byte a partir de um arquivo de entrada
void getByteFrequency(std::ifstream &entrada, unsigned int* listaBytes);

// Constrói a árvore de Huffman a partir da tabela de frequências
NodeArvore* BuildHuffmanTree(unsigned int* listaBytes);

// Libera a memória alocada para a árvore de Huffman
void FreeHuffmanTree(NodeArvore* n);

// Funções auxiliares para construção da tabela de códigos
void buildCodeTableHelper(NodeArvore* node, char* code, int depth, char* codeTable[256]);
void buildCodeTable(NodeArvore* root, char* codeTable[256]);

// Funções de compressão e descompressão de arquivos
void CompressFile(const char* arquivoEntrada, const char* arquivoSaida);
void DecompressFile(const char* arquivoEntrada, const char* arquivoSaida);
#endif


