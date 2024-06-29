#include "parser.hpp"
#include <cassert>

std::unique_ptr<ast_node_base> parser::try_build_value(std::vector<lexer::token>::const_iterator& itr) {
	if (itr->type != lexer::token_type::number) {
		return nullptr;
	}
	return std::make_unique<ast_node_value>(*itr++);
}
std::unique_ptr<ast_node_base> parser::try_build_timedivide_node(std::vector<lexer::token>::const_iterator& itr) {
	std::unique_ptr<ast_node_base> lhs = try_build_value(itr);
	if (!lhs) {
		return nullptr;
	}
	std::string op;
	if (itr->type == lexer::token_type::sign &&
		(itr->raw == "*" || itr->raw == "/")) {
		op = itr->raw;
	}
	else {
		return lhs;
	}
	++itr;
	std::unique_ptr<ast_node_base> rhs = try_build_value(itr);
	++itr;
	return std::make_unique<ast_node_bin>(op, std::move(lhs), std::move(rhs));
}

std::unique_ptr<ast_node_base> parser::try_build_plusminus_node(std::vector<lexer::token>::const_iterator& itr) {
	std::unique_ptr<ast_node_base> lhs = try_build_timedivide_node(itr);
	if (!is_a<ast_node_value>(lhs.get())) {
		return lhs;
	}

	std::string op;
	if (itr->type == lexer::token_type::sign &&
		(itr->raw == "+" || itr->raw == "-")) {
		op = itr->raw;
	}
	else {
		return lhs;
	}
	++itr;
	std::unique_ptr<ast_node_base> rhs = try_build_timedivide_node(itr);
	assert(rhs);
	return std::make_unique<ast_node_bin>(op, std::move(lhs), std::move(rhs));
}

std::unique_ptr<ast_node_base> parser::parse(const std::vector<lexer::token>& toks) noexcept {
	std::optional<lexer::token> tok = std::nullopt;
	std::vector<lexer::token>::const_iterator itr = toks.begin();
	return try_build_plusminus_node(itr);
}