#pragma once

#include "lexer.hpp"
#include <any>
#include <list>
#include <map>


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
	std::list<std::any> stack;
};
