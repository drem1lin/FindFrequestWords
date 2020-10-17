#pragma once

namespace CharUtils
{
	using skipCryteria = bool(*)(const char c);
	
	char const* const Skip(char const* const str, const size_t strLength, size_t& chSkipped, skipCryteria func) noexcept;
	char const* const SkipOtherSymbols(char const* const str, const size_t strLength, size_t& chSkipped) noexcept;
	char const* const SkipPlaceholders(char const* const str, const size_t strLength, size_t& chSkipped) noexcept;
	char const* const SkiplineEnds(char const* const str, const size_t strLength, size_t& chSkipped) noexcept;

	bool IsCharacter(const char c) noexcept;
	bool IsCorrectSymbol(const char c) noexcept;
	bool IsPlaceholder(const char c) noexcept;
	bool IsOtherSymbol(const char c) noexcept;
	bool IsEndOfLine(const char c) noexcept;
}