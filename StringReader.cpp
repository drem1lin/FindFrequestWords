// This is an independent project of an individual developer. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "StringReader.h"
#include <cstring>
#include <windows.h>
#include <exception>
#include "CCsGuard.h"
#include "Utils.h"

StringReader::StringReader(const std::wstring& fileName) noexcept(false)
	: m_hFile(), m_hFileMapping(), m_lpFileMap(nullptr), m_cs(), m_curPointer(nullptr), m_liFileSize({0,0})
{
		const HANDLE hFile = CreateFile(fileName.c_str(),
			GENERIC_READ,
			FILE_SHARE_READ,
			nullptr,
			OPEN_EXISTING,
			FILE_ATTRIBUTE_NORMAL,
			nullptr);
		if (hFile == INVALID_HANDLE_VALUE)
			throw std::exception("CreateFile failed");

		m_hFile.Attach(hFile);

		if (FALSE == GetFileSizeEx(m_hFile, &m_liFileSize))
			throw std::exception("GetFileSizeEx failed");

		if(m_liFileSize.QuadPart==0)
			throw std::exception("File is empty failed");

		const HANDLE hFileMapping = CreateFileMapping(m_hFile,
			nullptr,
			PAGE_READONLY,
			0,
			0,
			nullptr);
		if (hFileMapping == 0)
			throw std::exception("CreateFileMapping failed");
		m_hFileMapping.Attach(hFileMapping);

		auto lpFileMap = MapViewOfFile(m_hFileMapping,
			FILE_MAP_READ,
			0,
			0,
			static_cast<size_t>(m_liFileSize.QuadPart));
		if (lpFileMap == nullptr)
			throw std::exception("CreateFileMapping failed");

		m_lpFileMap = std::unique_ptr<VOID, std::function<void(LPVOID)>>(lpFileMap, [](LPVOID lpMem) {if (lpMem)UnmapViewOfFile(lpMem); });
		m_curPointer = reinterpret_cast<char*>(m_lpFileMap.get());
}

StringReader::~StringReader() noexcept = default;

char* StringReader::GetNextString(size_t& stringLength) noexcept
{
	if (m_curPointer == nullptr)
		return nullptr;

	CCsGuard guard(&m_cs);

	char* const lineStart = m_curPointer;
	char const* lineEnd = std::strchr(lineStart, '\n');
	if (lineEnd != nullptr)
	{
		size_t skipped = 0;
		lineEnd = CharUtils::SkiplineEnds(lineEnd, m_liFileSize.QuadPart + static_cast<char*>(m_lpFileMap.get())- lineEnd, skipped);
	}

	const size_t readed = lineStart - static_cast<char*>(m_lpFileMap.get());
	if (readed == m_liFileSize.QuadPart) //all characters processed
		return nullptr;
	
	//probably end of file
	if (!lineEnd)
	{
		stringLength = static_cast<char*>(m_lpFileMap.get()) + static_cast<size_t>(m_liFileSize.QuadPart) - lineStart;
		m_curPointer = m_curPointer + stringLength;
	}
	else
	{
		stringLength = lineEnd - lineStart;
		m_curPointer = const_cast<char*>(lineEnd);
	}

	return lineStart;
}
