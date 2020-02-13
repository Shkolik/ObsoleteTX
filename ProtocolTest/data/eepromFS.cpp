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

#include "eepromFS.h"

uint8_t   s_write_err = 0;    // error reasons
RlcFile   theFile;  //used for any file operation
EeFs      eeFs;

uint8_t  s_sync_write = false;

uint8_t * eeprom_buffer_data;
volatile uint8_t eeprom_buffer_size = 0;

uint16_t eeprom_pointer;

inline void eeprom_write_byte()
{
	EEAR = eeprom_pointer;
	EECR |= 1<<EERE;
	if(*eeprom_buffer_data == EEDR) return;

	EEDR = *eeprom_buffer_data;

	uint8_t flags=SREG;
	cli();
	
	EECR |= 1<<EEMWE;
	EECR |= 1<<EEWE;

	SREG = flags;

	eeprom_pointer++;
	eeprom_buffer_data++;
}

ISR(EE_READY_vect)
{
	if (--eeprom_buffer_size)
    {
		eeprom_write_byte();
    }
	else
    {
		debugln("ISR EE_READY_vect Cleared : Eeprom is saved");
		EECR &= ~(1<<EERIE);
    }
}

void eepromWriteBlock(uint8_t * i_pointer_ram, uint16_t i_pointer_eeprom, size_t size)
{
	debugln("eeprom buffer empty");

	eeprom_pointer = i_pointer_eeprom;
	eeprom_buffer_data = i_pointer_ram;
	eeprom_buffer_size = size+1;
	EECR |= (1<<EERIE);

	if (s_sync_write)
    {
		while (eeprom_buffer_size)
			wdt_reset();
    }
}

static uint8_t EeFsRead(blkid_t blk, uint8_t ofs)
{
	uint8_t ret;
	EEPROMREADBLOCK(&ret, (uint16_t)(blk*BS+ofs+BLOCKS_OFFSET), 1);
	return ret;
}

static blkid_t EeFsGetLink(blkid_t blk)
{
	return EeFsRead(blk, 0);
}

static void EeFsSetLink(blkid_t blk, blkid_t val)
{
	static blkid_t s_link; // we write asynchronously, then nothing on the stack!
	s_link = val;
	eepromWriteBlock((uint8_t *)&s_link, (blk*BS)+BLOCKS_OFFSET, sizeof(blkid_t));
}

static uint8_t EeFsGetDat(blkid_t blk, uint8_t ofs)
{
	return EeFsRead(blk, ofs+sizeof(blkid_t));
}

static void EeFsSetDat(blkid_t blk, uint8_t ofs, uint8_t *buf, uint8_t len)
{
	eepromWriteBlock(buf, (blk*BS)+ofs+sizeof(blkid_t)+BLOCKS_OFFSET, len);
}

static void EeFsFlushFreelist()
{
	eepromWriteBlock((uint8_t *)&eeFs.freeList, offsetof(EeFs, freeList), sizeof(eeFs.freeList));
}

static void EeFsFlushDirEnt(uint8_t i_fileId)
{
	eepromWriteBlock((uint8_t *)&eeFs.files[i_fileId], offsetof(EeFs, files) + sizeof(DirEnt)*i_fileId, sizeof(DirEnt));
}

static void EeFsFlush()
{
	eepromWriteBlock((uint8_t *)&eeFs, 0, sizeof(eeFs));
}

uint16_t EeFsGetFree()
{
	int16_t ret = 0;
	blkid_t i = eeFs.freeList;
	while (i)
	{
		ret += BS-sizeof(blkid_t);
		i = EeFsGetLink(i);
	}
	
	ret += eeFs.files[FILE_TMP].size;
	ret -= eeFs.files[FILE_MODEL(g_general.currentModel)].size;
	
	return (ret > 0 ? ret : 0);
}

/// free one or more blocks
static void EeFsFree(blkid_t blk)
{
	blkid_t i = blk;
	blkid_t tmp;

	while ((tmp=EeFsGetLink(i)))
	{
		i = tmp;
	}

	EeFsSetLink(i, eeFs.freeList);
	eeFs.freeList = blk; //chain in front
	EeFsFlushFreelist();
}

void eepromCheck()
{
	ENABLE_SYNC_WRITE(true);

	uint8_t *bufp = (uint8_t *)&g_model;
	memclear(bufp, BLOCKS);
	blkid_t blk ;

	for (uint8_t i=0; i<=MAXFILES; i++)
	{
		blkid_t *startP = (i==MAXFILES ? &eeFs.freeList : &eeFs.files[i].startBlk);
		blkid_t lastBlk = 0;
		blk = *startP;
		while (blk)
		{
			if (blk < FIRSTBLK || // bad blk index
				blk >= BLOCKS  || // bad blk index chain
				bufp[blk])        // blk double usage
			{
				if (lastBlk)
				{
					EeFsSetLink(lastBlk, 0);
				}
				else
				{
					*startP = 0; // interrupt chain at start pos
					EeFsFlush();
				}
				blk = 0; // abort
			}
			else
			{
				bufp[blk] = i+1;
				lastBlk   = blk;
				blk       = EeFsGetLink(blk);
			}
		}
	}

	for (blk=FIRSTBLK; blk<BLOCKS; blk++)
	{
		if (!bufp[blk])   // unused block
		{
			EeFsSetLink(blk, eeFs.freeList);
			eeFs.freeList = blk; // chain in front
			EeFsFlushFreelist();
		}
	}

	ENABLE_SYNC_WRITE(false);
}

void eepromFormat()
{
	ENABLE_SYNC_WRITE(true);

	uint8_t fil[4] = {0xFF,0xFF,0xFF,0xFF};

	for (uint16_t i=0; i<(EESIZE/DIM(fil)); i+=DIM(fil)) // erase eeprom
	{
		eepromWriteBlock((uint8_t*)&fil, i, DIM(fil));
	}

	memclear(&eeFs, sizeof(eeFs));
	eeFs.version	= EEFS_VERS;
	eeFs.size		= sizeof(eeFs);
	eeFs.freeList	= 0;
	eeFs.bs			= BS;
	for (blkid_t i=FIRSTBLK; i<BLOCKS-1; i++)
	{
		EeFsSetLink(i, i+1);
	}
	EeFsSetLink(BLOCKS-1, 0);
	eeFs.freeList = FIRSTBLK;
	EeFsFlush();

	ENABLE_SYNC_WRITE(false);
}

bool eepromOpen()
{
	EEPROMREADBLOCK((uint8_t *)&eeFs, 0, sizeof(eeFs));

	if (eeFs.version != EEFS_VERS || eeFs.size != sizeof(eeFs))
	{
		return false;
	}

	eepromCheck();
	return true;
}

bool EFile::exists(uint8_t i_fileId)
{
	return eeFs.files[i_fileId].startBlk;
}

/*
 * Swap two files in eeprom
 */
void EFile::swap(uint8_t i_fileId1, uint8_t i_fileId2)
{
	DirEnt            tmp = eeFs.files[i_fileId1];
	eeFs.files[i_fileId1] = eeFs.files[i_fileId2];
	eeFs.files[i_fileId2] = tmp;

	ENABLE_SYNC_WRITE(true);
	EeFsFlushDirEnt(i_fileId1);
	EeFsFlushDirEnt(i_fileId2);
	ENABLE_SYNC_WRITE(false);
}

void EFile::rm(uint8_t i_fileId)
{
	blkid_t i = eeFs.files[i_fileId].startBlk;
	memclear(&eeFs.files[i_fileId], sizeof(eeFs.files[i_fileId]));
	
	ENABLE_SYNC_WRITE(true);
	EeFsFlushDirEnt(i_fileId);
	
	if (i) EeFsFree(i); //chain in
	ENABLE_SYNC_WRITE(false);
}

/*
 * Open file i_fileId for reading.
 * Return the file's type
 */
void EFile::openRd(uint8_t i_fileId)
{
	m_fileId	= i_fileId;
	m_pos		= 0;
	m_currBlk	= eeFs.files[m_fileId].startBlk;
	m_ofs		= 0;
	s_write_err = ERR_NONE;       // error reasons */
}

void RlcFile::openRlc(uint8_t i_fileId)
{
	EFile::openRd(i_fileId);
	m_zeroes   = 0;
	m_bRlc     = 0;
}

uint8_t EFile::read(uint8_t *buf, uint8_t i_len)
{
	uint16_t len = eeFs.files[m_fileId].size - m_pos;
	if (i_len > len)
		i_len = len;

	uint8_t remaining = i_len;
	while (remaining)
	{
		if (!m_currBlk)
		break;

		*buf++ = EeFsGetDat(m_currBlk, m_ofs++);
		if (m_ofs >= BS-sizeof(blkid_t))
		{
			m_ofs = 0;
			m_currBlk = EeFsGetLink(m_currBlk);
		}
		remaining--;
	}

	i_len -= remaining;
	m_pos += i_len;
	return i_len;
}

/*
 * Read runlength (RLE) compressed bytes into buf.
 */
uint16_t RlcFile::readRlc(uint8_t *buf, uint16_t i_len)
{
	uint16_t i = 0;
	for( ; 1; )
	{
		uint8_t ln = min<uint16_t>(m_zeroes, i_len-i);
		memclear(&buf[i], ln);
		i        += ln;
		m_zeroes -= ln;
		if (m_zeroes)
			break;

		ln = min<uint16_t>(m_bRlc, i_len-i);
		uint8_t lr = read(&buf[i], ln);
		i        += lr ;
		m_bRlc   -= lr;
		if(m_bRlc)
			break;

		if (read(&m_bRlc, 1) !=1)
			break; // read how many bytes to read

		if (m_bRlc&0x80)   // if contains high byte
		{
			m_zeroes  =(m_bRlc>>4) & 0x7;
			m_bRlc    = m_bRlc & 0x0f;
		}
		else if(m_bRlc&0x40)
		{
			m_zeroes  = m_bRlc & 0x3f;
			m_bRlc    = 0;
		}
	}
	return i;
}

void RlcFile::write1(uint8_t b)
{
	m_write1_byte = b;
	write(&m_write1_byte, 1);
}

void RlcFile::write(uint8_t *buf, uint8_t i_len)
{
	m_write_len = i_len;
	m_write_buf = buf;

	do
	{
		nextWriteStep();
	}
	while (IS_SYNC_WRITE_ENABLE() && m_write_len && !s_write_err);
}

void RlcFile::nextWriteStep()
{
	if (!m_currBlk && m_pos==0)
	{
		eeFs.files[FILE_TMP].startBlk = m_currBlk = eeFs.freeList;
		if (m_currBlk)
		{
			eeFs.freeList = EeFsGetLink(m_currBlk);
			m_write_step |= WRITE_FIRST_LINK;
			EeFsFlushFreelist();
			return;
		}
	}

	if ((m_write_step & 0x0f) == WRITE_FIRST_LINK)
	{
		m_write_step -= WRITE_FIRST_LINK;
		EeFsSetLink(m_currBlk, 0);
		return;
	}

	while (m_write_len)
	{
		if (!m_currBlk)
		{
			s_write_err = ERR_FULL;
			break;
		}
		if (m_ofs >= (BS-sizeof(blkid_t)))
		{
			m_ofs = 0;
			blkid_t nextBlk = EeFsGetLink(m_currBlk);
			if (!nextBlk)
			{
				if (!eeFs.freeList)
				{
					s_write_err = ERR_FULL;
					break;
				}
				m_write_step += WRITE_NEXT_LINK_1;
				EeFsSetLink(m_currBlk, eeFs.freeList);
				// TODO not good
				return;
			}
			m_currBlk = nextBlk;
		}
		switch (m_write_step & 0x0f)
		{
		case WRITE_NEXT_LINK_1:
			m_currBlk = eeFs.freeList;

			eeFs.freeList = EeFsGetLink(eeFs.freeList);
			m_write_step += 1;
			EeFsFlushFreelist();
			return;
		case WRITE_NEXT_LINK_2:
			m_write_step -= WRITE_NEXT_LINK_2;
			EeFsSetLink(m_currBlk, 0);
			return;
		}
		uint8_t tmp = BS-sizeof(blkid_t)-m_ofs;
		if(tmp>m_write_len)
		tmp = m_write_len;
		m_write_buf += tmp;
		m_write_len -= tmp;
		m_ofs += tmp;
		m_pos += tmp;
		EeFsSetDat(m_currBlk, m_ofs-tmp, m_write_buf-tmp, tmp);
		return;
	}

	if (s_write_err == ERR_FULL)
	{
		//TODO: GUI
		//POPUP_WARNING(STR_EEPROMOVERFLOW);
		m_write_step = 0;
		m_write_len = 0;
		m_cur_rlc_len = 0;
	}
	else if (!IS_SYNC_WRITE_ENABLE())
	{
		nextRlcWriteStep();
	}
}

void RlcFile::create(uint8_t i_fileId, uint8_t type, uint8_t sync_write)
{
	// all write operations will be executed on FILE_TMP
	openRlc(FILE_TMP); // internal use
	eeFs.files[FILE_TMP].type     = type;
	eeFs.files[FILE_TMP].size     = 0;
	m_fileId = i_fileId;
	ENABLE_SYNC_WRITE(sync_write);
}

/*
 * Copy file src to dst
 */
bool RlcFile::copy(uint8_t i_fileDst, uint8_t i_fileSrc)
{
	EFile theFile2;
	theFile2.openRd(i_fileSrc);

	create(i_fileDst, FILE_TYPE_MODEL/*optimization, only model files are copied. should be eeFs.files[i_fileSrc].type*/, true);

	uint8_t buf[BS-sizeof(blkid_t)];
	uint8_t len;
	while ((len=theFile2.read(buf, sizeof(buf))))
	{
		write(buf, len);
		if (write_errno() != 0)
		{
			ENABLE_SYNC_WRITE(false);
			return false;
		}
	}

	blkid_t fri=0;
	if (m_currBlk && (fri=EeFsGetLink(m_currBlk)))
	EeFsSetLink(m_currBlk, 0);

	if (fri)
	EeFsFree(fri);  //chain in

	eeFs.files[FILE_TMP].size = m_pos;
	EFile::swap(m_fileId, FILE_TMP);

	debugln("error copying " + !m_write_step);

	// s_sync_write is set to false in swap();
	return true;
}

void RlcFile::writeRlc(uint8_t i_fileId, uint8_t type, uint8_t *buf, uint16_t i_len, uint8_t sync_write)
{
	create(i_fileId, type, sync_write);

	m_write_step = WRITE_START_STEP;
	m_rlc_buf = buf;
	m_rlc_len = i_len;
	m_cur_rlc_len = 0;
#if defined (EEPROM_PROGRESS_BAR)
	m_ratio = (type == FILE_TYPE_MODEL ? 100 : 10);
#endif

	do
	{
		nextRlcWriteStep();
	}
	while (IS_SYNC_WRITE_ENABLE() && m_write_step && !s_write_err);
}

void RlcFile::nextRlcWriteStep()
{
	uint8_t cnt    = 1;
	uint8_t cnt0   = 0;
	uint16_t i = 0;

	if (m_cur_rlc_len)
	{
		uint8_t tmp1 = m_cur_rlc_len;
		uint8_t *tmp2 = m_rlc_buf;
		m_rlc_buf += m_cur_rlc_len;
		m_cur_rlc_len = 0;
		write(tmp2, tmp1);
		return;
	}

	bool run0 = (m_rlc_buf[0] == 0);

	if (m_rlc_len==0)
		goto close;

	for (i=1; 1; i++)   // !! laeuft ein byte zu weit !!
	{
		bool cur0 = m_rlc_buf[i] == 0;
		if (cur0 != run0 || cnt==0x3f || (cnt0 && cnt==0x0f) || i==m_rlc_len)
		{
			if (run0)
			{
				debugln("error cnt0==0 " + cnt0==0);
				if (cnt<8 && i != m_rlc_len)
				cnt0 = cnt; //aufbew fuer spaeter
				else
				{
					m_rlc_buf+=cnt;
					m_rlc_len-=cnt;
					write1(cnt|0x40);
					return;
				}
			}
			else
			{
				m_rlc_buf+=cnt0;
				m_rlc_len-=cnt0+cnt;
				m_cur_rlc_len=cnt;
				if(cnt0)
				{
					write1(0x80 | (cnt0<<4) | cnt);
				}
				else
				{
					write1(cnt);
				}
				return;
			}
			cnt=0;
			if (i==m_rlc_len)
			break;
			run0 = cur0;
		}
		cnt++;
	}

	close:

	switch(m_write_step)
	{
		case WRITE_START_STEP:
		{
			blkid_t fri = 0;

			if (m_currBlk && (fri = EeFsGetLink(m_currBlk)))
			{
				// TODO reuse EeFsFree!!!
				blkid_t prev_freeList = eeFs.freeList;
				eeFs.freeList = fri;

				while (EeFsGetLink(fri))
				{
					fri = EeFsGetLink(fri);
				}
				m_write_step = WRITE_FREE_UNUSED_BLOCKS_STEP1;
				EeFsSetLink(fri, prev_freeList);
				return;
			}
		}

		case WRITE_FINAL_DIRENT_STEP:
		{
			m_currBlk = eeFs.files[FILE_TMP].startBlk;
			DirEnt & f = eeFs.files[m_fileId];
			eeFs.files[FILE_TMP].startBlk = f.startBlk;
			eeFs.files[FILE_TMP].size = f.size;
			f.startBlk = m_currBlk;
			f.size = m_pos;
			f.type = eeFs.files[FILE_TMP].type;
			m_write_step = WRITE_TMP_DIRENT_STEP;
			EeFsFlushDirEnt(m_fileId);
			return;
		}

		case WRITE_TMP_DIRENT_STEP:
			m_write_step = 0;
			EeFsFlushDirEnt(FILE_TMP);
			return;

		case WRITE_FREE_UNUSED_BLOCKS_STEP1:
			m_write_step = WRITE_FREE_UNUSED_BLOCKS_STEP2;
			EeFsSetLink(m_currBlk, 0);
			return;

		case WRITE_FREE_UNUSED_BLOCKS_STEP2:
			m_write_step = WRITE_FINAL_DIRENT_STEP;
			EeFsFlushFreelist();
			return;
    }
}

void RlcFile::flush()
{
	while (eeprom_buffer_size)
		wdt_reset();

	ENABLE_SYNC_WRITE(true);

	while (m_write_len && !s_write_err)
		nextWriteStep();

	while (isWriting() && !s_write_err)
		nextRlcWriteStep();

	ENABLE_SYNC_WRITE(false);
}

#if defined (EEPROM_PROGRESS_BAR)
void RlcFile::DisplayProgressBar(uint8_t x)
{
	if (s_eeDirtyMsk || isWriting() || eeprom_buffer_size)
	{
		uint8_t len = s_eeDirtyMsk ? 1 : limit<uint8_t>(1, (7 - (m_rlc_len/m_ratio)), 7);
		//TODO: make a macro
		//lcdDrawFilledRect(x+1, 0, 5, FH, SOLID, ERASE);
		//lcdDrawFilledRect(x+2, 7-len, 3, len);
	}
}
#endif

bool eeLoadGeneral()
{
	theFile.openRlc(FILE_GENERAL);
	if (theFile.readRlc((uint8_t*)&g_general, 1) == 1 && g_general.version == EEPROM_VER)
	{
		theFile.openRlc(FILE_GENERAL);
		if (theFile.readRlc((uint8_t*)&g_general, sizeof(g_general)) <= sizeof(GeneralSettings))
		{
			return true;
		}
	}

	debugln("Bad EEPROM version " + g_general.version);
	return false;
}

void eeLoadModelName(uint8_t id, char *name)
{
	memclear(name, sizeof(g_model.name));
	if (id < MAX_MODELS)
	{
		theFile.openRlc(FILE_MODEL(id));
		theFile.readRlc((uint8_t*)name, sizeof(g_model.name));
	}
}

bool eeModelExists(uint8_t id)
{
	return EFile::exists(FILE_MODEL(id));
}

void eeLoadModel(uint8_t id)
{
	if (id<MAX_MODELS)
	{
		if (pulsesStarted())
		{
			sendStopPulses();
		}

		theFile.openRlc(FILE_MODEL(id));
		uint16_t sz = theFile.readRlc((uint8_t*)&g_model, sizeof(g_model));

		bool newModel = false;
      
		/* TODO: Model initialization 
		if (sz < 256)
		{
			modelDefault(id);
			eeCheck(true);
			newModel = true;
		}

		flightReset();
		logicalSwitchesReset();
		setThrSource();

		if (pulsesStarted())
		{
	#if defined(GUI)
			if (!newModel)
			{
				checkAll();
			}
	#endif
			startPulses(PROTOCMD_INIT);
		}

		modelFunctionsContext.reset();
		restoreTimers();

		// TODO pulses should be started after mixer calculations ...
		*/
	}
}

void eeErase(bool warn)
{
	generalDefault();
	/* TODO: Separate GUI and logic
	if (warn)
	{
		ALERT(STR_EEPROMWARN, STR_BADEEPROMDATA, AU_BAD_EEPROM);
	}

	MESSAGE(STR_EEPROMWARN, STR_EEPROMFORMATTING, NULL, AU_EEPROM_FORMATTING);
	*/
	eepromFormat();
	theFile.writeRlc(FILE_GENERAL, FILE_TYPE_GENERAL, (uint8_t*)&g_general, sizeof(GeneralSettings), true);
	modelDefault(0);
	theFile.writeRlc(FILE_MODEL(0), FILE_TYPE_MODEL, (uint8_t*)&g_model, sizeof(g_model), true);
}

void eeCheck(bool immediately)
{
	if (immediately)
	{
		eeFlush();
	}

	if (s_eeDirtyMsk & EE_GENERAL)
	{
		debugln("eeprom write general");
		s_eeDirtyMsk -= EE_GENERAL;
		theFile.writeRlc(FILE_GENERAL, FILE_TYPE_GENERAL, (uint8_t*)&g_general, sizeof(GeneralSettings), immediately);
		if (!immediately)
			return;
	}

	if (s_eeDirtyMsk & EE_MODEL)
	{
		debugln("eeprom write model");
		s_eeDirtyMsk = 0;
		theFile.writeRlc(FILE_MODEL(g_general.currentModel), FILE_TYPE_MODEL, (uint8_t*)&g_model, sizeof(g_model), immediately);
	}
}