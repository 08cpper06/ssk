#pragma once
#include "context.hpp"


class ast_node_base;

class ast_evaluator {
public:
	static std::string encode(const context& con, const std::string& name);
	static std::map<std::string, context::info>::iterator find_var(context& con, const std::string name);
public:
	virtual std::optional<invalid_state> evaluate(context& con) { return con.return_code; }
};
