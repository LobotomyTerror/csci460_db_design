#include <iostream>
#include <cstring>
#include <fstream>

const int MAXBYTES = 32;
const long FILENULL = 0;

class __attribute__((packed))DataItems {
    public:
    int b;
    double a;
    bool c;

    friend std::ostream & operator <<(std::ostream &out, DataItems &item) {
        out << "a:" << item.a << ", b:" << item.b << ", c:"; 
            if (item.c) out << "T";
            else out << "F";
        return out << std::endl;
    }
};

class __attribute__((packed))Data {
    public:
    union dataList {
        DataItems data;
        long next;
        // long left, right; binary tree concept
    };
    bool used;
    char gap[MAXBYTES - sizeof(DataItems) - 1]; // Assumes size of DataItems is larger than long

    friend std::ostream & operator <<(std::ostream &out, Data &item) {
        if (item.used) 
            out << "Data Node: " << item.dataList.data;
        else
            out << "Linked List Node: " << item.dataList.next;
        out << std::endl;
    }

    // Will be one greater than the actual nuber of records
    static long getSize(std::istream &in) {
        in.seekg(0, std::ios_base::end);
        return in.tellg() / sizeof(Data) - 1;
    }
    void write(std::ostream &out, long pos, bool setUsed = true) {
        out.seekp(pos*sizeof(Data));
        if (setUsed) used = true;
        out.write(reinterpret_cast<char *>(this), sizeof(Data));
        out.flush();
    }
    void read(std::istream &in, long pos) {
        in.seekg(pos*sizeof(Data));
        in.read(reinterpret_cast<char *>(this), sizeof(Data));
    }

    static void createDB(std::string fname) {
        Data d;
        d.used = false;
        d.dataList.next = FILENULL;
        std::ofstream fout;
        fout.open(fname);
        d.write(fout, FILENULL, false);
        fout.close();
    }

    static long dataNew(std::istream &in) {
        long n = getSize(in);
        for (int i = 0; i < n; ++i) {
            Data d;
            d.read(in, i);
            if (d.used == false) return i;
        }
        return n;
    }
    static void dataDelete(std::ostream &out, long pos) {
        Data d;
        d.used = false;
        d.write(out, pos, false);
    }
};

class __attribute__((packed)) Database {
    public:
    std::fstream file;
    static void createDB(std::string fname) {
        Data::createDB(fname);
    }
    // Constructor should open the database file and keep the file handle for us
    Database(std::string fname) {
        file.open(fname);
    }
    Data write(long pos) {
        Data d;
        d.write(file, pos, true);
        return d;
    }
    void read(Data d, long pos) {
        d.read(file, pos);
    }
    ~Database() {
        file.flush();
        file.close();
    }
};

int main() {
    Data::dataList d;
    Data e;
    d.data.a = -1.0;
    d.data.b = -1;
    d.data.c = true;
    std::ofstream fout;
    std::ifstream fin;
    fout.open("Data.bin");
    d.write(fout, 0);
    e.read(fin, 0);
    d.data.a += 2;
    d.data.b += 2;
    d.data.c = true;
    std::cout << Data::getSize(fin) << std::endl;
    d.write(fout, 4);
    std::cout << Data::getSize(fin) << std::endl;
    Data::dataDelete(fout, 0);
    long pos = Data::dataNew(fin);
    std::cout << "Pos: " << pos << std::endl;

}