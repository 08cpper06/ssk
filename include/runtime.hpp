#pragma once
#include "parser.hpp"
#include "context.hpp"


class runtime {
public:
	static OBJECT evaluate(const std::unique_ptr<ast_node_base>& node, context& con);
	static OBJECT evaluate(const std::unique_ptr<ast_node_base>& node);
};