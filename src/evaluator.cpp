#include "parser.hpp"
#include <iostream>


std::string ast_evaluator::encode(const context& con, const std::string& name) {
	std::string encoded_name;
	for (const auto& item : con.name_space) {
		encoded_name += item + ".";
	}
	return encoded_name + name;
}

std::map<std::string, context::info>::iterator ast_evaluator::find_var(context& con, const std::string name) {
	std::string _namespace;
	std::map<std::string, context::info>::iterator itr = con.var_table.find(name);
	std::map<std::string, context::info>::iterator tmp;
	for (const auto& item : con.name_space) {
		_namespace += item + ".";
		tmp = con.var_table.find(_namespace + name);
		if (tmp != con.var_table.end()) {
			itr = tmp;
		}
	}
	return itr;
}

int ast_node_error::evaluate(context& con) {
	std::cout << "runtime error (" << point.line << ", " << point.col << "): syntax error(" << text << ")" << std::endl;
	abort();
}

int ast_node_value::evaluate(context& con) {
	if (value.type == lexer::token_type::identifier) {
		std::map<std::string, context::info>::const_iterator itr = find_var(con, value.raw);
		if (itr == con.var_table.end()) {
			std::cout << "runtime error (" << value.point.line << ", " << value.point.col << "): undefined method(" << value.raw << ")" << std::endl;
			abort();
		}
		con.stack.push_back(itr->second.value);
		if (itr->second.value.type() == typeid(float)) {
			return static_cast<int>(std::any_cast<float>(itr->second.value));
		}
		return std::any_cast<int>(itr->second.value);
	}
	else {
		con.return_code = std::atoi(value.raw.c_str());
		if (value.raw.find('.') != std::string::npos) {
			con.stack.push_back(static_cast<float>(std::stod(value.raw.c_str())));
		} else {
			con.stack.push_back(con.return_code);
		}
		return con.return_code;
	}
	return con.return_code;
}

int ast_node_bin::evaluate(context& con) {
	con.return_code = lhs->evaluate(con);
	con.return_code = rhs->evaluate(con);
	std::any rhs_value = con.stack.back(); con.stack.pop_back();
	std::any lhs_value = con.stack.back(); con.stack.pop_back();

	if (op == "=") {
		if (is_a<ast_node_value>(lhs.get())) {
			ast_node_value* value = static_cast<ast_node_value*>(lhs.get());
			if (value->value.type != lexer::token_type::identifier) {
				std::cout << "runtime error (" << value->point.line << ", " << value->point.col << "): lhs should be referencer" << std::endl;
				abort();
			}
			std::map<std::string, context::info>::iterator itr = find_var(con, value->value.raw);
			if (itr == con.var_table.end()) {
				std::cout << "runtime error (" << value->point.line << ", " << value->point.col << "): not found method(" << value->value.raw << ")" << std::endl;
				abort();
			}
			if (itr->second.modifier == lexer::token_type::_const) {
				std::cout << "runtime error (" << value->point.line << ", " << value->point.col << "): not constant value(" << value->value.raw << ")" << std::endl;
				abort();
			}
			if (itr->second.value.type() != rhs_value.type()) {
				std::cout << "runtime error (" << value->point.line << ", " << value->point.col << "): assign different type(`" << itr->second.value.type().name() << "` != `" << rhs_value.type().name() << "`)" << std::endl;
				abort();
			}
			itr->second.value = std::move(rhs_value);
		}
		return con.return_code;
	}

	if (lhs_value.type() != rhs_value.type()) {
		std::cout << "runtime error (" << lhs->point.line << "," << lhs->point.col << "): assign different type(`" << lhs_value.type().name() << "` " << op << " `" << rhs_value.type().name() << "`)" << std::endl;
		abort();
	} 

	if (lhs_value.type() == typeid(float)) {
		float result = 0.f;
		switch (op[0]) {
		case '+': result = std::any_cast<float>(lhs_value) + std::any_cast<float>(rhs_value); break;
		case '-': result = std::any_cast<float>(lhs_value) - std::any_cast<float>(rhs_value); break;
		case '*': result = std::any_cast<float>(lhs_value) * std::any_cast<float>(rhs_value); break;
		case '/': result = std::any_cast<float>(lhs_value) / std::any_cast<float>(rhs_value); break;
		}
		con.return_code = static_cast<int>(result);
		con.stack.push_back(result);
	} else if (lhs_value.type() == typeid(int)) {
		switch (op[0]) {
		case '+': con.return_code = std::any_cast<int>(lhs_value) + std::any_cast<int>(rhs_value); break;
		case '-': con.return_code = std::any_cast<int>(lhs_value) - std::any_cast<int>(rhs_value); break;
		case '*': con.return_code = std::any_cast<int>(lhs_value) * std::any_cast<int>(rhs_value); break;
		case '/': con.return_code = std::any_cast<int>(lhs_value) / std::any_cast<int>(rhs_value); break;
		}
		con.stack.push_back(con.return_code);
	}

	return con.return_code;
}
int ast_node_expr::evaluate(context& con) {
	con.return_code = expr->evaluate(con);
	return con.return_code;
}
int ast_node_return::evaluate(context& con) {
	if (value) {
		con.return_code = value->evaluate(con);
	}
	con.is_abort = true;
	return con.return_code;
}
int ast_node_block::evaluate(context& con) {
	con.name_space.push_back(block_name);
	for (const std::unique_ptr<ast_node_base>& node : exprs) {
		con.return_code = node->evaluate(con);
	}
	con.name_space.pop_back();
	return con.return_code;
}
int ast_node_var_definition::evaluate(context& con) {
	std::string encoded_name = encode(con, name);
	if (con.var_table.find(encoded_name) != con.var_table.end()) {
		std::cout << "runtime error (" << point.line << ", " << point.col << "): variable double definition (" << name << ")" << std::endl;
		abort();
	}
	std::any value = 0;
	if (init_value) {
		con.return_code = init_value->evaluate(con);
		if (con.stack.back().type() == typeid(int) && type != lexer::token_type::_int) {
			std::cout << "runtime error (" << init_value->point.line << ", " << init_value->point.col << "): initial value is not float (" << name << ")" << std::endl;
			abort();
		} else if (con.stack.back().type() == typeid(float) && type != lexer::token_type::_float) {
			std::cout << "runtime error (" << init_value->point.line << ", " << init_value->point.col << "): initial value is not int(" << name << ")" << std::endl;
			abort();
		}
		con.var_table.insert({ encoded_name, context::info { .modifier = modifier, .type = type, .value = con.stack.back() }});
		con.stack.pop_back();
		return con.return_code;
	}
	switch (type) {
	case lexer::token_type::_int: value = 0; break;
	case lexer::token_type::_float: value = 0.f; break;
	}
	con.var_table.insert({ encoded_name, context::info { .modifier = modifier, .type = type, .value = value }});
	return con.return_code;
}

int ast_node_if::evaluate(context& con) {
	con.return_code = condition_block->evaluate(con);
	if (con.return_code) {
		con.return_code = true_block->evaluate(con);
	} else if (false_block) {
		con.return_code = false_block->evaluate(con);
	}
	return con.return_code;
}

int ast_node_program::evaluate(context& con) {
	for (const std::unique_ptr<ast_node_base>& item : exprs) {
		con.return_code = item->evaluate(con);
		if (con.is_abort) {
			break;
		}
	}
	return con.return_code;
}
