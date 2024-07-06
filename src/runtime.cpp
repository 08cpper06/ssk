#include "runtime.hpp"
#include <iostream>
#include "state.hpp"


OBJECT runtime::evaluate(const std::unique_ptr<ast_node_base>& node, context& con) {
	for (std::unique_ptr<ast_node_base>& node : con.pre_evaluate) {
		con.return_code = node->evaluate(con);
	}
	if (node) {
		if (std::optional<invalid_state> state = node->evaluate(con)) {
			return state.value();
		} else {
			OBJECT return_value = con.stack.back();
			con.stack.pop_back();
			return return_value;
		}
	}
	return invalid_state {};
}

OBJECT runtime::evaluate(const std::unique_ptr<ast_node_base>& node) {
	context con { .return_code = std::nullopt, .is_abort = false };
	if (std::optional<invalid_state> state = node->evaluate(con)) {
		return state.value();
	} else {
		OBJECT return_value = con.stack.back();
		con.stack.pop_back();
		return return_value;
	}
}

void runtime::evaluate_pre_process(context& con) {
	for (std::unique_ptr<ast_node_base>& node : con.pre_evaluate) {
		con.return_code = node->evaluate(con);
	}
}
