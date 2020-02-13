/*
 * Copyright (C) ObsoleteTX
 *
 * Based on code named
 *   th9x - https://github.com/thus1/th9x
 *   er9x - https://github.com/MikeBland/mbtx
 *   OpenTx - https://github.com/opentx/opentx
 *   OpenAVRc - https://github.com/Ingwie/OpenAVRc_Dev
 *
 * License GPLv2: http://www.gnu.org/licenses/gpl-2.0.html
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */


#ifndef EEPROMFS_H_
#define EEPROMFS_H_

#include "../main.h"

#define blkid_t    uint8_t
#define EESIZE     1024*4
#define EEFS_VERS  5
#define MAXFILES   36
#define BS         16

// File header
struct DirEnt { 
	blkid_t  startBlk;
	uint16_t size:12;
	uint16_t type:4;
} __attribute__((__packed__));

#define EEFS_EXTRA_FIELDS

// File System
struct EeFs {
	uint8_t  version;
	blkid_t  size;
	blkid_t  freeList;
	uint8_t  bs;
	EEFS_EXTRA_FIELDS
	DirEnt files[MAXFILES];
} __attribute__((__packed__));

// eeprom file record
class EFile
{
	public:

	///remove contents of given file
	static void rm(uint8_t i_fileId);

	///swap contents of file1 with them of file2
	static void swap(uint8_t i_fileId1, uint8_t i_fileId2);

	///return true if the file with given file id exists
	static bool exists(uint8_t i_fileId);

	///open file for reading, no close necessary
	void openRd(uint8_t i_fileId);

	uint8_t read(uint8_t *buf, uint8_t len);

	uint8_t  m_fileId;    //index of file in directory = filename
	uint16_t m_pos;       //over all file position
	blkid_t  m_currBlk;   //current block.id
	uint8_t  m_ofs;       //offset inside of the current block
};

extern EeFs eeFs;

#define FILE_TYPE_GENERAL 1
#define FILE_TYPE_MODEL   2

#define ERR_NONE 0
#define ERR_FULL 1

/// fileId of general file
#define FILE_GENERAL   0
/// convert model number 0..MAX_MODELS-1  int fileId
#define FILE_MODEL(n) (1+(n))
#define FILE_TMP      (1+MAX_MODELS)

#define RESV          sizeof(EeFs)  //reserved for eeprom header with directory (eeFs)

#define FIRSTBLK      1
#define BLOCKS        (1+(EESIZE-RESV)/BS)
#define BLOCKS_OFFSET (RESV-BS)

#define ENABLE_SYNC_WRITE(val) s_sync_write = val;
#define IS_SYNC_WRITE_ENABLE() (s_sync_write)
#define EEPROMREADBLOCK(a, b, c)   eeprom_read_block(a, (const void *)b, c)

#if defined (EEPROM_PROGRESS_BAR)
#define DISPLAY_PROGRESS_BAR(x) theFile.DisplayProgressBar(x)
#else
#define DISPLAY_PROGRESS_BAR(x)
#endif

extern uint8_t  s_write_err;    // error reasons

// deliver current errno, this is reset in open
FORCEINLINE uint8_t write_errno()
{
	return s_write_err;
}

class RlcFile: public EFile
{
	uint8_t  m_bRlc;      // control byte for run length decoder
	uint8_t  m_zeroes;

	uint8_t m_flags;
	#define WRITE_FIRST_LINK               0x01
	#define WRITE_NEXT_LINK_1              0x02
	#define WRITE_NEXT_LINK_2              0x03
	#define WRITE_START_STEP               0x10
	#define WRITE_FREE_UNUSED_BLOCKS_STEP1 0x20
	#define WRITE_FREE_UNUSED_BLOCKS_STEP2 0x30
	#define WRITE_FINAL_DIRENT_STEP        0x40
	#define WRITE_TMP_DIRENT_STEP          0x50
	uint8_t m_write_step;
	uint16_t m_rlc_len;
	uint8_t * m_rlc_buf;
	uint8_t m_cur_rlc_len;
	uint8_t m_write1_byte;
	uint8_t m_write_len;
	uint8_t * m_write_buf;
	#if defined (EEPROM_PROGRESS_BAR)
	uint8_t m_ratio;
	#endif

	public:

	void openRlc(uint8_t i_fileId);

	void create(uint8_t i_fileId, uint8_t typ, uint8_t sync_write);

	/// copy contents of i_fileSrc to i_fileDst
	bool copy(uint8_t i_fileDst, uint8_t i_fileSrc);

	inline bool isWriting()
	{
		return m_write_step != 0;
	}
	void write(uint8_t *buf, uint8_t i_len);
	void write1(uint8_t b);
	void nextWriteStep();
	void nextRlcWriteStep();
	void writeRlc(uint8_t i_fileId, uint8_t typ, uint8_t *buf, uint16_t i_len, uint8_t sync_write);

	// flush the current write operation if any
	void flush();

	// read from opened file and decode rlc-coded data
	uint16_t readRlc(uint8_t *buf, uint16_t i_len);

	#if defined (EEPROM_PROGRESS_BAR)
	void DisplayProgressBar(uint8_t x);
	#endif
};

extern RlcFile theFile;  //used for any file operation

FORCEINLINE void eeFlush()
{
	theFile.flush();
}

FORCEINLINE bool eepromIsWriting()
{
	return theFile.isWriting();
}

FORCEINLINE void eepromWriteProcess()
{
	theFile.nextWriteStep();
}

#define eeCopyModel(dst, src) theFile.copy(FILE_MODEL(dst), FILE_MODEL(src))
#define eeSwapModels(id1, id2) EFile::swap(FILE_MODEL(id1), FILE_MODEL(id2))
#define eeDeleteModel(idx) EFile::rm(FILE_MODEL(idx))
#define eeFileSize(f)   eeFs.files[f].size
#define eeModelSize(id) eeFileSize(FILE_MODEL(id))

bool eepromOpen();
void eepromFormat();
uint16_t EeFsGetFree();

void eeLoadModelName(uint8_t id, char *name);
bool eeLoadGeneral();

extern uint8_t  s_sync_write;
extern volatile uint8_t eeprom_buffer_size;

#endif /* EEPROMFS_H_ */