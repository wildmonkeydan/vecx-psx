#pragma once
#include "File.h"

namespace System {
    class FileSerial : public FileBase
    {
    public:
        int Open(const char* filename);
        int Close();
        int Seek(int seek, SeekAnchor anc);
        unsigned char* Read(int numBytes);
        unsigned char* Read();
        int Write(unsigned char* data, int numBytes);
    private:
        int fd;
    };
}