#include "parser.hpp"
#include <iostream>
#include <cassert>

std::unique_ptr<ast_node_base> parser::try_build_value(std::vector<lexer::token>::const_iterator& itr) {
	if (itr->type != lexer::token_type::number &&
		itr->type != lexer::token_type::identifier) {
		return nullptr;
	}
	return std::make_unique<ast_node_value>(*itr++);
}
std::unique_ptr<ast_node_base> parser::try_build_timedivide_node(std::vector<lexer::token>::const_iterator& itr) {
	std::unique_ptr<ast_node_base> lhs = try_build_value(itr);
	if (!lhs) {
		return nullptr;
	}

	std::unique_ptr<ast_node_bin> node;
	std::string op;
	while (lhs) {
		if (itr->raw == "*" || itr->raw == "/") {
			node = std::make_unique<ast_node_bin>();
			node->op = itr->raw[0];
			node->lhs = std::move(lhs);
			++itr;
			node->rhs = try_build_value(itr);
			lhs = std::move(node);
		}
		else {
			return lhs;
		}
	}
	assert(false);
	return nullptr;
}

std::unique_ptr<ast_node_base> parser::try_build_plusminus_node(std::vector<lexer::token>::const_iterator& itr) {
	std::unique_ptr<ast_node_base> lhs = try_build_timedivide_node(itr);
	if (!lhs || !is_a<ast_node_value>(lhs.get())) {
		return lhs;
	}

	std::unique_ptr<ast_node_bin> node;
	std::string op;
	while (lhs) {
		if (itr->raw == "+" || itr->raw == "-") {
			node = std::make_unique<ast_node_bin>();
			node->op = itr->raw[0];
			node->lhs = std::move(lhs);
			++itr;
			node->rhs = try_build_timedivide_node(itr);
			lhs = std::move(node);
		}
		else {
			return lhs;
		}
	}
	assert(false);
	return nullptr;
}

std::unique_ptr<ast_node_base> parser::try_build_expr(std::vector<lexer::token>::const_iterator& itr) {
	std::unique_ptr<ast_node_base> expr = try_build_plusminus_node(itr);
	if (!expr) {
		return nullptr;
	}
	
	if (itr->type != lexer::token_type::semicolon) {
		std::cout << "not found semicolon" << std::endl;
		return nullptr;
	}
	++itr; /* skip semicolon */
	return std::make_unique<ast_node_expr>(std::move(expr));
}
std::unique_ptr<ast_node_base> parser::try_build_return(std::vector<lexer::token>::const_iterator& itr) {
	if (itr->type != lexer::token_type::_return) {
		return nullptr;
	}
	++itr;
	std::unique_ptr<ast_node_base> node = try_build_expr(itr);
	return std::make_unique<ast_node_return>(std::move(node));
}

std::unique_ptr<ast_node_base> parser::try_build_var_definition(std::vector<lexer::token>::const_iterator& itr) {
	if (itr->type != lexer::token_type::_const &&
		itr->type != lexer::token_type::_mut) {
		return nullptr;
	}
	lexer::token_type modifier = itr->type;
	std::vector<lexer::token>::const_iterator tmp = itr + 1;
	if (tmp->type != lexer::token_type::identifier) {
		return nullptr;
	}
	std::string name = tmp->raw;
	++tmp;

	if (tmp->raw != ":") {
		return nullptr;
	}
	++tmp;

	switch (tmp->type) {
	case lexer::token_type::_int: break;
	case lexer::token_type::_float: break;
	default: return nullptr;
	}
	lexer::token_type type = tmp->type;
	++tmp;

	if (tmp->type == lexer::token_type::semicolon) {
		itr = ++tmp;
		return std::make_unique<ast_node_var_definition>(modifier, name, type);
	}

	if (tmp->raw != "=") {
		std::cout << "expected `=`" << std::endl;
		return nullptr;
	}
	++tmp;
	std::unique_ptr<ast_node_base> init_value = try_build_expr(tmp);
	if (!init_value) {
		std::cout << "initial value is invalid" << std::endl;
		return nullptr;
	}

	itr = tmp;
	return std::make_unique<ast_node_var_definition>(modifier, name, type, std::move(init_value));
}

std::unique_ptr<ast_node_base> parser::try_build_program(std::vector<lexer::token>::const_iterator& itr) {
	std::vector<std::unique_ptr<ast_node_base>> exprs;
	std::unique_ptr<ast_node_base> node;
	for (; itr->type != lexer::token_type::eof;) {
		if (node = try_build_expr(itr)) {
			exprs.push_back(std::move(node));
		} else if (node = try_build_return(itr)) {
			exprs.push_back(std::move(node));
		} else if (node = try_build_var_definition(itr)) {
			exprs.push_back(std::move(node));
		} else if (itr->raw == "\n") {
			++itr;
		} else {
			break;
		}
	}
	return std::make_unique<ast_node_program>(std::move(exprs));
}

std::unique_ptr<ast_node_base> parser::parse(const std::vector<lexer::token>& toks) noexcept {
	std::optional<lexer::token> tok = std::nullopt;
	std::vector<lexer::token>::const_iterator itr = toks.begin();
	return try_build_program(itr);
}
