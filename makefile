CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++11 -O2 -g

TARGET = huffman
OBJS = main.o huffman.o

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS)

main.o: main.cpp huffman.h
	$(CXX) $(CXXFLAGS) -c main.cpp

huffman.o: huffman.cpp huffman.h
	$(CXX) $(CXXFLAGS) -c huffman.cpp

# Trocar aqui para os arquivos desejados para compressão e descompressão
run: $(TARGET)
	@echo ""
	@echo "Executando compressão:"
	valgrind ./huffman C teste1.txt output.huff
	@echo ""
	@echo "Executando descompressão:"
	valgrind ./huffman D output.huff output.txt
	@echo ""

clean:
	rm -f $(TARGET) $(OBJS)
	rm -f output.huff
