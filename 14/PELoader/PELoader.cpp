// PELoader.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include "FixImport.h"
#include "FixRelocateTable.h"


const std::string sPeFile = "F:\\courses\\reverse\\course\\3\\crackme.exe";

void SetImageBase(LPVOID peImage)
{
	PIMAGE_DOS_HEADER pDosHeader = (PIMAGE_DOS_HEADER)peImage;
	PIMAGE_NT_HEADERS pNtHeaders = (PIMAGE_NT_HEADERS)((ULONG32)peImage + pDosHeader->e_lfanew);
	pNtHeaders->OptionalHeader.ImageBase = (ULONG32)peImage;
}



LPVOID ReadFile(const std::string& filePath)
{
	HANDLE hFile = CreateFileA(filePath.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);

	if (hFile == INVALID_HANDLE_VALUE)
	{
		std::cout << "Error open file: " << filePath << " error: " << GetLastError();
		return NULL;
	}

	CHandle handle(hFile);
	DWORD dwFileSize = GetFileSize(hFile, NULL);
	if (dwFileSize)
	{
		LPVOID pvData = HeapAlloc(GetProcessHeap(), 0, dwFileSize);

		if (pvData)
		{
			DWORD dwRead = 0;
			if (ReadFile(hFile, pvData, dwFileSize, &dwRead, NULL) && dwRead == dwFileSize)
			{
				return pvData;
			}
			std::cout << L"Error read file";
			HeapFree(GetProcessHeap(), 0, pvData);
		}

	}

	return NULL;
}


int main()
{
	LPVOID pFile = ReadFile(sPeFile);
	if (!pFile)
		return 1;

	printf("selfModule: 0x%08x\n", GetModuleHandle(NULL));


	IMAGE_DOS_HEADER *dosHeader = PIMAGE_DOS_HEADER(pFile);
	IMAGE_NT_HEADERS *ImageNTHeaders = PIMAGE_NT_HEADERS(DWORD(pFile) + dosHeader->e_lfanew);

	LPVOID peImage = VirtualAlloc(NULL, ImageNTHeaders->OptionalHeader.SizeOfImage,  MEM_COMMIT,PAGE_EXECUTE_READWRITE);

	MoveMemory(peImage, pFile, ImageNTHeaders->OptionalHeader.SizeOfHeaders);

	const IMAGE_SECTION_HEADER* pFirst = IMAGE_FIRST_SECTION(ImageNTHeaders);
	if (!pFirst)
	{
		std::cout << "Error copy sections";
		return 1;
	}


	for (int i = 0; i < ImageNTHeaders->FileHeader.NumberOfSections; i++)
	{
		IMAGE_SECTION_HEADER* SectionHeader = PIMAGE_SECTION_HEADER(DWORD(pFile) + dosHeader->e_lfanew + 248 + (i * 40));
		MoveMemory(LPVOID(DWORD(peImage) + SectionHeader->VirtualAddress), LPVOID(DWORD(pFile) + SectionHeader->PointerToRawData), SectionHeader->SizeOfRawData);
	}

	FixImport(pFile, peImage);
	FixRelocate(peImage);

	DWORD dwOldProtect = 0;
	VirtualProtect(peImage, ImageNTHeaders->OptionalHeader.SizeOfImage, PAGE_EXECUTE_READWRITE, &dwOldProtect);
	SetImageBase(peImage);

	DWORD peEntryPoint = (DWORD)peImage + ImageNTHeaders->OptionalHeader.AddressOfEntryPoint;

	__asm
	{
		mov eax, [peEntryPoint]
		jmp eax
	}
	
	system("pause");
	return 0;
}

