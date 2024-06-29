#pragma once

#include <memory>
#include "lexer.hpp"


template <class Type, class UType>
bool is_a(UType target) {
	return target->get_tag() == &Type::tag;
}

class ast_node_base {
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

};

class ast_node_value : public ast_node_base {
public:
	struct ast_value_tag : public ast_base_tag {};
	inline static constexpr ast_value_tag tag;
public:
	ast_node_value(const lexer::token& value) :
		value(value)
	{}
	virtual ~ast_node_value() = default;

	virtual const ast_base_tag* get_tag() const { return &ast_node_value::tag; }
	virtual std::string log(std::string indent) {
		return indent + "<value>" + value.raw + "</value>\n";
	}
	lexer::token value;
};

class ast_node_bin : public ast_node_base {
public:
	struct ast_bin_tag : public ast_base_tag {};
	inline static constexpr ast_bin_tag tag;
public:
	ast_node_bin(const std::string& op, std::unique_ptr<ast_node_base>&& lhs, std::unique_ptr<ast_node_base>&& rhs) :
		op(op),
		lhs(std::move(lhs)),
		rhs(std::move(rhs))
	{}
	~ast_node_bin() = default;

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
	std::string op;
	std::unique_ptr<ast_node_base> lhs;
	std::unique_ptr<ast_node_base> rhs;
};

class parser {
public:
	static std::unique_ptr<ast_node_base> try_build_value(std::vector<lexer::token>::const_iterator& itr);
	static std::unique_ptr<ast_node_base> try_build_timedivide_node(std::vector<lexer::token>::const_iterator& itr);
	static std::unique_ptr<ast_node_base> try_build_plusminus_node(std::vector<lexer::token>::const_iterator& itr);
public:
	static std::unique_ptr<ast_node_base> parse(const std::vector<lexer::token>& toks) noexcept;
};