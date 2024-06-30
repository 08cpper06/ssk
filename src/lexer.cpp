#include "lexer.hpp"
#include <cassert>


std::optional<lexer::token> lexer::try_parse_number(std::string::const_iterator& itr) noexcept {
	std::string tok;
	bool has_point = false;
	while (*itr) {
		if (*itr >= '0' && *itr <= '9') {
			tok += *itr;
		}
		else if (!has_point && *itr == '.') {
			tok += *itr;
			has_point = true;
		}
		else {
			break;
		}
		++itr;
	}
	return tok.empty() ? std::nullopt : std::optional<token>(token{ .raw = std::move(tok), .type = token_type::number });
}
std::optional<lexer::token> lexer::try_parse_sign(std::string::const_iterator& itr) noexcept {
	std::string str;
	switch (*itr) {
	case '+':
	case '-':
	case '*':
	case '/':
	case '=':
	case ':':
	case '{':
	case '}':
	case '\n':
		str = *itr++;
		return token{ .raw = str, .type = token_type::sign };
	case ';':
		str = *itr++;
		return token{ .raw = str, .type = token_type::semicolon };
	default:
		return std::nullopt;
	}
}

std::optional<lexer::token> lexer::try_parse_keyword(std::string::const_iterator& itr, const std::string::const_iterator& end) noexcept {
	struct keyword_info {
		std::string str;
		lexer::token_type type;
	};
	static keyword_info keywords[] = {
		{ .str = "return", .type = lexer::token_type::_return },
		{ .str = "int", .type = lexer::token_type::_int },
		{ .str = "float", .type = lexer::token_type::_float },
		{ .str = "const", .type = lexer::token_type::_const },
		{ .str = "mut", .type = lexer::token_type::_mut },
	};
	auto start_with = [&end](std::string keyword, std::string::const_iterator _itr) -> int {
		int i = 0;
		for (i; keyword[i]; ++i) {
			if (_itr == end ||
				*_itr != keyword[i]) {
				return 0;
			}
			++_itr;
		}
		return i;
	};
	int index = -1, max_len = 0, len = 0;
	for (int i = 0; i < sizeof(keywords) / sizeof(keywords[0]); ++i) {
		len = start_with(keywords[i].str, itr);
		if (len && max_len < len) {
			index = i;
			max_len = len;
		}
	}
	itr += max_len;
	if (index == -1 || std::isalnum(*itr) || *itr == '_') {
		return std::nullopt;
	}
	return token { .raw = keywords[index].str, .type = keywords[index].type };
}

std::optional<lexer::token> lexer::try_parse_identifier(std::string::const_iterator& itr, const std::string::const_iterator& end) noexcept {
	std::string str;
	if (!isalpha(*itr) && *itr != '_') {
		return std::nullopt;
	}
	str += *itr++;
	while (itr != end) {
		if (isalnum(*itr) || *itr == '_') {
			str += *itr++;
		} else {
			break;
		}
	}
	return token { .raw = str, .type = lexer::token_type::identifier };
}

std::vector<lexer::token> lexer::tokenize(const std::string& source) noexcept {
	std::string::const_iterator itr = source.begin();
	std::vector<token> toks;
	while (itr != source.end()) {
		if (std::optional<token> number = try_parse_number(itr)) {
			toks.push_back(number.value());
			continue;
		}
		if (std::optional<token> sign = try_parse_sign(itr)) {
			toks.push_back(sign.value());
			continue;
		}
		if (std::optional<token> keyword = try_parse_keyword(itr, source.end())) {
			toks.push_back(keyword.value());
			continue;
		}
		if (std::optional<token> identifier = try_parse_identifier(itr, source.end())) {
			toks.push_back(identifier.value());
			continue;
		}
		if (std::isspace(*itr)) {
			do {
				++itr;
			} while (std::isspace(*itr));
			continue;
		}
		assert(!*itr);
	}

	toks.push_back(token{ .raw = '\0', .type = token_type::eof});
	return toks;
}