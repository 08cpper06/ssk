#pragma once
#include <vector>
#include <string>
#include <optional>


struct code_point {
	unsigned short line;
	unsigned short col;
};

class lexer {
public:
	enum class token_type : unsigned char {
		unknown,

		sign,
		number,
		semicolon,
		comma,
		arrow,
		repeat,

		comment_begin,
		comment_end,
		comment_line,

		identifier,

		underbar,

		string,

		_const,
		_mut,

		_return,
		_if,
		_else,
		_while,
		_do,
		_class,

		_int,
		_float,
		_bool,
		_str,

		_true,
		_false,

		func,

		eof,
	};
	struct token {
		std::string raw;
		token_type type;
		code_point point;
	};
private:
	struct context {
		code_point point;
		std::string::const_iterator itr;
		std::string::const_iterator end;
	};
	struct keyword_info {
		std::string str;
		lexer::token_type type;
		bool is_keyword;
	};
public:
	static std::optional<token> try_parse_number(context& con) noexcept;
	static std::optional<token> try_parse_sign_and_keyword(context& con) noexcept;
	static std::optional<token> try_parse_identifier(context& con) noexcept;
	static std::optional<token> try_parse_string(context& con) noexcept;
public:
	static std::vector<token> tokenize(const std::string& source) noexcept;
};