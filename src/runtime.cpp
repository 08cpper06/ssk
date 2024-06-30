#include "runtime.hpp"
#include <iostream>


int runtime::evaluate(const std::unique_ptr<ast_node_base>& node, context& con) {
	if (node) {
		return node->evaluate(con);
	}
	return con.return_code;
}

int runtime::evaluate(const std::unique_ptr<ast_node_base>& node) {
	context con { .return_code = 0, .is_abort = false };
	return node->evaluate(con);
}

