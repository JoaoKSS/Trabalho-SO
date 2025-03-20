class BTreeNode:
    def __init__(self, m, leaf=False, parent=None):
        """
        Inicializa um nó da Árvore B.
        
        Parâmetros:
            m: mínimo de chaves em nós não‑raiz (usado como m = ordem - 1)
            leaf: True se o nó for folha
            parent: referência para o nó pai (None se raiz)
        """
        self.m = m                # mínimo de chaves (exceto raiz)
        self.leaf = leaf          # indica se é nó folha
        self.keys = []            # lista de pares (chave, endereço)
        self.children = []        # lista de filhos (BTreeNode)
        self.parent = parent      # referência para o nó pai

    def search(self, key):
        """
        Pesquisa recursivamente a chave na subárvore enraizada neste nó.
        Retorna (nó, índice) se encontrada ou None.
        """
        i = 0
        while i < len(self.keys) and key > self.keys[i][0]:
            i += 1
        if i < len(self.keys) and self.keys[i][0] == key:
            return (self, i)
        if self.leaf:
            return None
        return self.children[i].search(key)

    def find_key_index(self, key):
        """
        Retorna o índice do primeiro elemento em self.keys cuja chave seja >= key.
        """
        idx = 0
        while idx < len(self.keys) and self.keys[idx][0] < key:
            idx += 1
        return idx

    def insert_nonfull(self, key, address):
        """
        Insere o par (chave, endereço) neste nó, assumindo que ele não está cheio.
        Se, após a inserção, o nó atingir 2*m+1 chaves, a divisão é feita imediatamente.
        Retorna nova raiz se houver divisão da raiz.
        """
        if self.leaf:
            i = len(self.keys) - 1
            self.keys.append(None)
            while i >= 0 and key < self.keys[i][0]:
                self.keys[i+1] = self.keys[i]
                i -= 1
            self.keys[i+1] = (key, address)
            if len(self.keys) == 2 * self.m + 1:
                if self.parent is not None:
                    idx = self.parent.children.index(self)
                    self.parent.split_child(idx)
                else:
                    new_root = BTreeNode(self.m, leaf=False)
                    new_root.children.append(self)
                    self.parent = new_root
                    new_root.split_child(0)
                    return new_root
            return None
        else:
            i = len(self.keys) - 1
            while i >= 0 and key < self.keys[i][0]:
                i -= 1
            i += 1
            if len(self.children[i].keys) == 2 * self.m + 1:
                self.split_child(i)
                if key > self.keys[i][0]:
                    i += 1
            ret = self.children[i].insert_nonfull(key, address)
            if ret is not None:
                return ret
            if len(self.keys) == 2 * self.m + 1:
                if self.parent is not None:
                    idx = self.parent.children.index(self)
                    self.parent.split_child(idx)
                else:
                    new_root = BTreeNode(self.m, leaf=False)
                    new_root.children.append(self)
                    self.parent = new_root
                    new_root.split_child(0)
                    return new_root
            return None

    def split_child(self, i):
        """
        Divide o filho self.children[i] que está cheio (com 2*m+1 chaves).
        Cria um novo nó que receberá as chaves de índice m+1 em diante e promove
        a chave mediana (índice m) para este nó.
        """
        m = self.m
        y = self.children[i]                      # nó a ser dividido
        z = BTreeNode(m, leaf=y.leaf, parent=self)  # novo nó que receberá as chaves finais
        mid_key = y.keys[m]                       # chave mediana a ser promovida

        # z recebe as chaves com índices m+1 até fim (total m chaves)
        z.keys = y.keys[m+1:]
        # Se não for folha, transfere também os filhos
        if not y.leaf:
            z.children = y.children[m+1:]
            for child in z.children:
                child.parent = z
            y.children = y.children[:m+1]
        # y mantém as chaves de índice 0 até m-1 (total m chaves)
        y.keys = y.keys[:m]
        # Insere z como filho deste nó, logo após y, e promove a chave mediana
        self.children.insert(i+1, z)
        self.keys.insert(i, mid_key)

    def remove(self, key):
        """
        Remove recursivamente o par (chave, endereço) da subárvore enraizada neste nó.
        Se após a remoção um nó (não raiz) ficar com menos que m chaves, aciona o fill
        e propaga a correção para os nós pais.
        """
        idx = self.find_key_index(key)
        if idx < len(self.keys) and self.keys[idx][0] == key:
            if self.leaf:
                self.remove_from_leaf(idx)
                # Se não for raiz e o nó ficou deficiente, propaga a correção para cima.
                if self.parent is not None and len(self.keys) < self.m:
                    idx_p = self.parent.children.index(self)
                    self.parent.fill(idx_p)
                    self.parent.fix_deficiency_upwards()
            else:
                self.remove_from_nonleaf(idx)
        else:
            if self.leaf:
                return  # chave não encontrada
            # Apenas forçamos o fill se o filho não contiver a chave
            if self.children[idx].search(key) is None and len(self.children[idx].keys) == self.m:
                self.fill(idx)
            self.children[idx].remove(key)

    def remove_from_leaf(self, idx):
        """Remove a chave de índice idx de um nó folha."""
        del self.keys[idx]

    def remove_from_nonleaf(self, idx):
        """
        Remove a chave de índice idx de um nó não-folha.
        Se o filho à esquerda ou o da direita tiver pelo menos m chaves,
        usa o predecessor ou o sucessor, respectivamente.
        Caso ambos tenham exatamente m chaves, mescla os dois filhos.
        """
        key = self.keys[idx][0]
        if len(self.children[idx].keys) >= self.m:
            pred = self.get_predecessor(idx)
            self.keys[idx] = pred
            self.children[idx].remove(pred[0])
        elif len(self.children[idx+1].keys) >= self.m:
            succ = self.get_successor(idx)
            self.keys[idx] = succ
            self.children[idx+1].remove(succ[0])
        else:
            self.merge(idx)
            self.children[idx].remove(key)

    def get_predecessor(self, idx):
        """Retorna o par (chave, endereço) predecessor da chave self.keys[idx]."""
        cur = self.children[idx]
        while not cur.leaf:
            cur = cur.children[-1]
        return cur.keys[-1]

    def get_successor(self, idx):
        """Retorna o par (chave, endereço) sucessor da chave self.keys[idx]."""
        cur = self.children[idx+1]
        while not cur.leaf:
            cur = cur.children[0]
        return cur.keys[0]

    def fill(self, idx):
        """
        Garante que o filho self.children[idx] tenha pelo menos m+1 chaves.
        Se necessário, realiza empréstimo ou mescla com irmão.
        Agora, se ambos os irmãos não tiverem chave extra, mescla preferencialmente com o irmão à esquerda.
        """
        if idx != 0 and len(self.children[idx-1].keys) > self.m:
            self.borrow_from_prev(idx)
        elif idx != len(self.keys) and len(self.children[idx+1].keys) > self.m:
            self.borrow_from_next(idx)
        else:
            if idx != 0:
                self.merge(idx-1)
            else:
                self.merge(idx)

    def borrow_from_prev(self, idx):
        """Emprsta uma chave do irmão à esquerda para self.children[idx]."""
        child = self.children[idx]
        sibling = self.children[idx-1]
        child.keys.insert(0, self.keys[idx-1])
        if not child.leaf:
            child.children.insert(0, sibling.children.pop())
            child.children[0].parent = child
        self.keys[idx-1] = sibling.keys.pop()

    def borrow_from_next(self, idx):
        """Emprsta uma chave do irmão à direita para self.children[idx]."""
        child = self.children[idx]
        sibling = self.children[idx+1]
        child.keys.append(self.keys[idx])
        if not child.leaf:
            child.children.append(sibling.children.pop(0))
            child.children[-1].parent = child
        self.keys[idx] = sibling.keys.pop(0)

    def merge(self, idx):
        """
        Mescla o filho self.children[idx] com o irmão à direita (self.children[idx+1]),
        movendo a chave separadora do nó atual para o nó mesclado.
        """
        child = self.children[idx]
        sibling = self.children[idx+1]
        child.keys.append(self.keys[idx])
        child.keys.extend(sibling.keys)
        if not child.leaf:
            child.children.extend(sibling.children)
            for c in sibling.children:
                c.parent = child
        del self.keys[idx]
        del self.children[idx+1]

    def rotate_internal_left(self):
        """
        Realiza uma rotação interna à esquerda para ajustar a estrutura do nó.
        """
        if len(self.children) < 2 or not self.children[0].keys:
            return
        left_child = self.children[0]
        right_child = self.children[1]
        temp = left_child.keys.pop()
        old_key = self.keys[0]
        self.keys[0] = temp
        right_child.keys.insert(0, old_key)
        if not left_child.leaf and left_child.children:
            child_temp = left_child.children.pop()
            right_child.children.insert(0, child_temp)
            child_temp.parent = right_child

    def fix_deficiency_upwards(self):
        """
        Propaga a correção de deficiência para os nós pais enquanto houver nós (exceto a raiz)
        com menos que m chaves.
        """
        current = self
        while current.parent is not None and len(current.keys) < current.m:
            parent = current.parent
            idx = parent.children.index(current)
            parent.fill(idx)
            current = parent

    def print_node(self, level=0):
        """
        Imprime recursivamente este nó e sua subárvore com recuo, mostrando apenas as chaves.
        """
        indent = "    " * level
        print(indent + str([k[0] for k in self.keys]))
        if not self.leaf:
            for child in self.children:
                child.print_node(level+1)


class BTree:
    def __init__(self, ordem):
        """
        Inicializa a Árvore B.
        
        Parâmetro:
            ordem: valor informado (por exemplo, 3). Internamente, m = ordem - 1.
        """
        self.m = ordem - 1  # mínimo de chaves em nós não‑raiz
        self.root = BTreeNode(self.m, leaf=True)

    def search(self, key):
        if self.root:
            return self.root.search(key)
        return None

    def insert(self, key, address):
        """
        Insere o par (chave, endereço) na árvore.
        Se a raiz estiver cheia, é dividida antes da inserção.
        """
        if len(self.root.keys) == 2 * self.m + 1:
            s = BTreeNode(self.m, leaf=False)
            s.children.append(self.root)
            self.root.parent = s
            s.split_child(0)
            self.root = s
        new_root = self.root.insert_nonfull(key, address)
        if new_root is not None:
            self.root = new_root

    def remove(self, key):
        """
        Remove a chave (e seu par associado) da árvore.
        """
        if not self.root:
            print("Árvore vazia.")
            return
        self.root.remove(key)
        if self.root and len(self.root.keys) == 0:
            if not self.root.leaf:
                self.root = self.root.children[0]
                self.root.parent = None
            else:
                self.root = None

    def print_tree(self):
        """
        Imprime a estrutura completa da árvore B.
        """
        if self.root:
            self.root.print_node()
        else:
            print("A Árvore B está vazia.")


# Exemplo de uso e testes:
if __name__ == "__main__":
    # Usando "ordem" 3 (logo, m = 2; mínimo = 2 chaves, máximo = 5 chaves)
    btree = BTree(3)
    
    # Vetor de elementos a serem inseridos
    elementos = [15, 3, 8, 23, 1, 9, 14, 18, 10, 20, 5, 6, 12, 30, 2, 4, 7, 11, 13, 16, 19, 21, 26, 28, 27, 29, 17, 22, 25, 24, 31, 35, 33, 36]
    
    print("=== Inserções ===")
    for chave in elementos:
        print(f"\nInserindo {chave}...")
        btree.insert(chave, f"End_{chave}")
        btree.print_tree()
        print("--------------------------")
    
    # Vetor de chaves para busca
    buscas = [1, 14, 18, 31, 40]  
    print("\n=== Buscas ===")
    for chave in buscas:
        resultado = btree.search(chave)
        if resultado:
            no, idx = resultado
            print(f"\nChave {chave} encontrada com endereço: {no.keys[idx][1]}")
        else:
            print(f"\nChave {chave} não encontrada na árvore.")
    
    # Exemplo de remoções
    print("\n=== Remoções ===")
    for chave in [15, 3, 8, 23, 1, 9, 14, 18, 10, 20, 5, 6, 12, 30, 2, 4, 7, 11, 13, 16, 19, 21, 26, 28, 27, 29, 17, 22, 25, 24, 31, 35, 33, 36]:
        print(f"\nRemovendo chave {chave}...")
        btree.remove(chave)
        btree.print_tree()
        print("--------------------------")
