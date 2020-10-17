// This is a personal academic project.Dear PVS - Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#pragma once
#include <string>
#include <atlsync.h>
#include <memory>
#include <functional>

class StringReader
{
public:
	StringReader(const std::wstring& fileName) noexcept(false);
	~StringReader() noexcept;
	char* GetNextString(size_t& stringLength) noexcept;

private:
	ATL::CHandle m_hFile;
	ATL::CHandle m_hFileMapping;
	std::unique_ptr<VOID, std::function<void(LPVOID)>> m_lpFileMap;
	LARGE_INTEGER m_liFileSize;
	char*  m_curPointer;
	ATL::CCriticalSection m_cs;
};
