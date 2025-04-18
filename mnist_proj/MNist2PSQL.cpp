#include <iostream>
#include <fstream>
#include <string>

struct __attribute__ ((packed)) HeaderIDX1 {
    int32_t magic;
    int32_t count;
};


struct __attribute__ ((packed)) HeaderIDX3 {
    int32_t magic;
    int32_t count;
    int32_t rows;
    int32_t columns;
};


struct __attribute__ ((packed)) Image {
    uint8_t data[28][28];
};


int main (int argc, char **argv) {
    if (argc < 2) {
        std::cerr << "Usage: executable filename" << std::endl;
        return -1;
    }

    std::string idx1Filename = std::string(argv[1]) + "-labels-idx1";
    std::string idx3Filename = std::string(argv[1]) + "-images-idx3";

    std::fstream idx1, idx3;
    idx1.open(idx1Filename);
    idx3.open(idx3Filename);

    HeaderIDX1 hidx1;
    idx1.read((char *)&hidx1, sizeof(HeaderIDX1));

    HeaderIDX3 hidx3;
    idx3.read((char *)&hidx3, sizeof(HeaderIDX3));

    if ((hidx1.magic != 2049 || hidx3.magic != 2051) && hidx1.count == hidx3.count) {
        for (int i = 0; i < hidx1.count; i++) {
            Image img;
            uint8_t label;
            idx1.read((char *)&label, sizeof(uint8_t));
            idx3.read((char *)&img, sizeof(Image));
        }
    } else {
        std::cerr << "Incorrect Files Entered" << std::endl;
        return -1;
    }

    idx1.close();
    idx3.close();
    return 0;
}   
