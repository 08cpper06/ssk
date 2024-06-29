#pragma once
#include "parser.hpp"


class runtime {
public:
	static int evaluate(const std::unique_ptr<ast_node_base>& node);
};