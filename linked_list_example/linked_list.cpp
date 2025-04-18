#include <iostream>

struct dataList {
    int data;
    dataList* next; // Pointer to the next node
};

class LinkedList { 
    private:
    dataList* head;

    public:
    LinkedList() : head(NULL) {}

    void insertAtBeggining(int value) {
        dataList* data = new dataList();
        data->data = value;
        data->next = head;
        head = data;
    }
    
    void insertAtEnd(int value) {
        if (head) {
            dataList* data = new dataList();
            data->data = value;
            data->next = NULL;
            dataList *temp = head;
            while (temp) {
                if (temp->next == NULL) {
                    temp->next = data;
                    break;
                } 
                temp = temp->next;
            }
        } else {
            insertAtBeggining(value);
        }
    }

    void insertAtPosition(int value, int pos) {
        if (pos < 1) {
            std::cout << "Position is to small should be >= 1" << std::endl;
            return;
        }
        if (pos == 1) {
            insertAtBeggining(value);
            return;
        }
        dataList* temp = head;
        for (int i = 1; i < pos - 1; ++i) {
            temp = temp->next;
        }
        if (!temp) {
            std::cout << "Position is out of bounds" << std::endl;
            return;
        }
        dataList* data = new dataList();
        data->data = value;
        data->next = NULL;
        if (!temp->next) {
            temp->next = data;
        } else {
            data->next = temp->next;
            temp->next = data;
        }
    }

    void deleteFromBeggining() { 
        if (!head) {
            std::cout << "Linked list is empty. No node to delete" << std::endl;
            return;
        }
        dataList* temp = head;
        head = head->next;
        delete temp;
    }

    void deleteFromEnd() {
        if (!head) {
            std::cout << "Linked list is empty. No node to delete." << std::endl;
            return;
        }
        if (!head->next) {
            delete head;
            head = NULL;
            return;
        }
        dataList* temp = head;
        while(temp) {
            if (temp->next->next == NULL) {
                delete temp->next;
                temp->next = NULL;
                break;
            }
            temp = temp->next;
        }
    }

    void deleteFromPos(int pos) {
        if (pos < 1) {
            std::cout << "Value needs to be >= 1" << std::endl;
            return;
        }
        if (pos == 1) {
            deleteFromBeggining();
            return;
        }
        dataList* temp = head;
        for (int i = 0; i < pos - 1 && temp; ++i)
            temp = temp->next;
        if (!temp || !temp->next) {
            std::cout << "Position is out of bounds" << std::endl;
            return;
        }
        dataList* nodeToDelete = temp->next;
        temp->next = temp->next->next;
        delete nodeToDelete;
    }
    void print() {
        if (head == NULL) {
            std::cout << "There is no nodes in the list" << std::endl;
            return;
        }
        dataList* temp = head;
        while(temp) {
            if (!temp->next) {
                std::cout << "Data: " << temp->data << std::endl;
                break;
            }
            std::cout << "Data: " << temp->data << std::endl;
            temp = temp->next;
        }
    }
};

int main() {
    LinkedList list;
    list.insertAtEnd(6);
    list.insertAtBeggining(5);
    list.insertAtEnd(7);
    list.insertAtPosition(8, 2);
    list.insertAtPosition(9, 3);
    list.insertAtEnd(15);
    list.deleteFromBeggining();
    list.deleteFromEnd();
    list.deleteFromPos(2);
    list.print();
    
    return 0;
}