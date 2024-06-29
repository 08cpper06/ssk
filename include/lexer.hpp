#pragma once
#include <vector>
#include <string>
#include <optional>


class lexer {
public:
	enum class token_type : unsigned char {
		sign,
		number,
		semicolon,

		_return,

		eof,
	};
	struct token {
		std::string raw;
		token_type type;
	};
public:
	static std::optional<token> try_parse_number(std::string::const_iterator& itr) noexcept;
	static std::optional<token> try_parse_sign(std::string::const_iterator& itr) noexcept;
	static std::optional<token> try_parse_keyword(std::string::const_iterator& itr, const std::string::const_iterator& end) noexcept;
public:
	static std::vector<token> tokenize(const std::string& source) noexcept;
};