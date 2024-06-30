#pragma once
#include "parser.hpp"
#include <map>
#include <any>
#include <queue>
#include <list>


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
		std::list<std::string> name_space;
		std::queue<std::any> stack;
	};
private:
	static std::string encode(const context& con, const std::string& name);
	static std::map<std::string, context::info>::iterator find_var(context& con, const std::string name);
public:
	static int evaluate(const std::unique_ptr<ast_node_base>& node, context& con);
	static int evaluate(const std::unique_ptr<ast_node_base>& node);
};