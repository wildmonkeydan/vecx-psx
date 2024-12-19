#include "FileSerial.h"
#include <psxsn.h>
#include <Utility/DebugUtil.h>
#include <Utility/GlobalDefs.h>

namespace System {
	int FileSerial::Open(const char* filename)
	{	
		filename++;
		fd = PCopen(filename, PCDRV_MODE_READ_WRITE);
		if (fd == -1) {
			fd = PCcreat(filename);
			if (fd == -1) {
				PRINTF("!ERROR! -- File not found %s\n", filename);
			}
		}
		else {
			PRINTF("Opened file %s\n", filename);
		}
		return fd;
	}

	int FileSerial::Close()
	{
		return PCclose(fd);
	}

	int FileSerial::Seek(int seek, SeekAnchor anc)
	{
		return PClseek(fd, seek, (PCDRV_SeekMode)anc);
	}

	unsigned char* FileSerial::Read(int numBytes)
	{
		if (numBytes % CD_SECTOR_SIZE) {
			PRINTF("!ERROR! -- Tried to read an unaligned amount of bytes\n");
			return nullptr;
		}

		unsigned char* buffer = nullptr;
		buffer = (unsigned char*)malloc(numBytes);

		if (PCread(fd, buffer, numBytes) != -1) {
			return buffer;
		}
		else {
			free(buffer);
			PRINTF("!ERROR! -- Serial read failed!\n");
			return nullptr;
		}
	}

	unsigned char* FileSerial::Read()
	{
		int size = PClseek(fd, 0, PCDRV_SEEK_END);
		PClseek(fd, 0, PCDRV_SEEK_SET);

		unsigned char* buffer = nullptr;
		buffer = (unsigned char*)malloc(size);

		if (PCread(fd, buffer, size) != -1) {
			return buffer;
		}
		else {
			free(buffer);
			PRINTF("!ERROR! -- Serial read failed!\n");
			return nullptr;
		}
	}

	int FileSerial::Write(unsigned char* data, int numBytes)
	{
		return PCwrite(fd, data, numBytes);
	}
}