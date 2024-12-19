#include "FileCD.h"
#include "Utility/DebugUtil.h"
#include "Utility/GlobalDefs.h"

namespace System {
	int FileCD::Open(const char* filename)
	{
		if (!CdSearchFile(&file, filename)) {
			printf("Not found!\n");
			file.size = -1;
		}
		else {
			seekPos = file.pos;
			CdControl(CdlSetloc, (unsigned char*)&file.pos, 0);
		}

		return CdIsoError();
	}
	int FileCD::Close()
	{
		file = { 0 };
		return 0;
	}
	int FileCD::Seek(int seek, SeekAnchor anc)
	{
		CdlLOC pos = { 0 };

		switch (anc)
		{
		case System::FileBase::SEEK_SET:
			CdIntToPos(CdPosToInt(&file.pos) + (seek >> 11), &pos);
			break;
		case System::FileBase::SEEK_CUR:
			CdIntToPos(CdPosToInt(&seekPos) + (seek >> 11), &pos);
			break;
		case System::FileBase::SEEK_END:
			CdIntToPos((CdPosToInt(&file.pos) + file.size) + (seek >> 11), &pos);
			break;
		default:
			return -1;
		}

		CdControl(CdlSetloc, (unsigned char*)&pos, 0);
		seekPos = pos;

		return CdIsoError();
	}
	unsigned char* FileCD::Read(int numBytes)
	{
		if (numBytes % CD_SECTOR_SIZE) {
			PRINTF("!ERROR! -- Tried to read an unaligned amount of bytes\n");
			return nullptr;
		}

		unsigned char* buffer = nullptr;
		buffer = (unsigned char*)malloc(2048 * ((numBytes + 2047) / 2048));

		// Read sectors
		CdRead((int)BtoS(numBytes), (uint32_t*)buffer, CdlModeSpeed);
		// Wait until read has completed
		CdReadSync(0, 0);

		return buffer;
	}
	unsigned char* FileCD::Read()
	{
		//CdlFILE	file;
		unsigned char* buffer = nullptr;

		// Allocate a buffer for the file
		buffer = (unsigned char*)malloc(2048 * ((file.size + 2047) / 2048));
		PRINTF("\nReading to %p\n", buffer);

		// Set seek target (seek actually happens on CdRead())
		CdControl(CdlSetloc, (unsigned char*)&file.pos, 0);

		// Read sectors
		CdRead((int)BtoS(file.size), (uint32_t*)buffer, CdlModeSpeed);

		// Wait until read has completed
		CdReadSync(0, 0);

		PRINTF("Done.\n");

		return buffer;
	}
	int FileCD::Write(unsigned char* data, int numBytes)
	{
		PRINTF("!ERROR! -- Tried to write to a CD file");
		return 0;
	}
}