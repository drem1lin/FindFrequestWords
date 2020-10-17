#include "Utils.h"

namespace CharUtils
{
	char const* const Skip(char const* const str, const size_t strLength, size_t& chSkipped, skipCryteria func) noexcept
	{
		char const* newPos = str;
		size_t skipped = 0;
		while ((skipped < strLength) && func(newPos[skipped]))
		{
			skipped++;
		}

		if (skipped == strLength)
			newPos = nullptr; //end of line reached

		else
		{
			newPos = str + skipped;
			chSkipped = skipped;
		}
		return newPos;
	}

	char const* const SkipOtherSymbols(char const* const str, const size_t strLength, size_t& chSkipped) noexcept
	{
		return Skip(str, strLength, chSkipped, IsOtherSymbol);
	}

	char const* const SkipPlaceholders(char const* const str, const size_t strLength, size_t& chSkipped) noexcept
	{
		return Skip(str, strLength, chSkipped, IsPlaceholder);
	}

	char const* const SkiplineEnds(char const* const str, const size_t strLength, size_t& chSkipped) noexcept
	{
		return Skip(str, strLength, chSkipped, IsEndOfLine);
	}

	bool IsCharacter(const char c) noexcept
	{
		return (c >= 'a' && c <= 'z')
			|| (c >= 'A' && c <= 'Z');
	}

	bool IsCorrectSymbol(const char c) noexcept
	{
		return IsCharacter(c)
			|| (c >= '0' && c <= '9')
			|| c == '_';
	}

	bool IsPlaceholder(const char c) noexcept
	{
		return (c == ' ')
			|| (c == '\"')
			|| (c == '\'')
			|| (c == '{')
			|| (c == '\t')
			|| (c == '(')
			|| (c == '[');
	}

	bool IsOtherSymbol(const char c) noexcept
	{
		return !IsCharacter(c);
	}

	bool IsEndOfLine(const char c) noexcept
	{
		return c == '\n';
	}
}