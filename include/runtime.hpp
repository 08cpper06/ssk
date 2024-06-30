#pragma once
#include "parser.hpp"
#include "context.hpp"


class runtime {
public:
	static int evaluate(const std::unique_ptr<ast_node_base>& node, context& con);
	static int evaluate(const std::unique_ptr<ast_node_base>& node);
};