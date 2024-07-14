#pragma once

#include "lexer.hpp"
#include "object.hpp"
#include <list>
#include <map>


class ast_node_base;

struct context {

	enum class var_type {
		_invalid = state_index,
		_bool = bool_index,
		_int = int_index,
		_float = float_index,
		_str = string_index,

		_bool_array = bool_array_index,
		_int_array = int_array_index,
		_float_array = float_array_index,
		_str_array = string_array_index,
	};

	struct var_info {
		lexer::token_type modifier;
		var_type type;
		std::string name;
		OBJECT value;
	};
	struct func_info {
		struct arg_info {
			std::string name;
			lexer::token_type modifier;
			var_type type;
		};
		std::vector<arg_info> arguments;
		var_type type;

		ast_node_base* block;
	};

	static var_type cast_from_token(lexer::token_type type, bool is_array) {
		switch (type) {
		case lexer::token_type::_int:
			if (is_array) {
				return var_type::_int_array;
			}
			return var_type::_int;
		case lexer::token_type::_bool:
			if (is_array) {
				return var_type::_bool_array;
			}
			return var_type::_bool;
		case lexer::token_type::_float:
			if (is_array) {
				return var_type::_float_array;
			}
			return var_type::_float;
		case lexer::token_type::_str:
			if (is_array) {
				return var_type::_str_array;
			}
			return var_type::_str;
		default:
			return var_type::_invalid;
		}
	}

	void abort() {
		::abort();
	}

	std::optional<invalid_state> return_code;
	bool is_abort { false };
	std::map<std::string, var_info> var_table;
	std::map<std::string, func_info> func_table;
	std::list<std::string> name_space;
	std::list<OBJECT> stack;

	std::vector<ast_node_base*> pre_evaluate;
};
