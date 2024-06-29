#pragma once
#include "parser.hpp"
#include <map>


class runtime {
public:
	struct context {
		struct info {
			lexer::token_type modifier;
			lexer::token_type type;
			int value;
		};
		int return_code;
		int is_abort;
		std::map<std::string, info> var_table;
	};
public:
	static int evaluate(const std::unique_ptr<ast_node_base>& node, context& con);
public:
	static int evaluate(const std::unique_ptr<ast_node_base>& node);
};