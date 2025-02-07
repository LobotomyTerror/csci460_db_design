#include <iostream>
#include <cstring>
#include <fstream>
#include <string>

// using namespace std;

class __attribute__ ((packed)) DataItems{
  public:
  int b;
  double a;
  bool c;
  //char first[20];
  //char last[20];
  //char address[100];
  friend std::ostream & operator <<(std::ostream &out,DataItems &item){
    out << "a:"<< item.a << ", b:"<< item.b << ", c:";
    if (item.c) out << "T";
    else out << "F";
    return out << std::endl;
  }
};

union DataList{
    DataItems data;
    long next;
};

const int MAXBYTES=32;
const long FILENULL=0;
class __attribute__ ((packed)) Data {
  public:
  DataList dataList;
  bool used;
  // Assumes size of DataItems is larger than long
  char gap[MAXBYTES-sizeof(DataItems)-1];

  Data(DataItems &d){
    dataList.data = d;
  }
  Data(){
    used=false;
    dataList.next=FILENULL;
  }
  friend std::ostream & operator <<(std::ostream &out,Data &item){
    if (item.used) 
      out << "Data Node:" << item.dataList.data;
    else 
      out << "Link List Node:" << item.dataList.next;
    return out << std::endl;
  }

  static long getSize(std::istream &in){
    in.seekg(0,std::ios_base::end);
    return in.tellg()/sizeof(Data)-1;
  }

  void write(std::ostream &out,long pos,bool setUsed=true){
    out.seekp(pos*sizeof(Data));
    if(setUsed) used=true;
    out.write(reinterpret_cast<char *>(this),sizeof(Data));
    out.flush();
  }

  void read(std::istream &in,long pos){
    in.seekg(pos*sizeof(Data));
    in.read(reinterpret_cast<char *>(this),sizeof(Data));
  }
 
  static void createDB(std::string fname){
    Data d;
    d.used=false;
    d.dataList.next=FILENULL;
    std::ofstream fout;
    fout.open(fname);
    d.write(fout,FILENULL,false);
    fout.close();
  }
  
  static long dataNew(std::istream &in){
    long n=getSize(in);
    for (long i=0;i<n;i++){
      Data d;
      d.read(in,i);
      if (d.used==false) return i;
    }
    return n;
  }

  static void dataDelete(std::ostream &out,long pos) {
     Data d;
     d.used=false;
     d.write(out,pos,false);
  }
};

class Database{
  public:
  std::fstream file;
  static void createDB(std::string fname){
    Data::createDB(fname);
  }
  long getSize() {
    return Data::getSize(file);
  }
  // Constructor should open the database file and keep the file handle for us
  Database(std::string fname){
    std::ifstream test;
    test.open(fname);
    if (!test.good())
      Database::createDB(fname);
    else 
      test.close();
    file.open(fname);
  }
  void write(long pos,DataItems &d){
    Data(d).write(file,pos,true);
  }
  DataItems read(long pos) {
    DataItems d;
    Data(d).read(file,pos);
    return d;
  }
  void dataDelete(long pos){
    Data::dataDelete(file,pos);
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
  DataItems d,e;
  Database db("Data.bin");
  d.a=-1.0;
  d.b=-1;
  d.c=true;
  db.write(1,d);
  e=db.read(1);
  std::cout << e ;
  d.b+=2;
  d.a+=2;
  d.c=true;
  db.write(4,d);
  std::cout << db.getSize() << std::endl;
  db.dataDelete(1);
  long pos=db.dataNew();
  std::cout << "Pos of new element" << pos << std::endl;
}
