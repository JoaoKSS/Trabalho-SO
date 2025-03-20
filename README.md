<h2>Sistemas Operacionais 2024/2</h2>

<h3>Sobre o Trabalho</h3>
Este projeto é uma implementação do algoritmo de compressão e descompressão de arquivos utilizando a codificação de Huffman, desenvolvido em C++. Ele demonstra conceitos como:
- Leitura de arquivos em blocos.
- Construção e manipulação de árvores de Huffman.
- Utilização de heaps para gerenciamento de frequências de bytes.
- Processamento binário de dados para compressão.

<h3>Estrutura do Projeto</h3>
- **main.cpp**: Responsável pela interface de linha de comando e execução das funções de compressão e descompressão.  
- **huffman.cpp**: Implementa as funções principais, incluindo a construção da árvore de Huffman, geração da tabela de códigos, compressão e descompressão.  
- **huffman.h**: Contém as declarações das funções e definição da estrutura **NodeArvore**.  
- **makefile**: Automatiza a compilação e execução dos testes utilizando **g++**.

<h3>Requisitos</h3>
<ul>
  <li>C++11 ou superior</li>
  <li>Compilador g++</li>
  <li>Valgrind (opcional para verificação de memória)</li>
</ul>

<h3>Como Compilar</h3>
Para compilar o projeto, abra o terminal no diretório do projeto e execute:
```sh
make
