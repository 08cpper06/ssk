#include "parser.hpp"
#include <iostream>
#include <cassert>


void parser::skip_until_semicolon(std::vector<lexer::token>::const_iterator& itr) {
	while (itr->type != lexer::token_type::eof) {
		if (itr->type == lexer::token_type::semicolon) {
			return;
		}
		++itr;
	}
}

std::unique_ptr<ast_node_base> parser::try_build_value(context& con, std::vector<lexer::token>::const_iterator& itr) {
	if (itr->type == lexer::token_type::identifier && (itr + 1)->raw == "(") {
		return try_build_call_function(con, itr);
	}
	if (itr->type == lexer::token_type::identifier && (itr + 1)->raw == "[") {
		return try_build_reference_array(con, itr);
	}
	if (itr->type == lexer::token_type::string) {
		return std::make_unique<ast_node_string>(*itr++, itr->point);
	}
	if (itr->raw == "{") {
		return try_build_initial_list(con, itr);
	}
	if (itr->type != lexer::token_type::number &&
		itr->type != lexer::token_type::_true &&
		itr->type != lexer::token_type::_false &&
		itr->type != lexer::token_type::identifier) {
		return nullptr;
	}
	return std::make_unique<ast_node_value>(*itr++, itr->point);
}

std::unique_ptr<ast_node_base> parser::try_build_repeat(context& con, std::vector<lexer::token>::const_iterator& itr) {
	std::unique_ptr<ast_node_base> bgn = try_build_value(con, itr);
	if (!bgn) {
		return nullptr;
	}
	if (itr->type != lexer::token_type::repeat) {
		return bgn;
	}
	++itr;
	std::unique_ptr<ast_node_base> end = try_build_value(con, itr);
	if (!end) {
		return std::make_unique<ast_node_error>("expected end value", itr->point);
	}
	return std::make_unique<ast_node_repeat>(std::move(bgn), std::move(end), itr->point);
}

std::unique_ptr<ast_node_base> parser::try_build_timedivide_node(context& con, std::vector<lexer::token>::const_iterator& itr) {
	std::unique_ptr<ast_node_base> lhs = try_build_repeat(con, itr);
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
			node->rhs = try_build_repeat(con, itr);
			lhs = std::move(node);
		}
		else {
			return lhs;
		}
	}
	assert(false);
	return nullptr;
}

std::unique_ptr<ast_node_base> parser::try_build_plusminus_node(context& con, std::vector<lexer::token>::const_iterator& itr) {
	std::unique_ptr<ast_node_base> lhs = try_build_timedivide_node(con, itr);
	if (!lhs || is_a<ast_node_error>(lhs.get())) {
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
			node->rhs = try_build_timedivide_node(con, itr);
			lhs = std::move(node);
		}
		else {
			return lhs;
		}
	}
	assert(false);
	return nullptr;
}

std::unique_ptr<ast_node_base> parser::try_build_equality(context& con, std::vector<lexer::token>::const_iterator& itr) {
	std::unique_ptr<ast_node_base> lhs = try_build_relational(con, itr);
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
	node->rhs = try_build_relational(con, itr);
	if (!(node->rhs)) {
		return std::make_unique<ast_node_error>("not found right hand of `"+ node->op + "`", node->point);
	}
	return std::move(node);
}

std::unique_ptr<ast_node_base> parser::try_build_relational(context& con, std::vector<lexer::token>::const_iterator& itr) {
	std::unique_ptr<ast_node_base> lhs = try_build_plusminus_node(con, itr);
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
	node->rhs = try_build_plusminus_node(con, itr);
	if (!(node->rhs)) {
		return std::make_unique<ast_node_error>("not found right hand of `" + node->op + "`", node->point);
	}
	return std::move(node);
}

std::unique_ptr<ast_node_base> parser::try_build_assign(context& con, std::vector<lexer::token>::const_iterator& itr) {
	std::unique_ptr<ast_node_base> lhs = try_build_equality(con, itr);
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
			node->rhs = try_build_assign(con, itr);
			lhs = std::move(node);
		} else {
			return lhs;
		}
	}
	assert(false);
	return nullptr;
}

std::unique_ptr<ast_node_base> parser::try_build_expr(context& con, std::vector<lexer::token>::const_iterator& itr) {
	std::unique_ptr<ast_node_base> expr = try_build_assign(con, itr);
	if (!expr) {
		return nullptr;
	}
	
	if (itr->type != lexer::token_type::semicolon) {
		return std::make_unique<ast_node_error>("not found semicolon", itr->point);
	}
	++itr;
	return std::make_unique<ast_node_expr>(std::move(expr), expr->point);
}

std::unique_ptr<ast_node_base> parser::try_build_call_function(context& con, std::vector<lexer::token>::const_iterator& itr) {
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
		node = try_build_assign(con, itr);
		if (!node) {
			skip_until_semicolon(itr);
			node = std::make_unique<ast_node_error>("argument is invalid", point);
		}
		arguments.push_back(std::move(node));
		if (itr->raw == ")") {
			break;
		}
		if (itr->type != lexer::token_type::comma) {
			skip_until_semicolon(itr);
			return std::make_unique<ast_node_error>("not found `,`", point);
		}
		++itr;
	}
	if (itr->raw != ")") {
		skip_until_semicolon(itr);
		return std::make_unique<ast_node_error>("expected `)`", point);
	}
	++itr;
	return std::make_unique<ast_node_call_function>(name, std::move(arguments), point);
}

std::unique_ptr<ast_node_base> parser::try_build_reference_array(context& con, std::vector<lexer::token>::const_iterator& itr) {
	if (itr->type != lexer::token_type::identifier) {
		return nullptr;
	}
	lexer::token name = *itr;
	std::vector<lexer::token>::const_iterator tmp = itr;
	++tmp;
	if (tmp->raw != "[") {
		return nullptr;
	}
	code_point point = tmp->point;
	++tmp;
	std::unique_ptr<ast_node_base> index = try_build_assign(con, tmp);
	if (tmp->raw != "]") {
		itr = tmp;
		return std::make_unique<ast_node_error>("expected `]`", tmp->point);
	}
	itr = ++tmp;
	return std::make_unique<ast_node_array_refernce>(name, std::move(index), point);
}

std::unique_ptr<ast_node_base> parser::try_build_return(context& con, std::vector<lexer::token>::const_iterator& itr) {
	if (itr->type != lexer::token_type::_return) {
		return nullptr;
	}
	++itr;
	std::unique_ptr<ast_node_base> node = try_build_expr(con, itr);
	if (!node) {
		skip_until_semicolon(itr);
		if (itr->type == lexer::token_type::semicolon) {
			++itr;
		}
	}
	return std::make_unique<ast_node_return>(std::move(node), itr->point);
}

std::unique_ptr<ast_node_base> parser::try_build_var_definition(context& con, std::vector<lexer::token>::const_iterator& itr) {
	code_point point { 0, 0 };
	if (itr->type != lexer::token_type::_const &&
		itr->type != lexer::token_type::_mut) {
		return nullptr;
	}
	lexer::token_type modifier = itr->type;
	std::vector<lexer::token>::const_iterator tmp = itr + 1;
	lexer::token var_name_token = *tmp;
	point = tmp->point;
	++tmp;

	if (tmp->raw != ":") {
		itr = tmp;
		std::cout << "expected `:`" << std::endl;
		skip_until_semicolon(itr);
		if (itr->type == lexer::token_type::semicolon) {
			++itr;
		}
		return std::make_unique<ast_node_error>("expected identifier", tmp->point);
	}
	++tmp;

	lexer::token_type type = tmp->type;
	switch (tmp->type) {
	case lexer::token_type::_int: break;
	case lexer::token_type::_float: break;
	case lexer::token_type::_bool: break;
	case lexer::token_type::_str: break;
	default:
		type = lexer::token_type::unknown;
		point = itr->point;
		break;
	}
	++tmp;
	int size = -1;
	bool is_integer_dim = false;
	if (tmp->raw == "[") {
		size = 0;
		++tmp;
		if (tmp->type == lexer::token_type::number) {
			is_integer_dim = tmp->raw.find(".") == std::string::npos;
			size = std::atoi(tmp->raw.c_str());
			++tmp;
		} else if (tmp->raw != "]") {
			itr = tmp;
			std::cout << "expected `]`" << tmp->raw << ")" << std::endl;
			skip_until_semicolon(itr);
			if (itr->type == lexer::token_type::semicolon) {
				++itr;
			}
			return std::make_unique<ast_node_error>("expected `]`", itr->point);
		} else {
			is_integer_dim = true;
		}
		++tmp;
	}

	if (tmp->type == lexer::token_type::semicolon) {
		itr = ++tmp;
		if (type != lexer::token_type::unknown) {
			return std::make_unique<ast_node_error>("invalid type : " + tmp->raw, point);
		}
		if (var_name_token.type != lexer::token_type::identifier) {
			return std::make_unique<ast_node_error>("expected identifier", point);
		}
		if (!is_integer_dim) {
			return std::make_unique<ast_node_error>("dimension should be integer", point);
		}

		return std::make_unique<ast_node_var_definition>(modifier, var_name_token.raw, context::cast_from_token(type, size >= 0), size, point);
	}

	if (tmp->raw != "=") {
		itr = tmp;
		skip_until_semicolon(itr);
		if (itr->type == lexer::token_type::semicolon) {
			++itr;
		}
		return std::make_unique<ast_node_error>("expected `=`", itr->point);
	}
	point = tmp->point;
	++tmp;
	std::unique_ptr<ast_node_base> init_value;
	if (size < 0) {
		init_value = try_build_expr(con, tmp);
		if (!init_value) {
			itr = tmp;
			skip_until_semicolon(itr);
			if (itr->type == lexer::token_type::semicolon) {
				++itr;
			}
			return std::make_unique<ast_node_error>("initial value is invalid", itr->point);
		}
	} else {
		init_value = try_build_initial_list(con, tmp);
		if (!init_value) {
			init_value = try_build_expr(con, tmp);
			if (!init_value && !is_a<ast_node_repeat>(init_value.get())) {
				itr = tmp;
				skip_until_semicolon(itr);
				if (itr->type == lexer::token_type::semicolon) {
					++itr;
				}
				return std::make_unique<ast_node_error>("initial value is invalid", itr->point);
			}
		}
		if (!is_a<ast_node_expr>(init_value.get()) && tmp->type != lexer::token_type::semicolon) {
			itr = tmp;
			return std::make_unique<ast_node_error>("not found semicolon", itr->point);
		}
		if (!is_integer_dim) {
			itr = tmp;
			return std::make_unique<ast_node_error>("dimension should be integer", point);
		}
		if (is_a<ast_node_initial_list>(init_value.get())) {
			ast_node_initial_list* values = static_cast<ast_node_initial_list*>(init_value.get());
			if (size == 0) {
				size = values->values.size();
			} else if (size < values->values.size()) {
				std::cout << "the array size < the size of initialize_list (" << size << "<" << values->values.size() << ")" << std::endl;
				while (tmp->type != lexer::token_type::eof && tmp->raw != "}") {
					++tmp;
				}
				itr = tmp;
				return std::make_unique<ast_node_error>("the array size < the size of initialize_list (" + std::to_string(size) + "<" + std::to_string(values->values.size()) + ")", itr->point);
			}
		}
		++tmp;
	}

	itr = tmp;
	if (var_name_token.type != lexer::token_type::identifier) {
		return std::make_unique<ast_node_error>("expected identifier", point);
	}
	return std::make_unique<ast_node_var_definition>(modifier, var_name_token.raw, context::cast_from_token(type, size >= 0), size, std::move(init_value), point);
}

std::unique_ptr<ast_node_base> parser::try_build_initial_list(context& con, std::vector<lexer::token>::const_iterator& itr) {
	if (itr->raw != "{") {
		return nullptr;
	}
	code_point bgn_point = itr->point;
	++itr;
	std::vector<std::unique_ptr<ast_node_base>> values;
	while (itr->raw != "}") {
		std::unique_ptr<ast_node_base> value = try_build_repeat(con, itr);
		if (!value) {
			values.push_back(std::make_unique<ast_node_error>("invalid token in the initialize_list", itr->point));
		} else {
			values.push_back(std::move(value));
		}
		if (itr->raw == "}") {
			break;
		}
		if (itr->raw != ",") {
			values.push_back(std::make_unique<ast_node_error>("expected `,`", itr->point));
		}
		++itr;
		if (itr->type == lexer::token_type::eof) {
			return std::make_unique<ast_node_error>("expected `}`", bgn_point);
		}
	}
	++itr;
	return std::make_unique<ast_node_initial_list>(std::move(values), bgn_point);
}

std::unique_ptr<ast_node_base> parser::try_build_if(context& con, std::vector<lexer::token>::const_iterator& itr) {
	if (itr->type != lexer::token_type::_if) {
		return nullptr;
	}
	++itr;
	if (itr->raw != "(") {
		return std::make_unique<ast_node_error>("expected `(`", itr->point);
	}
	++itr;
	std::unique_ptr<ast_node_base> cond = try_build_assign(con, itr);
	if (itr->raw != ")") {
		return std::make_unique<ast_node_error>("expected `)`", itr->point);
	}
	++itr;
	std::unique_ptr<ast_node_base> true_block = try_build_block(con, itr);
	if (true_block) {
		if (ast_node_block* block = static_cast<ast_node_block*>(true_block.get())) {
			block->block_name = "true_" + block->block_name;
		}
	}

	if (itr->type != lexer::token_type::_else) {
		return std::make_unique<ast_node_if>(std::move(cond), std::move(true_block));
	}
	++itr;
	std::unique_ptr<ast_node_base> false_block = try_build_block(con, itr);
	if (false_block) {
		if (ast_node_block* block = static_cast<ast_node_block*>(false_block.get())) {
			block->block_name = "false_" + block->block_name;
		}
	}

	return std::make_unique<ast_node_if>(std::move(cond), std::move(true_block), std::move(false_block));
}

std::unique_ptr<ast_node_base> parser::try_build_while(context& con, std::vector<lexer::token>::const_iterator& itr) {
	if (itr->type != lexer::token_type::_while) {
		return nullptr;
	}
	code_point point = itr->point;
	++itr;
	if (itr->raw != "(") {
		return std::make_unique<ast_node_error>("expected `(`", point);
	}
	++itr;
	std::unique_ptr<ast_node_base> condition = try_build_assign(con, itr);
	if (!condition) {
		return std::make_unique<ast_node_error>("expected condition expression", point);
	}
	if (itr->raw != ")") {
		return std::make_unique<ast_node_error>("expected `)`", point);
	}
	++itr;
	std::unique_ptr<ast_node_base> block = try_build_block(con, itr);
	return std::make_unique<ast_node_while>(std::move(condition), std::move(block), point);
}

std::unique_ptr<ast_node_base> parser::try_build_do_while(context& con, std::vector<lexer::token>::const_iterator& itr) {
	if (itr->type != lexer::token_type::_do) {
		return nullptr;
	}
	code_point point = itr->point;
	++itr;
	std::unique_ptr<ast_node_base> block = try_build_block(con, itr);
	if (!block) {
		return std::make_unique<ast_node_error>("expected code block", point);
	}
	
	if (itr->type != lexer::token_type::_while) {
		return std::make_unique<ast_node_error>("expected `while`", point);
	}
	++itr;
	if (itr->raw != "(") {
		return std::make_unique<ast_node_error>("expected `(`", point);
	}
	++itr;
	std::unique_ptr<ast_node_base> condition = try_build_assign(con, itr);
	if (!condition) {
		return std::make_unique<ast_node_error>("expected condition expression", point);
	}
	if (itr->raw != ")") {
		return std::make_unique<ast_node_error>("expected `)`", point);
	}
	++itr;
	if (itr->type != lexer::token_type::semicolon) {
		return std::make_unique<ast_node_error>("expected `;`", point);
	}
	return std::make_unique<ast_node_do_while>(std::move(condition), std::move(block), point);
}

std::unique_ptr<ast_node_base> parser::try_build_block(context& con, std::vector<lexer::token>::const_iterator& itr) {
	if (itr->raw != "{") {
		return nullptr;
	}
	++itr;
	std::vector<std::unique_ptr<ast_node_base>> exprs;
	std::unique_ptr<ast_node_base> node;
	for (; itr->type != lexer::token_type::eof;) {
		if (try_skip_comment(itr)) {
			continue;
		}
		if (node = try_build_do_while(con, itr)) {
			exprs.push_back(std::move(node));
		} else if (node = try_build_while(con, itr)) {
			exprs.push_back(std::move(node));
		} else if (node = try_build_if(con, itr)) {
			exprs.push_back(std::move(node));
		} else if (node = try_build_expr(con, itr)) {
			exprs.push_back(std::move(node));
		} else if (node = try_build_return(con, itr)) {
			exprs.push_back(std::move(node));
		} else if (node = try_build_var_definition(con, itr)) {
			exprs.push_back(std::move(node));
		} else if (node = try_build_function(con, itr)) {
			exprs.push_back(std::make_unique<ast_node_error>("could not define function in the block", node->point));
		} else if (isspace(itr->raw[0]) || itr->raw == ";") {
			++itr;
		} else {
			break;
		}
	}
	if (itr->raw != "}") {
		exprs.push_back(std::make_unique<ast_node_error>("expeceted `}`", itr->point));
		return std::make_unique<ast_node_block>(std::move(exprs), itr->point);
	}
	if (itr->type != lexer::token_type::eof) {
		++itr;
	}
	return std::make_unique<ast_node_block>(std::move(exprs), itr->point);
}

std::unique_ptr<ast_node_base> parser::try_build_function(context& con, std::vector<lexer::token>::const_iterator& itr) {
	if (itr->type != lexer::token_type::func) {
		return nullptr;
	}
	++itr;
	lexer::token func_name_token = *itr;
	code_point point = itr->point;
	++itr;
	if (itr->raw != "(") {
		return std::make_unique<ast_node_error>("expeceted function name", itr->point);
	}
	++itr;
	std::vector<context::var_info> args;
	context::var_info var;
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
			case lexer::token_type::_int:
				++itr;
				var.type = context::var_type::_int;
				if (itr->raw == "[") {
					var.type = context::var_type::_int_array;
					++itr;
					if (itr->raw != "]") {
						var.type = context::var_type::_invalid;
					} else {
						++itr;
					}
				}
				break;
			case lexer::token_type::_float:
				++itr;
				var.type = context::var_type::_float;
				if (itr->raw == "[") {
					var.type = context::var_type::_float_array;
					++itr;
					if (itr->raw != "]") {
						var.type = context::var_type::_invalid;
					} else {
						++itr;
					}
				}
				break;
			case lexer::token_type::_bool:
				++itr;
				var.type = context::var_type::_bool;
				if (itr->raw == "[") {
					var.type = context::var_type::_bool_array;
					++itr;
					if (itr->raw != "]") {
						var.type = context::var_type::_invalid;
					} else {
						++itr;
					}
				}
				break;
			case lexer::token_type::_str:
				++itr;
				var.type = context::var_type::_str;
				if (itr->raw == "[") {
					var.type = context::var_type::_str_array;
					++itr;
					if (itr->raw != "]") {
						var.type = context::var_type::_invalid;
					} else {
						++itr;
					}
				}
				break;
			default:
				std::cout << "invalid type (" << itr->raw << ")" << std::endl;
				return std::make_unique<ast_node_error>("invalid type (" + itr->raw + ")", point);
			}

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
	lexer::token return_type_token = *itr++;

	int return_type_size = -1;
	if (itr->raw == "[") {
		++itr;
		if (itr->raw == "]") {
			return_type_size = 0;
			++itr;
		} else {
			return std::make_unique<ast_node_error>("return type dimension is empty", itr->point);
		}
	}

	std::unique_ptr<ast_node_base> block = try_build_block(con, itr);
	if (ast_node_block* casted_block = dynamic_cast<ast_node_block*>(block.get())) {
		casted_block->block_name = func_name_token.raw;
	}

	context::var_type return_type = context::cast_from_token(return_type_token.type, return_type_size >= 0);
	if (return_type == context::var_type::_invalid) {
		std::cout << "invalid type (" << itr->raw << ")" << std::endl;
		return std::make_unique<ast_node_error>("expeceted type (" + itr->raw + ")", point);
	}

	if (func_name_token.type != lexer::token_type::identifier) {
		return std::make_unique<ast_node_error>("expeceted function name", itr->point);
	}

	std::unique_ptr<ast_node_function> func = std::make_unique<ast_node_function>();
	func->block = std::move(block);
	func->return_type = return_type;
	func->return_type_size = return_type_size;
	func->function_name = func_name_token.raw;
	func->point = point;
	func->arguments = std::move(args);
	con.pre_evaluate.push_back(func.get());
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

std::unique_ptr<ast_node_base> parser::try_build_program(context& con, std::vector<lexer::token>::const_iterator& itr) {
	std::vector<std::unique_ptr<ast_node_base>> exprs;
	std::unique_ptr<ast_node_base> node;
	for (; itr->type != lexer::token_type::eof;) {
		if (try_skip_comment(itr)) {
			continue;
		}
		if (node = try_build_function(con, itr)) {
			exprs.push_back(std::move(node));
		} else if (node = try_build_do_while(con, itr)) {
			exprs.push_back(std::move(node));
		} else if (node = try_build_while(con, itr)) {
			exprs.push_back(std::move(node));
		} else if (node = try_build_if(con, itr)) {
			exprs.push_back(std::move(node));
		} else if (node = try_build_expr(con, itr)) {
			exprs.push_back(std::move(node));
		} else if (node = try_build_return(con, itr)) {
			exprs.push_back(std::move(node));
		} else if (node = try_build_var_definition(con, itr)) {
			exprs.push_back(std::move(node));
		} else if (node = try_build_block(con, itr)) {
			exprs.push_back(std::move(node));
		} else if (isspace(itr->raw[0]) || itr->raw == ";") {
			++itr;
		} else {
			break;
		}
	}
	return std::make_unique<ast_node_program>(std::move(exprs), itr->point);
}

std::unique_ptr<ast_node_base> parser::parse(context& con, const std::vector<lexer::token>& toks) noexcept {
	std::optional<lexer::token> tok = std::nullopt;
	std::vector<lexer::token>::const_iterator itr = toks.begin();
	return try_build_program(con, itr);
}
