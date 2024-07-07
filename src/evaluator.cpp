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
	std::vector<std::string> candiate;
	for (const std::string& item : con.name_space) {
		_namespace += item + ".";
		candiate.push_back(_namespace + name);
	}

	for (int i = candiate.size() - 1; i >= 0; --i) {
		tmp = con.var_table.find(candiate[i]);
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
	} else {
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

std::optional<invalid_state> ast_node_call_function::evaluate(context& con) {
	std::map<std::string, context::func_info>::iterator itr = con.func_table.find(function_name);
	if (itr == con.func_table.end()) {
		std::cout << "runtime error (" << point.line << ", " << point.col << "): not found method(" << function_name << ")" << std::endl;
		abort();
	}
	if (!(itr->second.block)) {
		std::cout << "runtime error (" << point.line << ", " << point.col << "): not found implement (" << function_name << ")" << std::endl;
		abort();
	}

	if (itr->second.arguments.size() != arguments.size()) {
		std::cout << "runtime error (" << point.line << ", " << point.col << "): the count of arguments is mismatch (" << function_name << ")" << std::endl;
		abort();
	}

	int idx = 0;
	for (const context::func_info::arg_info& info: itr->second.arguments) {
		con.return_code = arguments[idx]->evaluate(con);
		++idx;
		OBJECT value = con.stack.back();
		con.stack.pop_back();
		con.var_table.insert({ encode(con, function_name) + "." + info.name, context::var_info {.modifier = info.modifier, .type = info.type, .value = value}});
	}
	con.return_code = itr->second.block->evaluate(con);
	con.is_abort = false;

	if (con.stack.back().index() == bool_index && itr->second.type != lexer::token_type::_bool) {
		std::cout << "runtime error (" << point.line << ", " << point.col << "): expected bool value as return value (" << std::visit(get_object_type_name {}, con.stack.back()) << ")" << std::endl;
		abort();
	} else if (con.stack.back().index() == int_index && itr->second.type != lexer::token_type::_int) {
		std::cout << "runtime error (" << point.line << ", " << point.col << "): expected int value as return value (" << std::visit(get_object_type_name {}, con.stack.back()) << ")" << std::endl;
		abort();
	} else if (con.stack.back().index() == float_index && itr->second.type != lexer::token_type::_float) {
		std::cout << "runtime error (" << point.line << ", " << point.col << "): expected float value as return value (" << std::visit(get_object_type_name {}, con.stack.back()) << ")" << std::endl;
		abort();
	}

	for (const context::func_info::arg_info& info : itr->second.arguments) {
		con.var_table.erase(encode(con, function_name) + "." + info.name);
	}
	return con.return_code;
}

std::optional<invalid_state> ast_node_array_refernce::evaluate(context& con) {
	if (!index) {
		std::cout << "runtime error (" << point.line << ", " << point.col << "): not found index" << std::endl;
		abort();
	}
	con.return_code = index->evaluate(con);
	OBJECT obj_index = con.stack.back();
	con.stack.pop_back();
	if (obj_index.index() != int_index) {
		std::cout << "runtime error (" << point.line << ", " << point.col << "): index is invalid" << std::endl;
		abort();
	}
	std::map<std::string, context::var_info>::iterator itr = find_var(con, name.raw);
	if (itr == con.var_table.end()) {
		std::cout << "runtime error (" << point.line << ", " << point.col << "): undefined method(" << name.raw << ")" << std::endl;
		abort();
	}

	OBJECT value = std::visit(operate_index_ref_object(std::get<int>(obj_index)), itr->second.value);
	con.stack.push_back(std::move(value));
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
		} else if (is_a<ast_node_array_refernce>(lhs.get())) {
			ast_node_array_refernce* reference = static_cast<ast_node_array_refernce*>(lhs.get());
			std::map<std::string, context::var_info>::iterator itr = find_var(con, reference->name.raw);
			if (itr == con.var_table.end()) {
				std::cout << "runtime error (" << reference->point.line << ", " << reference->point.col << "): not found method(" << reference->name.raw << ")" << std::endl;
				abort();
			}
			if (itr->second.modifier == lexer::token_type::_const) {
				std::cout << "runtime error (" << reference->point.line << ", " << reference->point.col << "): not constant value(" << reference->name.raw << ")" << std::endl;
				abort();
			}
			if (itr->second.value.index() == float_array_index &&
				rhs_value.index() != float_index) {
				std::cout << "runtime error (" << reference->point.line << ", " << reference->point.col << "): assign different type(`float array` != `" << std::visit(get_object_type_name{}, rhs_value) << "`)" << std::endl;
				abort();
			} else if (itr->second.value.index() == float_array_index &&
				rhs_value.index() != int_index) {
				std::cout << "runtime error (" << reference->point.line << ", " << reference->point.col << "): assign different type(`int array" << std::visit(get_object_type_name{}, itr->second.value) << "` != `" << std::visit(get_object_type_name{}, rhs_value) << "`)" << std::endl;
				abort();
			} else if (itr->second.value.index() == bool_array_index &&
				rhs_value.index() != bool_index) {
				std::cout << "runtime error (" << reference->point.line << ", " << reference->point.col << "): assign different type(`bool array" << std::visit(get_object_type_name{}, itr->second.value) << "` != `" << std::visit(get_object_type_name{}, rhs_value) << "`)" << std::endl;
				abort();
			}
			reference->index->evaluate(con);
			OBJECT index = con.stack.back();
			con.stack.pop_back();
			if (index.index() != int_index) {
				std::cout << "runtime error (" << reference->point.line << ", " << reference->point.col << "): assign different type(`" << std::visit(get_object_type_name{}, itr->second.value) << "` != `" << std::visit(get_object_type_name{}, rhs_value) << "`)" << std::endl;
				abort();
			}

			OBJECT result = std::visit(operate_assign_object(std::get<int>(index)), itr->second.value, rhs_value);
			if (result.index() == state_index) {
				std::cout << "runtime error (" << reference->point.line << ", " << reference->point.col << "): out of range (" << std::get<int>(index) << ")" << std::endl;
				abort();
			}

		}
		return con.return_code;
	}

	if (lhs_value.index() != rhs_value.index()) {
		std::cout << "runtime error (" << lhs->point.line << "," << lhs->point.col << "): assign different type(`" << std::visit(get_object_type_name {}, lhs_value) << "` " << op << " `" << std::visit(get_object_type_name {}, rhs_value) << "`)" << std::endl;
		abort();
	} 

	OBJECT result = invalid_state("no result");
	if (op == "+") {
		result = std::visit(operate_add_object(-1, -1), lhs_value, rhs_value);
	} else if (op == "-") {
		result = std::visit(operate_sub_object(-1, -1), lhs_value, rhs_value);
	} else if (op == "*") {
		result = std::visit(operate_mul_object(-1, -1), lhs_value, rhs_value);
	} else if (op == "/") {
		result = std::visit(operate_div_object(-1, -1), lhs_value, rhs_value);
		if (result.index() == state_index) {
			std::cout << "runtime error (" << point.line << ", " << point.col << "): divide by zero" << std::endl;
			abort();
		}
	} else if (op == "==") {
		result = std::visit(operate_equal_object (-1, -1), lhs_value, rhs_value);
	} else if (op == "!=") {
		result = std::visit(operate_not_object (-1, -1), lhs_value, rhs_value);
	} else if (op == "<") {
		result = std::visit(operate_less_than_object(-1, -1), lhs_value, rhs_value);
	} else if (op == ">") {
		result = std::visit(operate_greater_than_object(-1, -1), lhs_value, rhs_value);
	} else if (op == "<=") {
		result = std::visit(operate_less_than_or_equal_object(-1, -1), lhs_value, rhs_value);
	} else if (op == ">=") {
		result = std::visit(operate_greater_than_or_equal_object(-1, -1), lhs_value, rhs_value);
	}

	con.stack.push_back(std::move(result));
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
	for (const ast_node_function::var_type& arg : arguments) {
		info.arguments.push_back(context::func_info::arg_info { .name = arg.name, .modifier = arg.modifier, .type = arg.type });
	}
	info.type = return_type;
	info.block = block.get();
	con.func_table.insert({ function_name, std::move(info) });
	con.return_code = std::nullopt;
	return con.return_code;
}
std::optional<invalid_state> ast_node_block::evaluate(context& con) {
	con.name_space.push_back(block_name);
	for (const std::unique_ptr<ast_node_base>& node : exprs) {
		con.return_code = node->evaluate(con);
		if (con.is_abort) {
			break;
		}
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
	case lexer::token_type::_int: {
		if (size >= 0) {
			value = std::vector<int>(size, 0);
		} else {
			value = 0;
		}
		break;
	}
	case lexer::token_type::_float: {
		if (size >= 0) {
			value = std::vector<float>(size, 0.f);
		} else {
			value = 0.f;
		}
		break;
	}
	case lexer::token_type::_bool: {
		if (size >= 0) {
			value = std::vector<bool>(size, false);
		} else {
			value = false;
		}
		break;
	}
	}
	con.var_table.insert({ encoded_name, context::var_info { .modifier = modifier, .type = type, .value = value }});
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

std::optional<invalid_state> ast_node_while::evaluate(context& con) {
	do {
		con.return_code = condition->evaluate(con);
		OBJECT cond = std::visit(cast_bool_object {}, con.stack.back());
		con.stack.pop_back();
		if (cond.index() != bool_index) {
			return con.return_code;
		}
		if (std::get<bool>(cond)) {
			con.return_code = block->evaluate(con);
		} else {
			break;
		}
	} while (true);
	return con.return_code;
}

std::optional<invalid_state> ast_node_do_while::evaluate(context& con) {
	do {
		con.return_code = block->evaluate(con);

		con.return_code = condition->evaluate(con);
		OBJECT cond = std::visit(cast_bool_object {}, con.stack.back());
		con.stack.pop_back();
		if (cond.index() != bool_index) {
			return con.return_code;
		}
		if (!std::get<bool>(cond)) {
			break;
		}
	} while (true);
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
