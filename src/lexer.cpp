#include "lexer.hpp"
#include <cassert>
#include <iostream>


std::optional<lexer::token> lexer::try_parse_number(lexer::context& con) noexcept {
	std::string tok;
	bool has_point = false;
	while (*con.itr) {
		if (*con.itr >= '0' && *con.itr <= '9') {
			tok += *con.itr;
		}
		else if (!has_point && *con.itr == '.') {
			tok += *con.itr;
			has_point = true;
		}
		else {
			break;
		}
		++con.point.col;
		++con.itr;
	}
	return tok.empty() ? std::nullopt : std::optional<token>(token{ .raw = std::move(tok), .type = token_type::number, .point = con.point });
}

std::optional<lexer::token> lexer::try_parse_sign_and_keyword(lexer::context& con) noexcept {
	static keyword_info keywords[] = {
		{ .str = ";", .type = lexer::token_type::semicolon, .is_keyword = false },
		{ .str = "+", .type = lexer::token_type::sign, .is_keyword = false },
		{ .str = "-", .type = lexer::token_type::sign, .is_keyword = false },
		{ .str = "*", .type = lexer::token_type::sign, .is_keyword = false },
		{ .str = "/", .type = lexer::token_type::sign, .is_keyword = false },
		{ .str = "=", .type = lexer::token_type::sign, .is_keyword = false },
		{ .str = ":", .type = lexer::token_type::sign, .is_keyword = false },
		{ .str = ",", .type = lexer::token_type::comma, .is_keyword = false },
		{ .str = "{", .type = lexer::token_type::sign, .is_keyword = false },
		{ .str = "}", .type = lexer::token_type::sign, .is_keyword = false },
		{ .str = "(", .type = lexer::token_type::sign, .is_keyword = false },
		{ .str = ")", .type = lexer::token_type::sign, .is_keyword = false },
		{ .str = "<", .type = lexer::token_type::sign, .is_keyword = false },
		{ .str = ">", .type = lexer::token_type::sign, .is_keyword = false },
		{ .str = "[", .type = lexer::token_type::sign, .is_keyword = false },
		{ .str = "]", .type = lexer::token_type::sign, .is_keyword = false },
		{ .str = "<=", .type = lexer::token_type::sign, .is_keyword = false },
		{ .str = ">=", .type = lexer::token_type::sign, .is_keyword = false },
		{ .str = "!=", .type = lexer::token_type::sign, .is_keyword = false },
		{ .str = "==", .type = lexer::token_type::sign, .is_keyword = false },
		{ .str = "/*", .type = lexer::token_type::comment_begin, .is_keyword = false },
		{ .str = "*/", .type = lexer::token_type::comment_end, .is_keyword = false },
		{ .str = "//", .type = lexer::token_type::comment_line, .is_keyword = false },
		{ .str = "->", .type = lexer::token_type::arrow, .is_keyword = false },
		{ .str = "return", .type = lexer::token_type::_return, .is_keyword = true },
		{ .str = "int", .type = lexer::token_type::_int, .is_keyword = true },
		{ .str = "float", .type = lexer::token_type::_float, .is_keyword = true },
		{ .str = "bool", .type = lexer::token_type::_bool, .is_keyword = true },
		{ .str = "str", .type = lexer::token_type::_str, .is_keyword = true },
		{ .str = "true", .type = lexer::token_type::_true, .is_keyword = true },
		{ .str = "false", .type = lexer::token_type::_false, .is_keyword = true },
		{ .str = "const", .type = lexer::token_type::_const, .is_keyword = true },
		{ .str = "mut", .type = lexer::token_type::_mut, .is_keyword = true },
		{ .str = "if", .type = lexer::token_type::_if, .is_keyword = true },
		{ .str = "else", .type = lexer::token_type::_else, .is_keyword = true },
		{ .str = "fn", .type = lexer::token_type::func, .is_keyword = true },
		{ .str = "while", .type = lexer::token_type::_while, .is_keyword = true },
		{ .str = "do", .type = lexer::token_type::_do, .is_keyword = true },
	};
	if (*con.itr == '\n') {
		++con.itr;
		++con.point.line;
		con.point.col = 0;
		return token { .raw = "\n", .type = token_type::sign, .point = con.point };
	}
	auto start_with = [&con](std::string keyword, std::string::const_iterator _itr) -> int {
		int i = 0;
		for (i; keyword[i]; ++i) {
			if (_itr == con.end ||
				*_itr != keyword[i]) {
				return 0;
			}
			++_itr;
		}
		return i;
	};
	int index = -1, max_len = 0, len = 0;
	for (int i = 0; i < sizeof(keywords) / sizeof(keywords[0]); ++i) {
		len = start_with(keywords[i].str, con.itr);
		if (len && max_len < len) {
			index = i;
			max_len = len;
		}
	}
	con.itr += max_len;
	code_point point = con.point;
	con.point.col += max_len;
	if (index == -1) {
		return std::nullopt;
	}
	if (keywords[index].type != lexer::token_type::sign && (keywords[index].is_keyword && (std::isalnum(*con.itr)  || *con.itr == '_'))) {
		return std::nullopt;
	}
	return token { .raw = keywords[index].str, .type = keywords[index].type, .point = point };
}

std::optional<lexer::token> lexer::try_parse_identifier(lexer::context& con) noexcept {
	std::string str;
	if (!isalpha(*con.itr) && *con.itr != '_') {
		return std::nullopt;
	}
	str += *con.itr++;
	++con.point.col;
	while (con.itr != con.end) {
		if (isalnum(*con.itr) || *con.itr == '_') {
			str += *con.itr++;
			++con.point.col;
		} else {
			break;
		}
	}
	if (str == "_") {
		return token { .raw = str, .type = lexer::token_type::semicolon, .point = con.point };
	}
	return token { .raw = str, .type = lexer::token_type::identifier, .point = con.point };
}

std::optional<lexer::token> lexer::try_parse_string(context& con) noexcept {
	if (*con.itr != '\"') {
		return std::nullopt;
	}
	code_point point = con.point;
	++con.itr;
	std::string str;
	while (con.itr != con.end) {
		if (*con.itr == '\"') {
			++con.itr;
			break;
		}
		str += *con.itr;
		++con.itr;
	}
	if (con.itr == con.end) {
		return std::nullopt;
	}
	return token { .raw = str, .type = lexer::token_type::string, .point = point };
}

std::vector<lexer::token> lexer::tokenize(const std::string& source) noexcept {
	lexer::context con { .point = { .line = 1, .col = 0 }, .itr = source.begin(), .end = source.end() };
	std::vector<token> toks;
	while (con.itr != source.end()) {
		if (std::optional<token> number = try_parse_number(con)) {
			toks.push_back(number.value());
			continue;
		}
		if (std::optional<token> keyword = try_parse_sign_and_keyword(con)) {
			toks.push_back(keyword.value());
			continue;
		}
		if (std::optional<token> identifier = try_parse_identifier(con)) {
			toks.push_back(identifier.value());
			continue;
		}
		if (std::optional<token> string_tok = try_parse_string(con)) {
			toks.push_back(string_tok.value());
			continue;
		}
		if (std::isspace(*con.itr)) {
			do {
				if (*con.itr == '\n') {
					++con.point.line;
					con.point.col = 0;
				} else {
					++con.point.col;
				}
				++con.itr;
			} while (std::isspace(*con.itr));
			continue;
		}

		std::cout << "tokenize error (" << con.point.line << ", " << con.point.col << "):" << *con.itr << std::endl;
		abort();
	}

	toks.push_back(token{ .raw = '\0', .type = token_type::eof, .point = con.point });
	return toks;
}