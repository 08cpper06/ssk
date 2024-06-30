#pragma once
#include "parser.hpp"
#include <map>
#include <any>
#include <queue>


class runtime {
public:
	struct context {
		struct info {
			lexer::token_type modifier;
			lexer::token_type type;
			std::any value;
		};
		int return_code;
		int is_abort;
		std::map<std::string, info> var_table;
		std::queue<std::any> stack;
	};
public:
	static int evaluate(const std::unique_ptr<ast_node_base>& node, context& con);
public:
	static int evaluate(const std::unique_ptr<ast_node_base>& node);
};