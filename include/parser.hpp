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
		return indent + "<error code_point=(" + std::to_string(point.line) + "," + std::to_string(point.col) + ")>" + text + "</error>\n";
	}
	virtual std::optional<invalid_state> evaluate(context& con);
	std::string text;
};

class ast_node_string : public ast_node_base {
public:
	struct ast_string_tag : public ast_base_tag {};
	inline static constexpr ast_string_tag tag;
public:
	ast_node_string(const lexer::token& value, code_point point) :
		value(value)
	{
		this->point = point;
	}
	virtual ~ast_node_string() = default;

	virtual const ast_base_tag* get_tag() const { return &ast_node_string::tag; }
	virtual std::string log(std::string indent) {
		return indent + "<value>" + value.raw + "</value>\n";
	}
	virtual std::optional<invalid_state> evaluate(context& con);
	lexer::token value;
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

class ast_node_call_function : public ast_node_base {
public:
	struct ast_call_function_tag : public ast_base_tag {};
	inline static constexpr ast_call_function_tag tag;
public:
	ast_node_call_function(const std::string& function_name, std::vector<std::unique_ptr<ast_node_base>>&& arguments, code_point point) :
		function_name(function_name),
		arguments(std::move(arguments))
	{
		this->point = point;
	}
	virtual ~ast_node_call_function() = default;

	virtual const ast_base_tag* get_tag() const { return &ast_node_call_function::tag; }
	virtual std::string log(std::string indent) {
		std::string ret = indent + "<call_function name=\"" + function_name + "\">\n";
		ret += indent + "\t<arguments>\n";
		for (const std::unique_ptr<ast_node_base>& arg : arguments) {
			ret += arg->log(indent + "\t\t");
		}
		ret += indent + "\t</arguments>\n";
		ret += indent + "</call_function>\n";
		return ret;
	}
	virtual std::optional<invalid_state> evaluate(context& con);
	std::string function_name;
	std::vector<std::unique_ptr<ast_node_base>> arguments;
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
		} else {
			ret += indent + "\t<error>no expression</error>\n";
		}
		return ret + indent + "</return>\n";
	}
	virtual std::optional<invalid_state> evaluate(context& con);
	std::unique_ptr<ast_node_base> value;
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

class ast_node_function : public ast_node_base {
public:
	struct ast_function_tag : public ast_base_tag {};
	inline static constexpr ast_function_tag tag;

public:
	ast_node_function() :
		block(nullptr),
		function_name(),
		arguments({}),
		return_type(context::var_type::_invalid),
		return_type_size(-1)
	{}
	virtual ~ast_node_function() = default;

	virtual const ast_base_tag* get_tag() const { return &ast_node_function::tag; }
	virtual std::string log(std::string indent) {
		std::string ret = indent + "<function name=" + function_name + ">\n";
		ret += indent + "\t<return type=\"";
		switch (return_type) {
		case context::var_type::_invalid: ret += "invalid"; break;
		case context::var_type::_int: ret += "int"; break;
		case context::var_type::_float: ret += "float"; break;
		case context::var_type::_bool: ret += "bool"; break;
		case context::var_type::_str: ret += "str"; break;
		case context::var_type::_int_array: ret += "int[]"; break;
		case context::var_type::_float_array: ret += "float[]"; break;
		case context::var_type::_bool_array: ret += "bool[]"; break;
		case context::var_type::_str_array: ret += "str[]"; break;
		}
		ret += "\"></return>\n";
		for (const context::var_info& arg : arguments) {
			ret += indent + "\t<argument type=\"";
			switch (arg.modifier) {
			case lexer::token_type::_mut: ret += "mut"; break;
			case lexer::token_type::_const: ret += "const"; break;
			}
			switch (arg.type) {
			case context::var_type::_int: ret += " int"; break;
			case context::var_type::_float: ret += " float"; break;
			case context::var_type::_bool: ret += " bool"; break;
			case context::var_type::_str: ret += " str"; break;
			case context::var_type::_int_array: ret += " int[]"; break;
			case context::var_type::_float_array: ret += " float[]"; break;
			case context::var_type::_bool_array: ret += " bool[]"; break;
			case context::var_type::_str_array: ret += " str[]"; break;
			}
			ret += "\">" + arg.name + "</argument>\n";
		}
		if (block) {
			if (ast_node_block* casted_block = dynamic_cast<ast_node_block*>(block.get())) {
				casted_block->block_name = "implement";
				ret += block->log(indent + "\t");
				casted_block->block_name = function_name;
			}
		}
		ret += indent + "</function>\n";
		return ret;
	}
	virtual std::optional<invalid_state> evaluate(context& con);
	std::unique_ptr<ast_node_base> block;
	std::string function_name;

	std::vector<context::var_info> arguments;

	context::var_type return_type;
	int return_type_size;
};

class ast_node_array_refernce : public ast_node_base {
public:
	struct ast_array_reference_tag : public ast_base_tag {};
	inline static constexpr ast_array_reference_tag tag;
public:
	ast_node_array_refernce(const lexer::token& name, std::unique_ptr<ast_node_base>&& index, code_point point) :
		name(name),
		index(std::move(index))
	{
		this->point = point;
	}

	virtual ~ast_node_array_refernce() = default;

	virtual const ast_base_tag* get_tag() const { return &ast_node_array_refernce::tag; }
	virtual std::string log(std::string indent) {
		std::string ret = indent + "<array-reference>\n";
		ret += indent + "\t<variable>" + name.raw + "</variable>\n";
		ret += indent + "\t<index>\n";
		if (index) {
			ret += index->log(indent + "\t\t");
		}
		ret += indent + "\t</index>\n";
		ret += indent + "</array-reference>\n";
		return ret;
	}
	virtual std::optional<invalid_state> evaluate(context& con);

	lexer::token name;
	std::unique_ptr<ast_node_base> index;
};

class ast_node_var_definition : public ast_node_base {
public:
	struct ast_var_definition_tag : public ast_base_tag {};
	inline static constexpr ast_var_definition_tag tag;
public:
	ast_node_var_definition(lexer::token_type modifier, std::string name, context::var_type type, int size, code_point point) :
		modifier(modifier),
		name(name),
		type(type),
		size(size)
	{
		this->point = point;
	}
	ast_node_var_definition(lexer::token_type modifier, std::string name, context::var_type type, int size, std::unique_ptr<ast_node_base>&& init_value, code_point point) :
		modifier(modifier),
		name(name),
		type(type),
		init_value(std::move(init_value)),
		size(size)
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
		case context::var_type::_int: ret += " int"; break;
		case context::var_type::_float: ret += " float"; break;
		}
		if (size > 0) {
			ret += "[" + std::to_string(size)  + "]";
		} else if (!size) {
			ret += "[]";
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
	context::var_type type;
	std::unique_ptr<ast_node_base> init_value;
	int size;
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

class ast_node_while : public ast_node_base {
public:
	struct ast_while_tag : public ast_base_tag {};
	inline static constexpr ast_while_tag tag;
public:
	ast_node_while(std::unique_ptr<ast_node_base>&& condition, std::unique_ptr<ast_node_base>&& block, code_point point) :
		condition(std::move(condition)),
		block(std::move(block))
	{
		this->point = point;
	}
	~ast_node_while() = default;

	virtual const ast_base_tag* get_tag() const { return &ast_node_while::tag; }
	virtual std::string log(std::string indent) {
		std::string ret = indent + "<while>\n";
		ret += indent + "\t<condition>\n";
		ret += condition->log(indent + "\t");
		ret += indent + "\t</condition>\n";
		ret += block->log(indent + "\t");
		ret += indent + "</while>\n";
		return ret;
	}
	virtual std::optional<invalid_state> evaluate(context& con);

	std::unique_ptr<ast_node_base> condition;
	std::unique_ptr<ast_node_base> block;
};

class ast_node_do_while : public ast_node_base {
public:
	struct ast_do_while_tag : public ast_base_tag {};
	inline static constexpr ast_do_while_tag tag;
public:
	ast_node_do_while(std::unique_ptr<ast_node_base>&& condition, std::unique_ptr<ast_node_base>&& block, code_point point) :
		condition(std::move(condition)),
		block(std::move(block))
	{
		this->point = point;
	}
	~ast_node_do_while() = default;

	virtual const ast_base_tag* get_tag() const { return &ast_node_do_while::tag; }
	virtual std::string log(std::string indent) {
		std::string ret = indent + "<do-while>\n";
		ret += indent + "\t<condition>\n";
		ret += condition->log(indent + "\t");
		ret += indent + "\t</condition>\n";
		ret += block->log(indent + "\t");
		ret += indent + "</do-while>\n";
		return ret;
	}
	virtual std::optional<invalid_state> evaluate(context& con);

	std::unique_ptr<ast_node_base> condition;
	std::unique_ptr<ast_node_base> block;
};

class ast_node_initial_list : public ast_node_base {
public:
	struct ast_initial_list_tag : public ast_base_tag {};
	inline static constexpr ast_initial_list_tag tag;
public:
	ast_node_initial_list(std::vector<std::unique_ptr<ast_node_base>>&& values, code_point point) :
		values(std::move(values))
	{
		this->point = point;
	}
	virtual ~ast_node_initial_list() = default;

	virtual const ast_base_tag* get_tag() const { return &ast_node_initial_list::tag; }
	virtual std::string log(std::string indent) {
		std::string ret = indent + "<initialize>\n";

		for (const std::unique_ptr<ast_node_base>& value : values) {
			ret += indent + "\t<value>\n";
			ret += value->log(indent + "\t\t");
			ret += indent + "\t</value>\n";
		}
		return ret + indent + "</initialize>\n";
	}
	virtual std::optional<invalid_state> evaluate(context& con);

	std::vector<std::unique_ptr<ast_node_base>> values;
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
private:
	static void skip_until_semicolon(std::vector<lexer::token>::const_iterator& itr);
public:
	static std::unique_ptr<ast_node_base> try_build_value(context& con, std::vector<lexer::token>::const_iterator& itr);
	static std::unique_ptr<ast_node_base> try_build_call_function(context& con, std::vector<lexer::token>::const_iterator& itr);
	static std::unique_ptr<ast_node_base> try_build_reference_array(context& con, std::vector<lexer::token>::const_iterator& itr);
	static std::unique_ptr<ast_node_base> try_build_timedivide_node(context& con, std::vector<lexer::token>::const_iterator& itr);
	static std::unique_ptr<ast_node_base> try_build_plusminus_node(context& con, std::vector<lexer::token>::const_iterator& itr);
	static std::unique_ptr<ast_node_base> try_build_equality(context& con, std::vector<lexer::token>::const_iterator& itr);
	static std::unique_ptr<ast_node_base> try_build_relational(context& con, std::vector<lexer::token>::const_iterator& itr);
	static std::unique_ptr<ast_node_base> try_build_expr(context& con, std::vector<lexer::token>::const_iterator& itr);
	static std::unique_ptr<ast_node_base> try_build_return(context& con, std::vector<lexer::token>::const_iterator& itr);
	static std::unique_ptr<ast_node_base> try_build_var_definition(context& con, std::vector<lexer::token>::const_iterator& itr);
	static std::unique_ptr<ast_node_base> try_build_initial_list(context& con, std::vector<lexer::token>::const_iterator& itr);
	static std::unique_ptr<ast_node_base> try_build_assign(context& con, std::vector<lexer::token>::const_iterator& itr);
	static std::unique_ptr<ast_node_base> try_build_if(context& con, std::vector<lexer::token>::const_iterator& itr);
	static std::unique_ptr<ast_node_base> try_build_while(context& con, std::vector<lexer::token>::const_iterator& itr);
	static std::unique_ptr<ast_node_base> try_build_do_while(context& con, std::vector<lexer::token>::const_iterator& itr);
	static std::unique_ptr<ast_node_base> try_build_block(context& con, std::vector<lexer::token>::const_iterator& itr);
	static std::unique_ptr<ast_node_base> try_build_function(context& con, std::vector<lexer::token>::const_iterator& itr);
	static bool try_skip_comment(std::vector<lexer::token>::const_iterator& itr);
	static std::unique_ptr<ast_node_base> try_build_program(context& con, std::vector<lexer::token>::const_iterator& itr);
public:
	static std::unique_ptr<ast_node_base> parse(context& con, const std::vector<lexer::token>& toks) noexcept;
};