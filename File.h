#pragma once

//#define DEVFILE

namespace System {
	class FileBase
	{
	public:
		enum SeekAnchor {
			SEEK_SET,
			SEEK_CUR,
			SEEK_END
		};

		virtual int Open(const char* filename) = 0;
		virtual int Close() = 0;
		virtual int Seek(int seek, SeekAnchor anc) = 0;
		virtual unsigned char* Read(int numBytes) = 0;
		virtual unsigned char* Read() = 0;
		virtual int Write(unsigned char* data, int numBytes) = 0;
	};
}

#include "FileSerial.h"
#include "FileCD.h"

namespace System {

#ifdef DEVFILE
	#define File FileSerial
#else // DEVFILE
	#define File FileCD
#endif

	#define ReadWholeFile(f, name, p) f.Open(name); p = f.Read(); f.Close();
}