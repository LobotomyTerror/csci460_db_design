#include <iostream>
#include <fstream>
#include <cstring>
#include <sys/stat.h>

const int FIRST_SIZE = 20;
const long ROOT_NODE = 0;
const long EMPTY_NODE = -1;

struct __attribute__((packed)) IndexDataNode
{
    /* data */
    bool used;
    long left, right;   // Position in the index file of the left and right nodes
    long dataPos;   // Position in the data file
    char first[FIRST_SIZE];
};

struct __attribute__((packed)) LLNode
{
    /* data */
    char padding[35];
    bool used;
    long next;
};

union IndexBlock
{
    /* data */
    IndexDataNode data;
    LLNode ll;
};

void writeIndexBlock(const IndexBlock &ib, std::fstream &out, long pos) {
    out.seekp(pos * sizeof(IndexBlock));
    out.write((const char*)&ib, sizeof(IndexBlock));
    out.flush();
}

void readIndexBlock(IndexBlock &ib, std::fstream &in, long pos) {
    in.seekg(pos * sizeof(IndexBlock));
    in.read((char*)&ib, sizeof(IndexBlock));
}

std::fstream openIndexFile(std::string name) {
    std::fstream in_out;
    in_out.open(name);
    return in_out;
}

void createIndexFile(std::string name) {
    std::fstream create;
    create.open(name, std::ios::out);
}

void closeIndexFile(std::fstream &in_out) {
    in_out.close();
}

void initializeIndex() {
    std::fstream out = openIndexFile("Index.dat");
    IndexBlock ib;
    ib.ll.used = false;
    ib.ll.next = 0;
    writeIndexBlock(ib, out, 0);
    closeIndexFile(out);
}


long getPos(IndexBlock &ib, std::fstream &f) {
    if (!ib.ll.next) {
        f.seekp(0, std::ios::end);
        long pos = f.tellp() / sizeof(IndexBlock);
        return pos;
    }
    // Return next available position in the free list
    return ib.ll.next;
}

void createNewNode(std::fstream &in, long pos) {
    IndexBlock newNode;
    newNode.data.used = false;
    newNode.data.left = EMPTY_NODE;
    newNode.data.right = EMPTY_NODE;
    newNode.data.dataPos = pos;

    writeIndexBlock(newNode, in, pos);
}

void insertNode(const char *name, std::fstream &in, long curr = ROOT_NODE) {
    IndexBlock currNode;

    readIndexBlock(currNode, in, curr);
    if (!currNode.data.used) { // Insert Into Head Node
        currNode.data.used = true;
        currNode.data.left = EMPTY_NODE;
        currNode.data.right = EMPTY_NODE;
        currNode.data.dataPos = curr;
        strncpy(currNode.data.first, name, FIRST_SIZE);

        writeIndexBlock(currNode, in, curr);
    } else {
        if ((std::strcmp(currNode.data.first, name)) < 0) {
            if (currNode.data.right == EMPTY_NODE) {
                long newPos = getPos(currNode, in);
                currNode.data.right = newPos;
                writeIndexBlock(currNode, in, curr);

                createNewNode(in, newPos);
                
                curr = newPos;
            } else
                curr = currNode.data.right;
            insertNode(name, in, curr);
                
        } else {
            if (currNode.data.left == EMPTY_NODE) {
                long newPos = getPos(currNode, in);
                currNode.data.left = newPos;
                writeIndexBlock(currNode, in, curr);

                createNewNode(in, newPos);

                curr = newPos;
            }
            else
                curr = currNode.data.left;
            insertNode(name, in, curr);
        }
    }
}

void unsetNode(IndexBlock &removeNode, std::fstream &in, long pos) {
    removeNode.data.used = false;
    removeNode.data.left = EMPTY_NODE;
    removeNode.data.right = EMPTY_NODE;
    removeNode.data.dataPos = ROOT_NODE;
    strncpy(removeNode.data.first, "", FIRST_SIZE);

    writeIndexBlock(removeNode, in, pos);
}

long findParent(const char *name, long leafPos, std::fstream &file, long curr = ROOT_NODE) {
    IndexBlock head;
    readIndexBlock(head, file, curr);

    if (head.data.left == leafPos || head.data.right == leafPos)
        return head.data.dataPos;

    if (std::strcmp(head.data.first, name) < 0)
        return findParent(name, leafPos, file, head.data.right);
    else if (std::strcmp(head.data.first, name) > 0)
        return findParent(name, leafPos, file, head.data.left);

    return 0;
}

void deleteNode(const char *name, std::fstream &in, long curr = ROOT_NODE) {
    IndexBlock ib;
    readIndexBlock(ib, in, curr);

    if (!ib.data.used) return;

    // Navigate Left or Right to find the location of the node
    if ((std::strcmp(ib.data.first, name)) < 0){
        if (ib.data.right != EMPTY_NODE)
            deleteNode(name, in, ib.data.right);
    } else if ((std::strcmp(ib.data.first, name)) > 0) {
        if (ib.data.left != EMPTY_NODE)
            deleteNode(name, in, ib.data.left);
    } else {
        if (ib.data.right == EMPTY_NODE && ib.data.left == EMPTY_NODE) { // Leaf Node
            long parentPos = findParent(ib.data.first, ib.data.dataPos, in);
            if (!parentPos) {
                std::cerr << "Could not find parent data" << std::endl;
                exit(1);
            }
            IndexBlock parentNode;
            readIndexBlock(parentNode, in, parentPos);

            char lOrR = parentNode.data.left ? 'L' : 'R';
            if (lOrR == 'L') {
                parentNode.ll.next = parentNode.data.left;
                parentNode.data.left = ROOT_NODE;
                unsetNode(ib, in, curr);
                writeIndexBlock(parentNode, in, parentPos);
            } else {
                parentNode.ll.next = parentNode.data.right;
                parentNode.data.right = ROOT_NODE;
                unsetNode(ib, in, curr);
                writeIndexBlock(parentNode, in, parentPos);
            }
        } else if (ib.data.right == EMPTY_NODE) {   // Keeping nodes left
            long pos = ib.data.left;
            long dataPos = ib.data.dataPos;

            IndexBlock leftNode;
            readIndexBlock(leftNode, in, pos);

            ib = leftNode;
            ib.data.dataPos = dataPos;
            ib.ll.used = false;
            ib.ll.next = pos;

            unsetNode(leftNode, in, pos);
            writeIndexBlock(ib, in, curr);
        } else if (ib.data.left == EMPTY_NODE) { // Keeping right nodes
            long pos = ib.data.right;
            long dataPos = ib.data.dataPos;

            IndexBlock rightNode;
            readIndexBlock(rightNode, in, pos);

            ib = rightNode;
            ib.data.dataPos = dataPos;
            ib.ll.used = false;
            ib.ll.next = pos;

            unsetNode(rightNode, in, pos);
            writeIndexBlock(ib, in, curr);
        } else {    // Both subtrees are being kept
            long leftPos = ib.data.left;
            long rightPos = ib.data.right;
            long dataPos = ib.data.dataPos;
            
            // Bring up right sub tree
            IndexBlock rightTree;
            readIndexBlock(rightTree, in, rightPos);

            IndexBlock leftTree;
            readIndexBlock(leftTree, in, leftPos);

            ib = rightTree;
            ib.data.dataPos = dataPos;
            ib.data.left = leftTree.data.dataPos;
            ib.ll.used = false;
            ib.ll.next = rightPos;

            unsetNode(rightTree, in, rightTree.data.dataPos);
            writeIndexBlock(ib, in, curr);
        }
    }
}

void searchNode(const char *name, std::fstream &in, long curr = ROOT_NODE) {
    IndexBlock ib;
    readIndexBlock(ib, in, curr);
    if (curr < 0) {
        std::cerr << "Name: " << name << ", could not be found" << std::endl;
        return;
    } else if ((std::strcmp(ib.data.first, name)) < 0)
        searchNode(name, in, ib.data.right);
    else if ((std::strcmp(ib.data.first, name)) > 0)
        searchNode(name, in, ib.data.left);
    else
        std::cout << ib.data.first << " is at position " << ib.data.dataPos << std::endl;
}

void inorderPrint(std::fstream &file, long curr = ROOT_NODE) {
    IndexBlock ib;
    readIndexBlock(ib, file, curr);

    if (ib.data.left != EMPTY_NODE)
        inorderPrint(file, ib.data.left);
    std::cout << ib.data.first << " Pos: " << ib.data.dataPos << " l: " << ib.data.left << " r: " << ib.data.right << std::endl;
    if (ib.data.right != EMPTY_NODE)
        inorderPrint(file, ib.data.right);
}

int main(int argc, char **argv) {
    struct stat s;
    std::string filename = "Index.dat";

    if (stat("Index.dat", &s) == 0) {
        initializeIndex();
    } 
    else {
        createIndexFile("Index.dat");
        initializeIndex();
    }

    std::fstream in = openIndexFile(filename);

    insertNode("Daniel", in);
    insertNode("Soren", in);
    insertNode("Brandy", in);
    insertNode("Mark", in);
    insertNode("Shelley", in);
    insertNode("Adeline", in);
    insertNode("Tyler", in);

    inorderPrint(in);

    deleteNode("Brandy", in);
    insertNode("Brandy", in);

    searchNode("Tyler", in);

    inorderPrint(in);

    searchNode("Adrian", in);

    closeIndexFile(in);

    return 0;
}
