#include <iostream>
#include <cstring>
#include <fstream>

using namespace std;

class __attribute__ ((packed)) DataItems{
  public:
  int b;
  double a;
  bool c;
  // unsigned char first[8];
  //char last[20];
  //char address[100];
  friend ostream & operator <<(ostream &out,DataItems &item){
    out << "a:"<< item.a << ", b:"<< item.b << ", c:";
    if (item.c) out << "T";
    else out << "F";
    return out << endl;
  }
};

union DataList{
    DataItems data;
    long next;
};

const int MAXBYTES=16;
const long FILENULL=0;
class __attribute__ ((packed)) Data {
  public:
  DataList dataList;
  bool used;
  // Assumes size of DataItems is larger than long
  char gap[MAXBYTES-sizeof(DataItems)-1];


  Data(DataItems &d){
    dataList.data=d;
  }
  Data(){
    used = true;
    dataList.next = FILENULL;
  }
  friend ostream & operator <<(ostream &out,Data &item){
    if (item.used) 
      out << "Data Node:" << item.dataList.data;
    else 
      out << "Link List Node:" << item.dataList.next;
    return out << endl;
  }

  static long getSize(istream &in){
    in.seekg(0,ios_base::end);
    return in.tellg()/sizeof(Data)-1;
  }

  void write(fstream &file, long pos, bool setUsed=true){  // Needs to update freelist
    file.seekp(pos*sizeof(Data));
    if(setUsed) used=true;
    file.write(reinterpret_cast<char *>(this), sizeof(Data));
    file.flush();
  }

  void read(istream &in,long pos) {
    in.seekg(pos*sizeof(Data));
    in.read(reinterpret_cast<char *>(this), sizeof(Data));
  }
 
  static void createDB(string fname){
    Data d;
    d.used=true;
    d.dataList.next=FILENULL;
    fstream fout;
    fout.open(fname, ios::out | ios::binary);
    d.write(fout,FILENULL,false);
    fout.close();
  }
  
  static long dataNew(fstream &file){
    // Read in the head node to find free spaces
    Data head;
    head.read(file, 0);

    if (head.dataList.next == FILENULL) {
      long endOfFilePos = getSize(file) + 1;
      
      Data newBlock;
      newBlock.used = false;
      newBlock.dataList.next = FILENULL;

      newBlock.write(file, endOfFilePos, false);

      return endOfFilePos;
    } else {
      Data tail;
      tail.read(file, head.dataList.next);

      long filePos = head.dataList.next;
      head.dataList.next = tail.dataList.next;
      tail.dataList.next = FILENULL;

      head.write(file, 0);
      file.seekp(filePos * sizeof(Data));
      file.write(reinterpret_cast<char *>(&tail), sizeof(Data));

      return filePos;
    }
  }

  // Free space and link head to new free block
  static void dataDelete(fstream &file, long pos) {
    if (pos == 0) { // Checking if head node is being accessed
      std::cout << "Accessing head node is not allowed" << std::endl;
      exit(1);
    }

    if (pos > getSize(file)) {
      std::cout << "Cannot delete, position is out of bounds" << std::endl;
      exit(2);
    }

    file.seekg(0, ios::beg);
    long headPos;
    file.read(reinterpret_cast<char *>(&headPos), sizeof(long));

    if (headPos == FILENULL) {
      Data d, head;
      d.read(file, pos);
      head.read(file, headPos);

      if (!d.used) {
        std::cout << "Position is already free" << std::endl;
        return;
      }

      d.dataList.data.a = 0.0;
      d.dataList.data.b = 0;
      d.dataList.data.c = false;
      d.used = false;

      d.dataList.next = FILENULL;

      file.seekp(pos*sizeof(Data));
      file.write(reinterpret_cast<char *>(&d), sizeof(Data));

      head = d;
      head.dataList.next = pos;
      head.write(file, headPos, false);
    } else {
      Data d, head, tail;

      d.read(file, pos);
      head.read(file, 0);
      tail.read(file, headPos);

      if (!d.used) {
        std::cout << "Position is already free" << std::endl;
        return;
      }

      d.dataList.data.a = 0.0;
      d.dataList.data.b = 0;
      d.dataList.data.c = false;
      d.used = false;

      if (tail.dataList.next == FILENULL) {
        auto tailPos = head.dataList.next;
        d.dataList.next = tailPos;
        head.dataList.next = pos;
        head.write(file, 0, false);
      } else {
        auto tailPos = tail.dataList.next;
        tail.dataList.next = pos;
        d.dataList.next = tailPos;
        tail.write(file, headPos, false);
      }

      file.seekp(pos*sizeof(Data));
      file.write(reinterpret_cast<char *>(&d), sizeof(Data));
    }

    std::cout << "Empty block at position: " << pos << std::endl;
  }
};

class Database{
  public:
  fstream file;
  static void createDB(string fname){
    Data::createDB(fname);
  }
  long getSize() {
    return Data::getSize(file);
  }
  // Constructor should open the database file and keep the file handle for us
  Database(string fname){
    ifstream test;
    test.open(fname);
    if (!test.good())
      Database::createDB(fname);
    else 
      test.close();
    file.open(fname);
  }
  void write(long pos, DataItems &d){
    Data(d).write(file, pos, true);
  }
  DataItems read(long pos) {
    Data d;
    d.read(file, pos);
    return d.dataList.data;
  }
  void dataDelete(long pos){
    Data::dataDelete(file, pos);
  }
  long dataNew() {
    return Data::dataNew(file);
  }
  ~Database(){
    file.flush();
    file.close();
  }
};

int main() {
  DataItems d;
  Database db("Data.bin");

  d.a = -1.0;
  d.b = -1;
  d.c = true;
  db.write(db.dataNew(), d);

  d.b+=2;
  d.a+=2;
  d.c=true;
  db.write(db.dataNew(), d);

  db.dataDelete(1);
  db.dataDelete(2);

  d.a = -1.0;
  d.b = -1;
  d.c = true;
  db.write(db.dataNew(), d);

  d.b+=2;
  d.a+=2;
  d.c=true;
  db.write(db.dataNew(), d);

  d.b = 3;
  d.a = 3.0;
  d.c = true;
  db.write(db.dataNew(), d);
  db.dataDelete(3);
  db.dataDelete(1);
  db.dataDelete(2);
  db.dataDelete(4); // Should produce an out of bounds error based on the build out of my file
}
