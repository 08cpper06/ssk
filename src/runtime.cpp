#include "runtime.hpp"
#include <iostream>
#include "state.hpp"


OBJECT runtime::evaluate(const std::unique_ptr<ast_node_base>& node, context& con) {
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

