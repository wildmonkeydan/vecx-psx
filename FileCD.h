#pragma once
#include "File.h"
#include <psxcd.h>

namespace System {
    class FileCD : public FileBase
    {
    public:
        int Open(const char* filename);
        int Close();
        int Seek(int seek, SeekAnchor anc);
        unsigned char* Read(int numBytes);
        unsigned char* Read();
        int Write(unsigned char* data, int numBytes);
    private:
        CdlFILE file;
        CdlLOC seekPos;
    };
}