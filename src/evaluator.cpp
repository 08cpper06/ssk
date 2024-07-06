#include "parser.hpp"
#include <iostream>


std::string ast_evaluator::encode(const context& con, const std::string& name) {
	std::string encoded_name;
	for (const auto& item : con.name_space) {
		encoded_name += item + ".";
	}
	return encoded_name + name;
}

std::map<std::string, context::var_info>::iterator ast_evaluator::find_var(context& con, const std::string name) {
	std::string _namespace;
	std::map<std::string, context::var_info>::iterator itr = con.var_table.find(name);
	std::map<std::string, context::var_info>::iterator tmp;
	for (const auto& item : con.name_space) {
		_namespace += item + ".";
		tmp = con.var_table.find(_namespace + name);
		if (tmp != con.var_table.end()) {
			itr = tmp;
		}
	}
	return itr;
}

std::map<std::string, context::func_info>::iterator ast_evaluator::find_func(context& con, const std::string name) {
	std::string _namespace;
	std::map<std::string, context::func_info>::iterator itr = con.func_table.find(name);
	std::map<std::string, context::func_info>::iterator tmp;
	for (const auto& item : con.name_space) {
		_namespace += item + ".";
		tmp = con.func_table.find(_namespace + name);
		if (tmp != con.func_table.end()) {
			itr = tmp;
		}
	}
	return itr;
}

std::optional<invalid_state> ast_node_error::evaluate(context& con) {
	std::cout << "runtime error (" << point.line << ", " << point.col << "): syntax error(" << text << ")" << std::endl;
	abort();
}

std::optional<invalid_state> ast_node_value::evaluate(context& con) {
	if (value.type == lexer::token_type::identifier) {
		std::map<std::string, context::var_info>::const_iterator itr = find_var(con, value.raw);
		if (itr == con.var_table.end()) {
			std::cout << "runtime error (" << value.point.line << ", " << value.point.col << "): undefined method(" << value.raw << ")" << std::endl;
			abort();
		}
		con.stack.push_back(itr->second.value);
		return std::visit(get_object_return_code{}, itr->second.value);
	}
	else {
		if (value.type == lexer::token_type::_true) {
			con.stack.push_back(true);
			con.return_code = std::nullopt;
			return con.return_code;
		} else if (value.type == lexer::token_type::_false) {
			con.stack.push_back(false);
			con.return_code = std::nullopt;
			return con.return_code;
		}
		if (value.raw.find('.') != std::string::npos) {
			con.stack.push_back(static_cast<float>(std::stod(value.raw.c_str())));
		} else {
			con.stack.push_back(std::atoi(value.raw.c_str()));
		}
		con.return_code = std::nullopt;
		return con.return_code;
	}
	return con.return_code;
}

std::optional<invalid_state> ast_node_bin::evaluate(context& con) {
	con.return_code = lhs->evaluate(con);
	con.return_code = rhs->evaluate(con);
	OBJECT rhs_value = con.stack.back(); con.stack.pop_back();
	OBJECT lhs_value = con.stack.back(); con.stack.pop_back();

	if (op == "=") {
		if (is_a<ast_node_value>(lhs.get())) {
			ast_node_value* value = static_cast<ast_node_value*>(lhs.get());
			if (value->value.type != lexer::token_type::identifier) {
				std::cout << "runtime error (" << value->point.line << ", " << value->point.col << "): lhs should be referencer" << std::endl;
				abort();
			}
			std::map<std::string, context::var_info>::iterator itr = find_var(con, value->value.raw);
			if (itr == con.var_table.end()) {
				std::cout << "runtime error (" << value->point.line << ", " << value->point.col << "): not found method(" << value->value.raw << ")" << std::endl;
				abort();
			}
			if (itr->second.modifier == lexer::token_type::_const) {
				std::cout << "runtime error (" << value->point.line << ", " << value->point.col << "): not constant value(" << value->value.raw << ")" << std::endl;
				abort();
			}
			if (itr->second.value.index() != rhs_value.index()) {
				std::cout << "runtime error (" << value->point.line << ", " << value->point.col << "): assign different type(`" << std::visit(get_object_type_name {}, itr->second.value) << "` != `" << std::visit(get_object_type_name {}, rhs_value) << "`)" << std::endl;
				abort();
			}
			itr->second.value = std::move(rhs_value);
		}
		return con.return_code;
	}

	if (lhs_value.index() != rhs_value.index()) {
		std::cout << "runtime error (" << lhs->point.line << "," << lhs->point.col << "): assign different type(`" << std::visit(get_object_type_name {}, lhs_value) << "` " << op << " `" << std::visit(get_object_type_name {}, rhs_value) << "`)" << std::endl;
		abort();
	} 

	OBJECT result;
	if (op == "+") {
		result = std::visit(operate_add_object {}, lhs_value, rhs_value);
	} else if (op == "-") {
		result = std::visit(operate_sub_object {}, lhs_value, rhs_value);
	} else if (op == "*") {
		result = std::visit(operate_mul_object {}, lhs_value, rhs_value);
	} else if (op == "/") {
		result = std::visit(operate_div_object {}, lhs_value, rhs_value);
		if (result.index() == state_index) {
			std::cout << "divide by zero" << std::endl;
			abort();
		}
	} else if (op == "==") {
		result = std::visit(operate_equal_object {}, lhs_value, rhs_value);
	} else if (op == "!=") {
		result = std::visit(operate_not_object {}, lhs_value, rhs_value);
	} else if (op == "<") {
		result = std::visit(operate_less_than_object {}, lhs_value, rhs_value);
	} else if (op == ">") {
		result = std::visit(operate_greater_than_object {}, lhs_value, rhs_value);
	} else if (op == "<=") {
		result = std::visit(operate_less_than_or_equal_object {}, lhs_value, rhs_value);
	} else if (op == ">=") {
		result = std::visit(operate_greater_than_or_equal_object {}, lhs_value, rhs_value);
	}

	con.stack.push_back(result);
	con.return_code = std::visit(get_object_return_code {}, result);

	return con.return_code;
}
std::optional<invalid_state> ast_node_expr::evaluate(context& con) {
	con.return_code = expr->evaluate(con);
	return con.return_code;
}
std::optional<invalid_state> ast_node_return::evaluate(context& con) {
	if (value) {
		con.return_code = value->evaluate(con);
	}
	con.is_abort = true;
	return con.return_code;
}
std::optional<invalid_state> ast_node_function::evaluate(context& con) {
	std::map<std::string, context::func_info>::iterator itr = find_func(con, function_name);
	if (itr != con.func_table.end()) {
		std::cout << "runtime error (" << point.line << ", " << point.col << "): function double definition (" << function_name << ")" << std::endl;
		abort();
	}
	context::func_info info;
	for (const auto& arg : arguments) {
		info.var_table.insert({ arg.name, context::var_info { .modifier = arg.modifier, .type = arg.type } });
	}
	info.type = return_type;
	con.func_table.insert({ function_name, std::move(info) });
	con.return_code = std::nullopt;
	return con.return_code;
}
std::optional<invalid_state> ast_node_block::evaluate(context& con) {
	con.name_space.push_back(block_name);
	for (const std::unique_ptr<ast_node_base>& node : exprs) {
		con.return_code = node->evaluate(con);
	}
	con.name_space.pop_back();
	return con.return_code;
}
std::optional<invalid_state> ast_node_var_definition::evaluate(context& con) {
	std::string encoded_name = encode(con, name);
	if (con.var_table.find(encoded_name) != con.var_table.end()) {
		std::cout << "runtime error (" << point.line << ", " << point.col << "): variable double definition (" << name << ")" << std::endl;
		abort();
	}
	OBJECT value = 0;
	if (init_value) {
		con.return_code = init_value->evaluate(con);
		if (type == lexer::token_type::_bool && con.stack.back().index() != bool_index) {
			std::cout << "runtime error (" << init_value->point.line << ", " << init_value->point.col << "): initial value is not bool (" << name << ")" << std::endl;
			abort();
		} else if (type == lexer::token_type::_int && con.stack.back().index() != int_index) {
			std::cout << "runtime error (" << init_value->point.line << ", " << init_value->point.col << "): initial value is not int (" << name << ")" << std::endl;
			abort();
		} else if (type == lexer::token_type::_float && con.stack.back().index() != float_index) {
			std::cout << "runtime error (" << init_value->point.line << ", " << init_value->point.col << "): initial value is not float (" << name << ")" << std::endl;
			abort();
		}
		con.var_table.insert({ encoded_name, context::var_info { .modifier = modifier, .type = type, .value = con.stack.back() }});
		con.stack.pop_back();
		return con.return_code;
	}
	switch (type) {
	case lexer::token_type::_int: value = 0; break;
	case lexer::token_type::_float: value = 0.f; break;
	case lexer::token_type::_bool: value = false; break;
	}
	con.var_table.insert({ encoded_name, context::var_info { .modifier = modifier, .type = type, .value = value }});
	con.stack.push_back(value);
	return con.return_code;
}

std::optional<invalid_state> ast_node_if::evaluate(context& con) {
	con.return_code = condition_block->evaluate(con);
	OBJECT cond = std::visit(cast_bool_object {}, con.stack.back());
	con.stack.pop_back();
	if (cond.index() != bool_index) {
		return con.return_code;
	}
	if (std::get<bool>(cond)) {
		con.return_code = true_block->evaluate(con);
	} else if (false_block) {
		con.return_code = false_block->evaluate(con);
	}
	return con.return_code;
}

std::optional<invalid_state> ast_node_program::evaluate(context& con) {
	for (const std::unique_ptr<ast_node_base>& item : exprs) {
		con.return_code = item->evaluate(con);
		if (con.is_abort) {
			break;
		}
	}
	return con.return_code;
}
