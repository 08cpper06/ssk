#include "runtime.hpp"
#include <iostream>



int runtime::evaluate(const std::unique_ptr<ast_node_base>& node, context& con) {
	if (ast_node_return* _return = dynamic_cast<ast_node_return*>(node.get())) {
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
		}
	} else if (ast_node_value* value = dynamic_cast<ast_node_value*>(node.get())) {
		return std::atoi(value->value.raw.c_str());
	} else {
		std::cout << "runtime error" << std::endl;
		abort();
	}
	return con.return_code;

}

int runtime::evaluate(const std::unique_ptr<ast_node_base>& node) {
	context con { .return_code = 0, .is_abort = false };
	return evaluate(node, con);
}

