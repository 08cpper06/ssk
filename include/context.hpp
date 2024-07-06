#pragma once

#include "lexer.hpp"
#include "object.hpp"
#include <list>
#include <map>


struct context {

	struct var_info {
		lexer::token_type modifier;
		lexer::token_type type;
		OBJECT value;
	};
	struct func_info {
		std::map<std::string, var_info> var_table;
		lexer::token_type type;
	};

	std::optional<invalid_state> return_code;
	int is_abort { 0 };
	std::map<std::string, var_info> var_table;
	std::map<std::string, func_info> func_table;
	std::list<std::string> name_space;
	std::list<OBJECT> stack;
};
