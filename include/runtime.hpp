#pragma once
#include "parser.hpp"


class runtime {
public:
	static int evaluate(const std::unique_ptr<ast_node_base>& node) {
		int ret = 0;
		if (ast_node_bin* bin = dynamic_cast<ast_node_bin*>(node.get())) {
			switch (bin->op[0]) {
			case '+': ret = evaluate(bin->lhs) + evaluate(bin->rhs); break;
			case '-': ret = evaluate(bin->lhs) - evaluate(bin->rhs); break;
			case '*': ret = evaluate(bin->lhs) * evaluate(bin->rhs); break;
			case '/': ret = evaluate(bin->lhs) / evaluate(bin->rhs); break;
			}
		} else if (ast_node_value* value = dynamic_cast<ast_node_value*>(node.get())) {
			return std::atoi(value->value.raw.c_str());
		} else {
			std::cout << "runtime error" << std::endl;
			abort();
		}
		return ret;
	}
};