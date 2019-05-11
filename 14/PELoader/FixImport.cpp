#include "pch.h"
#include "FixImport.h"
#include "ImageDirectory.h"


const char* GetDllName(LPVOID peImage, const IMAGE_IMPORT_DESCRIPTOR* pDescriptor, int sectionOffset)
{
	DWORD offset = pDescriptor->Name + sectionOffset;
	LPCSTR szLibraryName = (LPCSTR)((UINT_PTR)peImage + offset);

	return szLibraryName;
}

void FixImport(LPVOID peOrignImage, LPVOID peImage)
{
	IMAGE_DOS_HEADER *dosHeader = PIMAGE_DOS_HEADER(peOrignImage);
	IMAGE_NT_HEADERS *ImageNTHeaders = PIMAGE_NT_HEADERS(DWORD(peOrignImage) + dosHeader->e_lfanew);

	DWORD imprortDescRVA = ImageNTHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress;
	DWORD importDescRaw = RvaToRaw(imprortDescRVA, ImageNTHeaders);

	int sectionOffset = importDescRaw - imprortDescRVA;

	const IMAGE_IMPORT_DESCRIPTOR* pDescriptor = (PIMAGE_IMPORT_DESCRIPTOR)((DWORD)peOrignImage + importDescRaw);
	while (pDescriptor->Name)
	{
		const char* pszDllName = GetDllName(peOrignImage, pDescriptor, importDescRaw - imprortDescRVA);
		HMODULE hLib = LoadLibraryA(pszDllName);
		printf("%s\n", pszDllName);

		if (hLib)
		{
			PIMAGE_THUNK_DATA pThunk = NULL;
			PIMAGE_THUNK_DATA pAddrThunk = NULL;

			if (pDescriptor->OriginalFirstThunk)
			{
				pThunk = (PIMAGE_THUNK_DATA)((UINT_PTR)peOrignImage + (pDescriptor->OriginalFirstThunk + sectionOffset));
			}
			else
			{
				pThunk = (PIMAGE_THUNK_DATA)((UINT_PTR)peOrignImage + (pDescriptor->FirstThunk + sectionOffset));
			}

			pAddrThunk = (PIMAGE_THUNK_DATA)((UINT_PTR)peImage + pDescriptor->FirstThunk );

			

			while (pAddrThunk && pThunk &&	pThunk->u1.AddressOfData )
			{
				if (IMAGE_SNAP_BY_ORDINAL(pThunk->u1.Ordinal))
				{
					LPCSTR Ordinal = (LPCSTR)IMAGE_ORDINAL(pAddrThunk->u1.Ordinal);
					printf("%s\n", Ordinal);
#if defined(_WIN64)
					pAddrThunk->u1.Function = (ULONGLONG)GetProcAddress(hLib, Ordinal);
#else
					pAddrThunk->u1.Function = (DWORD)GetProcAddress(hLib, Ordinal);
#endif
				}
				else
				{
					PIMAGE_IMPORT_BY_NAME pImport = (PIMAGE_IMPORT_BY_NAME)((UINT_PTR)peOrignImage + (pThunk->u1.AddressOfData + sectionOffset));

#if defined(_WIN64)
					pAddrThunk->u1.Function = (ULONGLONG)GetProcAddress(hLib, pImport->Name);
#else
					pAddrThunk->u1.Function = (DWORD)GetProcAddress(hLib, pImport->Name);
#endif
					printf("\t%s\n", pImport->Name);
				}

				++pThunk;
				++pAddrThunk;
			}
		}


		++pDescriptor;
	}
}