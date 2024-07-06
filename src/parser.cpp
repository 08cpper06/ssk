#include "parser.hpp"
#include <iostream>
#include <cassert>

std::unique_ptr<ast_node_base> parser::try_build_value(std::vector<lexer::token>::const_iterator& itr) {
	if (itr->type == lexer::token_type::identifier && (itr + 1)->raw == "(") {
		return try_build_call_function(itr);
	}
	if (itr->type != lexer::token_type::number &&
		itr->type != lexer::token_type::_true &&
		itr->type != lexer::token_type::_false &&
		itr->type != lexer::token_type::identifier) {
		return nullptr;
	}
	return std::make_unique<ast_node_value>(*itr++, itr->point);
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
			node->point = itr->point;
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
	while (lhs) {
		if (itr->raw == "+" || itr->raw == "-") {
			node = std::make_unique<ast_node_bin>();
			node->op = itr->raw[0];
			node->lhs = std::move(lhs);
			node->point = itr->point;
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

std::unique_ptr<ast_node_base> parser::try_build_equality(std::vector<lexer::token>::const_iterator& itr) {
	std::unique_ptr<ast_node_base> lhs = try_build_relational(itr);
	if (!lhs) {
		return nullptr;
	}
	if (itr->raw != "==" &&
		itr->raw != "!=") {
		return std::move(lhs);
	}
	std::unique_ptr<ast_node_bin> node = std::make_unique<ast_node_bin>();
	node->lhs = std::move(lhs);
	node->point = itr->point;
	node->op = itr->raw;
	++itr;
	node->rhs = try_build_relational(itr);
	if (!(node->rhs)) {
		return std::make_unique<ast_node_error>("not found right hand of `"+ node->op + "`", node->point);
	}
	return std::move(node);
}

std::unique_ptr<ast_node_base> parser::try_build_relational(std::vector<lexer::token>::const_iterator& itr) {
	std::unique_ptr<ast_node_base> lhs = try_build_plusminus_node(itr);
	if (!lhs) {
		return nullptr;
	}
	if (itr->raw != "<" &&
		itr->raw != ">" &&
		itr->raw != "<=" &&
		itr->raw != ">=") {
		return lhs;
	}
	std::unique_ptr<ast_node_bin> node = std::make_unique<ast_node_bin>();
	node->lhs = std::move(lhs);
	node->point = itr->point;
	node->op = itr->raw;
	++itr;
	node->rhs = try_build_plusminus_node(itr);
	if (!(node->rhs)) {
		return std::make_unique<ast_node_error>("not found right hand of `" + node->op + "`", node->point);
	}
	return std::move(node);
}

std::unique_ptr<ast_node_base> parser::try_build_assign(std::vector<lexer::token>::const_iterator& itr) {
	std::unique_ptr<ast_node_base> lhs = try_build_equality(itr);
	if (!lhs) {
		return nullptr;
	}
	std::unique_ptr<ast_node_bin> node = nullptr;
	while (lhs) {
		if (itr->raw == "=") {
			node = std::make_unique<ast_node_bin>();
			node->op = itr->raw[0];
			node->lhs = std::move(lhs);
			node->point = itr->point;
			++itr;
			node->rhs = try_build_assign(itr);
			lhs = std::move(node);
		} else {
			return lhs;
		}
	}
	assert(false);
	return nullptr;
}

std::unique_ptr<ast_node_base> parser::try_build_expr(std::vector<lexer::token>::const_iterator& itr) {
	std::unique_ptr<ast_node_base> expr = try_build_assign(itr);
	if (!expr) {
		return nullptr;
	}
	
	if (itr->type != lexer::token_type::semicolon) {
		std::cout << "not found semicolon" << std::endl;
		return std::make_unique<ast_node_error>("not found semicolon", itr->point);
	}
	++itr;
	return std::make_unique<ast_node_expr>(std::move(expr), expr->point);
}

std::unique_ptr<ast_node_base> parser::try_build_call_function(std::vector<lexer::token>::const_iterator& itr) {
	if (itr->type != lexer::token_type::identifier) {
		return nullptr;
	}
	std::string name = itr->raw;
	code_point point = itr->point;
	if ((itr + 1)->raw != "(") {
		return nullptr;
	}
	++itr;
	++itr;
	
	std::vector<std::unique_ptr<ast_node_base>> arguments;
	std::unique_ptr<ast_node_base> node;
	while (itr->type != lexer::token_type::eof) {
		node = try_build_assign(itr);
		if (!node) {
			node = std::make_unique<ast_node_error>("argument is invalid", point);
		}
		arguments.push_back(std::move(node));
		if (itr->raw == ")") {
			break;
		}
		if (itr->type != lexer::token_type::comma) {
			return std::make_unique<ast_node_error>("not found `,`", point);
		}
		++itr;
	}
	if (itr->raw != ")") {
		return std::make_unique<ast_node_error>("expected `)`", point);
	}
	++itr;
	return std::make_unique<ast_node_call_function>(name, std::move(arguments), point);
}

std::unique_ptr<ast_node_base> parser::try_build_return(std::vector<lexer::token>::const_iterator& itr) {
	if (itr->type != lexer::token_type::_return) {
		return nullptr;
	}
	++itr;
	std::unique_ptr<ast_node_base> node = try_build_expr(itr);
	return std::make_unique<ast_node_return>(std::move(node), itr->point);
}

std::unique_ptr<ast_node_base> parser::try_build_var_definition(std::vector<lexer::token>::const_iterator& itr) {
	code_point point { 0, 0 };
	if (itr->type != lexer::token_type::_const &&
		itr->type != lexer::token_type::_mut) {
		return nullptr;
	}
	lexer::token_type modifier = itr->type;
	std::vector<lexer::token>::const_iterator tmp = itr + 1;
	if (tmp->type != lexer::token_type::identifier) {
		itr = tmp;
		std::cout << "expected identifier" << std::endl;
		return std::make_unique<ast_node_error>("expected identifier", tmp->point);
	}
	std::string name = tmp->raw;
	point = tmp->point;
	++tmp;

	if (tmp->raw != ":") {
		itr = tmp;
		std::cout << "expected `:`" << std::endl;
		return std::make_unique<ast_node_error>("expected identifier", tmp->point);
	}
	++tmp;

	switch (tmp->type) {
	case lexer::token_type::_int: break;
	case lexer::token_type::_float: break;
	case lexer::token_type::_bool: break;
	default:
		itr = tmp;
		std::cout << "invalid type (" << tmp->raw << ")" << std::endl;
		return std::make_unique<ast_node_error>("invalid type: " + tmp->raw, itr->point);
	}
	lexer::token_type type = tmp->type;
	++tmp;

	if (tmp->type == lexer::token_type::semicolon) {
		itr = ++tmp;
		return std::make_unique<ast_node_var_definition>(modifier, name, type, point);
	}

	if (tmp->raw != "=") {
		itr = tmp;
		std::cout << "expected `=`" << std::endl;
		return std::make_unique<ast_node_error>("expected `=`", itr->point);
	}
	point = tmp->point;
	++tmp;
	std::unique_ptr<ast_node_base> init_value = try_build_expr(tmp);
	if (!init_value) {
		itr = tmp;
		std::cout << "initial value is invalid" << std::endl;
		return std::make_unique<ast_node_error>("initial value is invalid", itr->point);
	}

	itr = tmp;
	return std::make_unique<ast_node_var_definition>(modifier, name, type, std::move(init_value), point);
}

std::unique_ptr<ast_node_base> parser::try_build_if(std::vector<lexer::token>::const_iterator& itr) {
	if (itr->type != lexer::token_type::_if) {
		return nullptr;
	}
	++itr;
	if (itr->raw != "(") {
		return std::make_unique<ast_node_error>("expected `(`", itr->point);
	}
	++itr;
	std::unique_ptr<ast_node_base> cond = try_build_assign(itr);
	if (itr->raw != ")") {
		return std::make_unique<ast_node_error>("expected `)`", itr->point);
	}
	++itr;
	std::unique_ptr<ast_node_base> true_block = try_build_block(itr);
	if (true_block) {
		if (ast_node_block* block = static_cast<ast_node_block*>(true_block.get())) {
			block->block_name = "true_" + block->block_name;
		}
	}

	if (itr->type != lexer::token_type::_else) {
		return std::make_unique<ast_node_if>(std::move(cond), std::move(true_block));
	}
	++itr;
	std::unique_ptr<ast_node_base> false_block = try_build_block(itr);
	if (false_block) {
		if (ast_node_block* block = static_cast<ast_node_block*>(false_block.get())) {
			block->block_name = "false_" + block->block_name;
		}
	}

	return std::make_unique<ast_node_if>(std::move(cond), std::move(true_block), std::move(false_block));
}

std::unique_ptr<ast_node_base> parser::try_build_block(std::vector<lexer::token>::const_iterator& itr) {
	if (itr->raw != "{") {
		return nullptr;
	}
	++itr;
	std::vector<std::unique_ptr<ast_node_base>> exprs;
	std::unique_ptr<ast_node_base> node;
	for (; itr->type != lexer::token_type::eof;) {
		if (node = try_build_if(itr)) {
			exprs.push_back(std::move(node));
		} else if (node = try_build_expr(itr)) {
			exprs.push_back(std::move(node));
		} else if (node = try_build_return(itr)) {
			exprs.push_back(std::move(node));
		} else if (node = try_build_var_definition(itr)) {
			exprs.push_back(std::move(node));
		} else if (isspace(itr->raw[0]) || itr->raw == ";") {
			++itr;
		}
		else {
			break;
		}
	}
	if (itr->raw != "}") {
		return std::make_unique<ast_node_error>("expeceted `}`", itr->point);
	}
	++itr;
	return std::make_unique<ast_node_block>(std::move(exprs), itr->point);
}

std::unique_ptr<ast_node_base> parser::try_build_function(std::vector<lexer::token>::const_iterator& itr) {
	if (itr->type != lexer::token_type::func) {
		return nullptr;
	}
	++itr;
	if (itr->type != lexer::token_type::identifier) {
		return std::make_unique<ast_node_error>("expeceted function name", itr->point);
	}
	std::string func_name = itr->raw;
	code_point point = itr->point;
	++itr;
	if (itr->raw != "(") {
		return std::make_unique<ast_node_error>("expeceted function name", itr->point);
	}
	++itr;
	std::vector<ast_node_function::var_type> args;
	ast_node_function::var_type var;
	if (itr->raw != ")") {
		while (itr->type == lexer::token_type::_const || itr->type == lexer::token_type::_mut) {
			switch (itr->type) {
			case lexer::token_type::_const: break;
			case lexer::token_type::_mut: break;
			default:
				std::cout << "invalid modifier (" << itr->raw << ")" << std::endl;
				return std::make_unique<ast_node_error>("invalid modifier: " + itr->raw, itr->point);
			}
			var.modifier = itr->type;
			++itr;

			if (itr->type != lexer::token_type::identifier) {
				std::cout << "expected identifier" << std::endl;
				return std::make_unique<ast_node_error>("expected identifier", itr->point);
			}
			var.name = itr->raw;
			++itr;

			if (itr->raw != ":") {
				std::cout << "expected `:`" << std::endl;
				return std::make_unique<ast_node_error>("expected identifier", itr->point);
			}
			++itr;

			switch (itr->type) {
			case lexer::token_type::_int: break;
			case lexer::token_type::_float: break;
			case lexer::token_type::_bool: break;
			default:
				std::cout << "invalid type (" << itr->raw << ")" << std::endl;
				return std::make_unique<ast_node_error>("expeceted type (" + itr->raw + ")", point);
			}
			var.type = itr->type;
			++itr;
			args.push_back(var);

			if (itr->raw == ")") {
				break;
			}
			if (itr->type != lexer::token_type::comma) {
				return std::make_unique<ast_node_error>("expected `)` or `,`", point);
			}
			++itr;
		}
	}
	++itr;
	if (itr->type != lexer::token_type::arrow) {
		return std::make_unique<ast_node_error>("expeceted `->`", point);
	}
	++itr;
	switch (itr->type) {
	case lexer::token_type::_int: break;
	case lexer::token_type::_float: break;
	case lexer::token_type::_bool: break;
	default:
		std::cout << "invalid type (" << itr->raw << ")" << std::endl;
		return std::make_unique<ast_node_error>("expeceted type (" + itr->raw + ")", point);
	}
	lexer::token_type return_type = itr->type;
	++itr;

	std::unique_ptr<ast_node_base> block = try_build_block(itr);
	if (ast_node_block* casted_block = dynamic_cast<ast_node_block*>(block.get())) {
		casted_block->block_name = func_name;
	}
	std::unique_ptr<ast_node_function> func = std::make_unique<ast_node_function>();
	func->block = std::move(block);
	func->return_type = return_type;
	func->function_name = func_name;
	func->point = point;
	func->arguments = std::move(args);
	return std::move(func);
}

bool parser::try_skip_comment(std::vector<lexer::token>::const_iterator& itr) {
	if (itr->type == lexer::token_type::comment_begin) {
		int nest_count = 1;
		++itr;
		while (itr->type != lexer::token_type::eof) {
			if (itr->type == lexer::token_type::comment_begin) {
				++nest_count;
			} else if (itr->type == lexer::token_type::comment_end) {
				++itr;
				--nest_count;
				if (!nest_count) {
					return true;
				}
				continue;
			}
			++itr;
		}
	} else if (itr->type == lexer::token_type::comment_line) {
		while (itr->type != lexer::token_type::eof) {
			if (itr->raw == "\n") {
				++itr;
				return true;
			}
			++itr;
		}
	}
	return false;
}

std::unique_ptr<ast_node_base> parser::try_build_program(std::vector<lexer::token>::const_iterator& itr) {
	std::vector<std::unique_ptr<ast_node_base>> exprs;
	std::unique_ptr<ast_node_base> node;
	for (; itr->type != lexer::token_type::eof;) {
		if (try_skip_comment(itr)) {
			continue;
		}
		if (node = try_build_function(itr)) {
			exprs.push_back(std::move(node));
		} else if (node = try_build_if(itr)) {
			exprs.push_back(std::move(node));
		} else if (node = try_build_expr(itr)) {
			exprs.push_back(std::move(node));
		} else if (node = try_build_return(itr)) {
			exprs.push_back(std::move(node));
		} else if (node = try_build_var_definition(itr)) {
			exprs.push_back(std::move(node));
		} else if (node = try_build_block(itr)) {
			exprs.push_back(std::move(node));
		} else if (isspace(itr->raw[0]) || itr->raw == ";") {
			++itr;
		} else {
			break;
		}
	}
	return std::make_unique<ast_node_program>(std::move(exprs), itr->point);
}

std::unique_ptr<ast_node_base> parser::parse(const std::vector<lexer::token>& toks) noexcept {
	std::optional<lexer::token> tok = std::nullopt;
	std::vector<lexer::token>::const_iterator itr = toks.begin();
	return try_build_program(itr);
}
