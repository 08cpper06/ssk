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
	case '\n':
		str = *itr++;
		return token{ .raw = str, .type = token_type::sign };
	default:
		return std::nullopt;
	}
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
		assert(!*itr);
	}
	return toks;
}