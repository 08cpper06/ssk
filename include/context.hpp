#pragma once

#include "lexer.hpp"
#include "object.hpp"
#include <list>
#include <map>


struct context {

	struct info {
		lexer::token_type modifier;
		lexer::token_type type;
		OBJECT value;
	};
	std::optional<invalid_state> return_code;
	int is_abort { 0 };
	std::map<std::string, info> var_table;
	std::list<std::string> name_space;
	std::list<OBJECT> stack;
};
