#include "pch.h"
#include "FixRelocateTable.h"
#include "ImageDirectory.h"

struct RELOCATION
{
	WORD    Offset : 12;
	WORD    Type : 4;
};

void FixRelocate(LPVOID peImage)
{
	IMAGE_DOS_HEADER *dosHeader = PIMAGE_DOS_HEADER(peImage);
	IMAGE_NT_HEADERS *ImageNTHeaders = PIMAGE_NT_HEADERS(DWORD(peImage) + dosHeader->e_lfanew);

	DWORD Size = ImageNTHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC].Size;

	if (Size > 0)
	{
		DWORD imprortDescRVA = ImageNTHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC].VirtualAddress;
		const IMAGE_BASE_RELOCATION* pRelocTable = (PIMAGE_BASE_RELOCATION)((DWORD)peImage + imprortDescRVA);
		if (pRelocTable)
		{
			while (pRelocTable->SizeOfBlock)
			{
				WORD* pLocData = (WORD*)((PBYTE)pRelocTable + sizeof(IMAGE_BASE_RELOCATION));

				int nNumberOfReloc = (pRelocTable->SizeOfBlock - sizeof(IMAGE_BASE_RELOCATION)) / sizeof(RELOCATION);
				if (nNumberOfReloc)
				{
					RELOCATION* pReloc = (RELOCATION*)(pRelocTable + 1);
					CONST UINT_PTR Difference = ((UINT_PTR)peImage - ImageNTHeaders->OptionalHeader.ImageBase);

					for (DWORD dwCount = 0; dwCount < nNumberOfReloc; ++dwCount)
					{
						UINT_PTR* pVal = (UINT_PTR*)((UINT_PTR)peImage + pRelocTable->VirtualAddress + pReloc[dwCount].Offset);

						switch (pReloc[dwCount].Type)
						{
						case IMAGE_REL_BASED_DIR64:
							*pVal += Difference;
							break;
						case IMAGE_REL_BASED_HIGHLOW:
							*pVal += Difference;
							break;
						case IMAGE_REL_BASED_HIGH:
							*pVal += HIWORD(Difference);
							break;
						case IMAGE_REL_BASED_LOW:
							*pVal += LOWORD(Difference);
							break;
						}
					}
				}
				pRelocTable = (CONST IMAGE_BASE_RELOCATION*)(((UINT_PTR)pRelocTable) + pRelocTable->SizeOfBlock);
			}
		}
	}
}