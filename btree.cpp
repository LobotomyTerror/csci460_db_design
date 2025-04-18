#include <iostream>
#include <cstring>

const int MAX_LEN = 20;
struct Node
{
    /* data */
    bool used;
    char first[MAX_LEN];
    int left, right;
};

const int ROOT_NODE = 0;
const int NODENULL = 0;
const int MAX_NODES = 10;

Node Nodes[MAX_NODES];


void nodePrint(Node &n) {
    std::cout << n.first << " l: " << n.left << " r: " << n.right << std::endl;
}


void inorderPrint(int curr = ROOT_NODE) {
    // if (curr == NODENULL) return;
    if (Nodes[curr].left != NODENULL)
        inorderPrint(Nodes[curr].left);
    nodePrint(Nodes[curr]);
    if (Nodes[curr].right != NODENULL)
        inorderPrint(Nodes[curr].right);  
}


void print() {
    for (int i = 0; i < MAX_NODES; ++i) {
        std::cout << "Pos: " << i << " ";
        nodePrint(Nodes[i]);
    }
}


void init() {
    for (int i = 0; i < MAX_NODES; ++i) Nodes[i].used = false;
}

int getNew() {
    for (int i = 0; i < MAX_NODES; ++i) { if (!Nodes[i].used) return i; }
}

void insert(char *Name, int curr = ROOT_NODE) {
    if(!Nodes[curr].used) {
        std::strncpy(Nodes[curr].first, Name, MAX_LEN);
        Nodes[curr].used = true;
        Nodes[curr].left = NODENULL;
        Nodes[curr].right = NODENULL;
    } else {
        if (std::strcmp(Nodes[curr].first, Name) > 0) { // First Name < Name
            if (Nodes[curr].left == NODENULL) Nodes[curr].left = getNew();
            insert(Name, Nodes[curr].left);
        } else if (std::strcmp(Nodes[curr].first, Name) < 0) { // First name > Name
            if (Nodes[curr].right == NODENULL) Nodes[curr].right = getNew();
            insert(Name, Nodes[curr].right);
        }
    }
}


void remove_nodes(char *Name, int curr = ROOT_NODE) {
    if (!Nodes[curr].used) return;

    if (std::strcmp(Nodes[curr].first, Name) > 0) { // First Name < Name
        if (Nodes[curr].left != NODENULL) remove_nodes(Name, Nodes[curr].left);
    } else if (std::strcmp(Nodes[curr].first, Name) < 0) { // First name > Name
        if (Nodes[curr].right != NODENULL) remove_nodes(Name, Nodes[curr].right);
    } else {
        if (Nodes[curr].right == NODENULL) { // Keep nodes left
            int pos = Nodes[curr].left;
            Nodes[curr] = Nodes[pos];
            Nodes[pos].used = false;
        }
        else if (Nodes[curr].left == NODENULL) { // keep the nodes right
            int pos = Nodes[curr].right;
            Nodes[curr] = Nodes[pos];
            Nodes[pos].used = false;
        }
        else { // Need to keep both sub trees
            int leftPos = Nodes[curr].left;
            int pos = Nodes[curr].right;
            Nodes[curr] = Nodes[pos];
            Nodes[pos].used = false;
            // insertLeft(curr, leftPos);
            while (Nodes[curr].left != NODENULL) curr = Nodes[curr].left;
            Nodes[curr].left = leftPos;
        }
        // Nodes[curr].used = false;
    }
}


int main(int argc, char **argv) {
    insert("Daniel");
    insert("Brandy");
    insert("Soren");
    insert("Mark");
    insert("Shelley");
    inorderPrint();
    remove_nodes("Mark");
    inorderPrint();
    return 0;
}
