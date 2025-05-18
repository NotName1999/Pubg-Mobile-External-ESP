namespace Offset {
	DWORD PointerUe4 = 0xE0C3560;  // never change
	DWORD Gword = 0xa41e6d4; // find  //00 9C DA 6C 98 00 00
	DWORD Gnames = 0x9FF1D74;// find //FF 7F 7F [60 EF 5F 04 04 00 00 00 04 00 00 00 00 00 80 3F 00 00 80 BF 00 00 80 3F 00 00 80 BF 00 00 80 3F 00 00 80 3F 00 00 80 BF 00 00 80 BF 00 00 80 BF 00 00 80 3F 00 00 80 3F 00 00 80 BF 05 00 05 05]
	DWORD VIEWMATRIX = 0xA3FD8F0;// find  //44 78 81 5F [40 E4 BB 9D 02 00 00 00 04 00 00 00 03 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 02 00 00 00 80 00 00 00 FF FF FF FF 00 00 00 00 01 00 00 00 00 00 00 00 01 00 00 00 00 01 00 00]
	DWORD PersistentLevel = 0x20; // never change
	DWORD EntityList = 0x70; // never change
	DWORD EntityCount = EntityList + 4; // never change
	DWORD ActorsClass = 0x384; // never change
	DWORD NetDriver = 0x24; // never change
	DWORD isDead = 0xa60; //bool bDead;
	DWORD RootComponent = 0x158; // neverchange
	DWORD Position = 0x160; // neverchange


}