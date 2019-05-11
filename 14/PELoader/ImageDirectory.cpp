#include "pch.h"
#include "ImageDirectory.h"


uint32_t alignDown(uint32_t value, uint32_t align)
{
	return value & ~(align - 1);
}

uint32_t alignUp(uint32_t value, uint32_t align)
{
	return ((value & (align - 1)) ? alignDown(value, align) + align : value);
}

const IMAGE_SECTION_HEADER* defSection(DWORD rva, IMAGE_NT_HEADERS *ImageNTHeader)
{
	assert(ImageNTHeader);
	const IMAGE_SECTION_HEADER* pFirst = IMAGE_FIRST_SECTION(ImageNTHeader);

	if (pFirst)
	{
		for (DWORD dwCurrent = 0; dwCurrent < ImageNTHeader->FileHeader.NumberOfSections; ++dwCurrent)
		{
			CONST IMAGE_SECTION_HEADER* pCurrent = &pFirst[dwCurrent];
			DWORD start = pCurrent->VirtualAddress;
			DWORD end = start + pCurrent->Misc.VirtualSize; // start + alignUp(pCurrent->Misc.VirtualSize, ImageNTHeader->OptionalHeader.SectionAlignment);


			if (rva >= start && rva < end)
			{
				return pCurrent;
			}
		}
	}

	return NULL;
}

DWORD RvaToRaw(DWORD rva, IMAGE_NT_HEADERS *ImageNTHeader)
{
	assert(ImageNTHeader);

	const IMAGE_SECTION_HEADER* pSection = defSection(rva, ImageNTHeader);
	if (pSection == NULL)
		return -1;

	int delta = pSection->VirtualAddress - pSection->PointerToRawData;
	return  rva - delta;
}