// This is an independent project of an individual developer. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com


#include <iostream>
#include <windows.h>
#include "StringReader.h"
#include <vector>
#include <map>
#include <functional>
#include <set>
#include <chrono>
#include "CCsGuard.h"
#include "Utils.h"

using namespace CharUtils;

#define MAX_POPULARITY_WORD_COUNT 20u

typedef struct _THREAD_PARAMETERS
{
	CRITICAL_SECTION* cs;
	StringReader* sr;
	std::map<std::string, unsigned int>* words;
} THREAD_PARAMETERS, *PTHREAD_PARAMETERS;

char const* FindNextPlaceholder(char const* const str, const size_t strLength, size_t& chSkipped) noexcept
{
	size_t i = 0;
	char const* newPos = str;
	while (i < strLength && !CharUtils::IsPlaceholder(newPos[i]))
	{
		i++;
	}
	if (i == strLength)
		newPos = nullptr; //end of line reached
	else
	{
		newPos = str + i;
		chSkipped = i;
	}
	return newPos;
}


char const* GetNextWord(char const*const  buffer, const size_t bufferSize, size_t& wordLength) noexcept
{
	if (buffer == nullptr || bufferSize == 0)
		return nullptr;

	size_t skipped = 0, skippedTmp = 0;
	char const* pWord = buffer;
	while (pWord != nullptr && CharUtils::IsOtherSymbol(pWord[0]))
	{
		pWord = FindNextPlaceholder(pWord, bufferSize - skipped, skippedTmp);
		if (!pWord)
			continue;
		skipped += skippedTmp;
		pWord = CharUtils::SkipPlaceholders(pWord, bufferSize - skipped, skippedTmp);
		if (!pWord)
			continue;
		skipped += skippedTmp;
	}

	if (pWord == nullptr || skipped >= bufferSize)
		return nullptr;

	size_t length = 0;
	while ((length < bufferSize) && CharUtils::IsCorrectSymbol(pWord[length]))
	{
		++length;
	}
	wordLength = length;
	return pWord;
}

bool ProcessString(char* const textString, const size_t textStringSize, CRITICAL_SECTION* const cs, std::map<std::string, unsigned int>* map) noexcept
{
	if (textString == nullptr || textStringSize == 0 || cs == nullptr)
		return false;

	size_t wordLength = 0;
	size_t processedSymbols = 0;
	char const* word =textString;
	do 
	{
		word = GetNextWord(word + wordLength, textStringSize - processedSymbols, wordLength);
		if (word == nullptr)
			break;

		processedSymbols = word - textString + wordLength;
		if (wordLength == 0)
			continue;
		
		const std::string strWord(word, wordLength);
		CCsGuard guard(cs);
		auto found = map->find(strWord);
		if (found != map->end())
			found->second += 1;
		else
			map->insert(map->end(), { strWord, 1 });
	} while (true);

	return true;
}

unsigned __stdcall StringProcessingThread( void* lpParameter)
{
	auto threadParam = static_cast<PTHREAD_PARAMETERS>(lpParameter);
	size_t length = 0;
	char* str = nullptr;
	do
	{
		str = threadParam->sr->GetNextString(length);
		if (str == nullptr || length == 0)
			break;

		ProcessString(str, length, threadParam->cs, threadParam->words);
	} while (true);
	return 0;
}

typedef std::function<bool(std::pair<std::string, int>, std::pair<std::string, int>)> Comparator;
Comparator compFunctor = [](std::pair<std::string, int> elem1, std::pair<std::string, int> elem2)
{
	return elem1.second > elem2.second;
};

void PrintPopularWords(std::map<std::string, unsigned int>& map, const size_t wordsCount) noexcept
{
	const std::set<std::pair<std::string, int>, Comparator> setOfWords(map.begin(), map.end(), compFunctor);

	const size_t toDisplay = setOfWords.size() < wordsCount ? setOfWords.size() : wordsCount;
	size_t i = 0;
	for (auto it = setOfWords.begin(); it!=setOfWords.end() && i < toDisplay; ++it, ++i)
	{
		std::cout << it->first << " : " << it->second << std::endl;
	}
}

int FindMostPopularWords(const std::wstring& filename, const size_t threadCount, const size_t wordsCount) noexcept
{
	std::unique_ptr<StringReader> sr;
	try
	{
		sr = std::make_unique<StringReader>(filename);
	}
	catch(const std::exception& e)
	{
		std::cout << "Exception: " << e.what() << std::endl;
		return 1;
	}

	CRITICAL_SECTION cs;
	InitializeCriticalSection(&cs);
	std::map<std::string, unsigned int> wordsMap;

	THREAD_PARAMETERS threadParam{ &cs, sr.get(), &wordsMap };

	std::vector<HANDLE> threads;
	threads.reserve(threadCount);

	std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();

	for (size_t i = 0; i < threadCount; ++i)
	{
		HANDLE hTmpThread = reinterpret_cast<HANDLE>(_beginthreadex(nullptr,
			0,
			StringProcessingThread,
			&threadParam,
			0,
			nullptr));
		if (hTmpThread == nullptr)
		{
			std::cout << "Thread " << i << " failed to start" << std::endl;
			continue;
		}
		threads.push_back(hTmpThread);
	}

	const DWORD dw = WaitForMultipleObjects(static_cast<DWORD>(threads.size()), threads.data(), TRUE, INFINITE);
	std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
	std::cout << "Time elapsed = " << std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count() << " microseconds" << std::endl;
	
	DeleteCriticalSection (&cs);
	PrintPopularWords(wordsMap, wordsCount);
	return dw;
} 
 
int main(int argc, char** argv)
{
	size_t threadCount = 6;
	/*std::cout << "Welcome! Please insert thread count:" << std::endl;
	std::cin >> threadCount;
	if (threadCount == 0 || threadCount > MAXIMUM_WAIT_OBJECTS)
	{
		std::cout << "Wrong thread count, please use numbers between 1 and " << MAXIMUM_WAIT_OBJECTS << std::endl;
		return 1;
	}*/

	std::wstring filename(L"G:\\big.txt");

	/*std::cout << "Thank you, now please insert filename:" << std::endl;
	std::wcin >> filename;
	if (filename.empty())
	{
		std::cout << "Wrong filename, filename is empty" << std::endl;
		return 2;
	}*/

	if(0!=FindMostPopularWords(filename, threadCount, MAX_POPULARITY_WORD_COUNT))
	{
		std::wcout << "Error occurred during processing file " << filename << std::endl;
		return 3;
	}

    std::cout << "Work finished!\n";
	return 0;
}