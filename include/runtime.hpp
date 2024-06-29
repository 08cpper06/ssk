#pragma once
#include "parser.hpp"


class runtime {
public:
	struct context {
		int return_code;
		int is_abort;
	};
public:
	static int evaluate(const std::unique_ptr<ast_node_base>& node, context& con);
public:
	static int evaluate(const std::unique_ptr<ast_node_base>& node);
};