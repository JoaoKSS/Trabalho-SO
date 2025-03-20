#include "huffman.h"
#include <iostream>
#include <cstring>
using namespace std;

int main(int argc, char *argv[]) {
    // Troca de arquivos para compactação no Makefile e execução usando "Make run" e "Make Clean"!
    
    if (argc < 4) {
        cout << "Uso: huffman [C|D] [ARQUIVO_ENTRADA] [ARQUIVO_SAIDA]" << endl;
        cout << "C: Compressao, D: Descompressao" << endl;
        return 1;
    }

    if (strcmp(argv[1], "C") == 0) {
        // Verifica se o arquivo de saída possui a extensão .huff
        if (strstr(argv[3], ".huff") != NULL) {
            CompressFile(argv[2], argv[3]);
        } else {
            cout << "O arquivo resultante da compressao deve ter a extensao '.huff'." << endl;
            return 1;
        }
    }
    else if (strcmp(argv[1], "D") == 0) {
        // Verifica se o arquivo de entrada possui a extensão .huff
        if (strstr(argv[2], ".huff") != NULL) {
            DecompressFile(argv[2], argv[3]);
        } else {
            cout << "O arquivo a ser descomprimido deve ter a extensao '.huff'." << endl;
            return 1;
        }
    }
    else {
        cout << "Opcao invalida. Use C para compressao e D para descompressao." << endl;
        return 1;
    }
    return 0;
}
