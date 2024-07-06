#pragma once

#include <memory>
#include "lexer.hpp"
#include "evaluator.hpp"


template <class Type, class UType>
bool is_a(UType target) {
	return target->get_tag() == &Type::tag;
}

class ast_node_base : public ast_evaluator {
public:
	struct ast_base_tag {};
	inline static constexpr ast_base_tag tag;
public:
	ast_node_base() = default;
	virtual ~ast_node_base() = default;

	virtual const ast_base_tag* get_tag() const { return &ast_node_base::tag; }

	virtual std::string log(std::string indent) {
		return "";
	}
	virtual std::optional<invalid_state> evaluate(context& con) { return con.return_code; }

	code_point point { 0, 0 };
};

class ast_node_error : public ast_node_base {
public:
	struct ast_error_tag : public ast_base_tag {};
	inline static constexpr ast_error_tag tag;
public:
	ast_node_error(const std::string& text, code_point point) :
		text(text)
	{
		this->point = point;
	}
	virtual ~ast_node_error() = default;

	virtual const ast_base_tag* get_tag() const { return &ast_node_error::tag; }
	virtual std::string log(std::string indent) {
		return indent + "<error>" + text + "</error>\n";
	}
	virtual std::optional<invalid_state> evaluate(context& con);
	std::string text;
};

class ast_node_value : public ast_node_base {
public:
	struct ast_value_tag : public ast_base_tag {};
	inline static constexpr ast_value_tag tag;
public:
	ast_node_value(const lexer::token& value, code_point point) :
		value(value)
	{
		this->point = point;
	}
	virtual ~ast_node_value() = default;

	virtual const ast_base_tag* get_tag() const { return &ast_node_value::tag; }
	virtual std::string log(std::string indent) {
		return indent + "<value>" + value.raw + "</value>\n";
	}
	virtual std::optional<invalid_state> evaluate(context& con);
	lexer::token value;
};

class ast_node_bin : public ast_node_base {
public:
	struct ast_bin_tag : public ast_base_tag {};
	inline static constexpr ast_bin_tag tag;
public:
	ast_node_bin() = default;
	ast_node_bin(const std::string& op, std::unique_ptr<ast_node_base>&& lhs, std::unique_ptr<ast_node_base>&& rhs, code_point point) :
		op(op),
		lhs(std::move(lhs)),
		rhs(std::move(rhs))
	{
		this->point = point;
	}
	virtual ~ast_node_bin() = default;

	virtual const ast_base_tag* get_tag() const { return &ast_node_bin::tag; }
	virtual std::string log(std::string indent) {
		std::string ret = indent + "<bin op=\"" + op + "\">\n";
		if (lhs) {
			ret += lhs->log(indent + '\t');
		} else {
			ret += indent + "\t<value>error</value>\n";
		}
		if (rhs) {
			ret += rhs->log(indent + '\t');
		} else {
			ret += indent + "\t<value>error</value>\n";
		}
		return ret + indent + "</bin>\n";
	}
	virtual std::optional<invalid_state> evaluate(context& con);
	std::string op;
	std::unique_ptr<ast_node_base> lhs;
	std::unique_ptr<ast_node_base> rhs;
};

class ast_node_expr : public ast_node_base {
public:
	struct ast_expr_tag : public ast_base_tag {};
	inline static constexpr ast_expr_tag tag;
public:
	ast_node_expr(std::unique_ptr<ast_node_base>&& expr, code_point point) :
		expr(std::move(expr))
	{
		this->point = point;
	}
	virtual ~ast_node_expr() = default;

	virtual const ast_base_tag* get_tag() const { return &ast_node_expr::tag; }
	virtual std::string log(std::string indent) {
		if (expr) {
			return expr->log(indent);
		}
		return indent + "<expr>error</expr>\n";
	}
	virtual std::optional<invalid_state> evaluate(context& con);
	std::unique_ptr<ast_node_base> expr;
};

class ast_node_return : public ast_node_base {
public:
	struct ast_return_tag : public ast_base_tag {};
	inline static constexpr ast_return_tag tag;
public:
	ast_node_return(std::unique_ptr<ast_node_base>&& value, code_point point) :
		value(std::move(value))
	{
		this->point = point;
	}
	virtual ~ast_node_return() = default;

	virtual const ast_base_tag* get_tag() const { return &ast_node_return::tag; }
	virtual std::string log(std::string indent) {
		std::string ret = indent + "<return>\n";
		if (value) {
			ret += value->log(indent + '\t');
			return ret + indent + "</return>\n";
		}
		return indent + "<expr>error</expr>\n";
	}
	virtual std::optional<invalid_state> evaluate(context& con);
	std::unique_ptr<ast_node_base> value;
};

class ast_node_function : public ast_node_base {
public:
	struct var_type {
		lexer::token_type modifier;
		lexer::token_type type;
		std::string name;
	};
public:
	struct ast_function_tag : public ast_base_tag {};
	inline static constexpr ast_function_tag tag;

public:
	ast_node_function() = default;
	virtual ~ast_node_function() = default;

	virtual const ast_base_tag* get_tag() const { return &ast_node_function::tag; }
	virtual std::string log(std::string indent) {
		std::string ret = indent + "<function name=" + function_name + ">\n";
		ret += indent + "\t<return type=\"";
		switch (return_type) {
		case lexer::token_type::_int: ret += "int"; break;
		case lexer::token_type::_float: ret += "float"; break;
		case lexer::token_type::_bool: ret += "bool"; break;
		}
		ret += "\"></return>\n";
		for (const var_type& arg : arguments) {
			ret += indent + "\t<argument type=\"";
			switch (arg.modifier) {
			case lexer::token_type::_mut: ret += "mut"; break;
			case lexer::token_type::_const: ret += "const"; break;
			}
			switch (arg.type) {
			case lexer::token_type::_int: ret += " int"; break;
			case lexer::token_type::_float: ret += " float"; break;
			case lexer::token_type::_bool: ret += " bool"; break;
			}
			ret += "\">" + arg.name + "</argument>\n";
		}
		if (block) {
			ret += block->log(indent + "\t");
		}
		ret += indent + "</function>\n";
		return ret;
	}
	virtual std::optional<invalid_state> evaluate(context& con);
	std::unique_ptr<ast_node_base> block;
	std::string function_name;

	std::vector<var_type> arguments;

	lexer::token_type return_type;
};

class ast_node_block : public ast_node_base {
public:
	struct ast_return_tag : public ast_base_tag {};
	inline static constexpr ast_return_tag tag;

public:
	static std::string generate_blockname() {
		static int i = 0;
		return "block_" + std::to_string(i++);
	}
public:
	ast_node_block(std::vector<std::unique_ptr<ast_node_base>>&& exprs, code_point point) :
		exprs(std::move(exprs)),
		block_name(generate_blockname())
	{
		this->point = point;
	}
	ast_node_block(std::vector<std::unique_ptr<ast_node_base>>&& exprs, const std::string& block_name, code_point point) :
		exprs(std::move(exprs)),
		block_name(block_name)
	{
		this->point = point;
	}
	virtual ~ast_node_block() = default;

	virtual const ast_base_tag* get_tag() const { return &ast_node_block::tag; }
	virtual std::string log(std::string indent) {
		std::string ret = indent + "<" + block_name + ">\n";
		for (const std::unique_ptr<ast_node_base>& item : exprs) {
			ret += item->log(indent + '\t');
		}
		return ret + indent + "</" + block_name + ">\n";
	}
	virtual std::optional<invalid_state> evaluate(context& con);
	std::vector<std::unique_ptr<ast_node_base>> exprs;
	std::string block_name;
};

class ast_node_var_definition : public ast_node_base {
public:
	struct ast_var_definition_tag : public ast_base_tag {};
	inline static constexpr ast_var_definition_tag tag;
public:
	ast_node_var_definition(lexer::token_type modifier, std::string name, lexer::token_type type, code_point point) :
		modifier(modifier),
		name(name),
		type(type)
	{
		this->point = point;
	}
	ast_node_var_definition(lexer::token_type modifier, std::string name, lexer::token_type type, std::unique_ptr<ast_node_base>&& init_value, code_point point) :
		modifier(modifier),
		name(name),
		type(type),
		init_value(std::move(init_value))
	{
		this->point = point;
	}
	virtual ~ast_node_var_definition() = default;

	virtual const ast_base_tag* get_tag() const { return &ast_node_var_definition::tag; }
	virtual std::string log(std::string indent) {
		std::string ret = "";
		ret += indent + "<define type=\"";
		switch (modifier) {
		case lexer::token_type::_mut: ret += "mut"; break;
		case lexer::token_type::_const: ret += "const"; break;
		}
		switch (type) {
		case lexer::token_type::_int: ret += " int"; break;
		case lexer::token_type::_float: ret += " float"; break;
		}
		ret += "\">\n";
		ret += indent + "\t<name>" + name + "</name>\n";
		if (init_value) {
			ret += indent + "\t<init>\n";
			ret += init_value->log(indent + "\t\t");
			ret += indent + "\t</init>\n";
		}
		ret += indent + "</define>\n";
		return ret;
	}
	virtual std::optional<invalid_state> evaluate(context& con);
	lexer::token_type modifier;
	std::string name;
	lexer::token_type type;
	std::unique_ptr<ast_node_base> init_value;
};

class ast_node_if : public ast_node_base {
public:
	struct ast_if_tag : public ast_base_tag {};
	inline static constexpr ast_if_tag tag;
public:
	ast_node_if(std::unique_ptr<ast_node_base>&& condition_block, std::unique_ptr<ast_node_base>&& true_block) :
		condition_block(std::move(condition_block)),
		true_block(std::move(true_block)),
		false_block(nullptr)
	{}
	ast_node_if(std::unique_ptr<ast_node_base>&& condition_block, std::unique_ptr<ast_node_base>&& true_block, std::unique_ptr<ast_node_base>&& false_block) :
		condition_block(std::move(condition_block)),
		true_block(std::move(true_block)),
		false_block(std::move(false_block))
	{}
	~ast_node_if() = default;

	virtual const ast_base_tag* get_tag() const { return &ast_node_if::tag; }
	virtual std::string log(std::string indent) {
		std::string ret = "";
		ret += indent + "<if>\n";
		ret += indent + "\t<condition>\n";
		ret += condition_block->log(indent + "\t\t");
		ret += indent + "\t</condition>\n";
		if (true_block) {
			ret += true_block->log(indent + "\t");
		}
		else {
			ret += indent + "\t<expr>error</expr>\n";
		}
		if (false_block) {
			ret += false_block->log(indent + "\t");
		}
		ret += indent + "</if>\n";
		return ret;
	}
	virtual std::optional<invalid_state> evaluate(context& con);

	std::unique_ptr<ast_node_base> condition_block;
	std::unique_ptr<ast_node_base> true_block;
	std::unique_ptr<ast_node_base> false_block;
};

class ast_node_program : public ast_node_base {
public:
	struct ast_expr_tag : public ast_base_tag {};
	inline static constexpr ast_expr_tag tag;
public:
	ast_node_program(std::vector<std::unique_ptr<ast_node_base>>&& exprs, code_point point) :
		exprs(std::move(exprs))
	{
		this->point = point;
	}
	~ast_node_program() = default;

	virtual const ast_base_tag* get_tag() const { return &ast_node_expr::tag; }
	virtual std::string log(std::string indent) {
		std::string ret = "";
		ret += indent + "<program>\n";
		if (exprs.size()) {
			for (const std::unique_ptr<ast_node_base>& item : exprs) {
				ret += item->log(indent + "\t");
			}
		} else {
			ret += indent + "\t<expr>error</expr>\n";
		}
		ret += indent + "</program>\n";
		return ret;
	}
	virtual std::optional<invalid_state> evaluate(context& con);
	std::vector<std::unique_ptr<ast_node_base>> exprs;
};

class parser {
public:
	static std::unique_ptr<ast_node_base> try_build_value(std::vector<lexer::token>::const_iterator& itr);
	static std::unique_ptr<ast_node_base> try_build_timedivide_node(std::vector<lexer::token>::const_iterator& itr);
	static std::unique_ptr<ast_node_base> try_build_plusminus_node(std::vector<lexer::token>::const_iterator& itr);
	static std::unique_ptr<ast_node_base> try_build_equality(std::vector<lexer::token>::const_iterator& itr);
	static std::unique_ptr<ast_node_base> try_build_relational(std::vector<lexer::token>::const_iterator& itr);
	static std::unique_ptr<ast_node_base> try_build_expr(std::vector<lexer::token>::const_iterator& itr);
	static std::unique_ptr<ast_node_base> try_build_return(std::vector<lexer::token>::const_iterator& itr);
	static std::unique_ptr<ast_node_base> try_build_var_definition(std::vector<lexer::token>::const_iterator& itr);
	static std::unique_ptr<ast_node_base> try_build_assign(std::vector<lexer::token>::const_iterator& itr);
	static std::unique_ptr<ast_node_base> try_build_if(std::vector<lexer::token>::const_iterator& itr);
	static std::unique_ptr<ast_node_base> try_build_block(std::vector<lexer::token>::const_iterator& itr);
	static std::unique_ptr<ast_node_base> try_build_function(std::vector<lexer::token>::const_iterator& itr);
	static bool try_skip_comment(std::vector<lexer::token>::const_iterator& itr);
	static std::unique_ptr<ast_node_base> try_build_program(std::vector<lexer::token>::const_iterator& itr);
public:
	static std::unique_ptr<ast_node_base> parse(const std::vector<lexer::token>& toks) noexcept;
};