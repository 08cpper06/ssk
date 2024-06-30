#include "runtime.hpp"
#include <iostream>



int runtime::evaluate(const std::unique_ptr<ast_node_base>& node, context& con) {
	if (ast_node_error* error = dynamic_cast<ast_node_error*>(node.get())) {
		std::cout << "runtime error: syntax error(" << error->text << ")" << std::endl;
		abort();
	} else if (ast_node_var_definition* var_definition = dynamic_cast<ast_node_var_definition*>(node.get())) {
		if (con.var_table.find(var_definition->name) != con.var_table.end()) {
			std::cout << "runtime error: variable double definition (" << var_definition->name << ")" << std::endl;
			abort();
		}
		int value = 0;
		if (var_definition->init_value) {
			value = evaluate(var_definition->init_value);
		}
		con.var_table.insert({ var_definition->name, context::info { .modifier = var_definition->modifier, .type = var_definition->type, .value = value} });
		return con.return_code;
	} else if (ast_node_return* _return = dynamic_cast<ast_node_return*>(node.get())) {
		con.return_code = evaluate(_return->value, con);
		con.is_abort = true;
	} else if (ast_node_program* program = dynamic_cast<ast_node_program*>(node.get())) {
		for (const std::unique_ptr<ast_node_base>& item : program->exprs) {
			con.return_code = evaluate(item, con);
			if (con.is_abort) {
				break;
			}
		}
	} else if (ast_node_expr* expr = dynamic_cast<ast_node_expr*>(node.get())) {
		con.return_code = evaluate(expr->expr, con);
	} else if (ast_node_bin* bin = dynamic_cast<ast_node_bin*>(node.get())) {
		switch (bin->op[0]) {
		case '+': con.return_code = evaluate(bin->lhs, con) + evaluate(bin->rhs, con); break;
		case '-': con.return_code = evaluate(bin->lhs, con) - evaluate(bin->rhs, con); break;
		case '*': con.return_code = evaluate(bin->lhs, con) * evaluate(bin->rhs, con); break;
		case '/': con.return_code = evaluate(bin->lhs, con) / evaluate(bin->rhs, con); break;
		case '=': {
				if (ast_node_value* value = dynamic_cast<ast_node_value*>(bin->lhs.get())) {
					if (value->value.type != lexer::token_type::identifier) {
						std::cout << "runtime error: lhs should be referencer" << std::endl;
						abort();
					}
					std::map<std::string, runtime::context::info>::iterator itr = con.var_table.find(value->value.raw);
					if (itr == con.var_table.end()) {
						std::cout << "runtime error: not found method (" << value->value.raw << ")" << std::endl;
						abort();
					}
					if (itr->second.modifier == lexer::token_type::_const) {
						std::cout << "runtime error: not constant value (" << value->value.raw << ")" << std::endl;
						abort();
					}
					con.return_code = evaluate(bin->rhs, con);
					itr->second.value = con.return_code;
				}
				break;
			}
		}
	} else if (ast_node_value* value = dynamic_cast<ast_node_value*>(node.get())) {
		if (value->value.type == lexer::token_type::identifier) {
			std::map<std::string, context::info>::const_iterator itr = con.var_table.find(value->value.raw);
			if (itr == con.var_table.end()) {
				std::cout << "runtime error: undefined method (" << value->value.raw << ")" << std::endl;
				abort();
			}
			return std::any_cast<int>(itr->second.value);
		} else {
			return std::atoi(value->value.raw.c_str());
		}
	} else {
		std::cout << "runtime error: unknown expression" << std::endl;
		abort();
	}
	return con.return_code;

}

int runtime::evaluate(const std::unique_ptr<ast_node_base>& node) {
	context con { .return_code = 0, .is_abort = false };
	return evaluate(node, con);
}

