#include <iostream>
#include <fstream>
#include <string>
#include "runtime.hpp"


int main(int argc, const char* argv[]) {
	if (argc == 1) {
		std::cout << "no input" << std::endl;
		return 1;
	}

	std::ifstream in(argv[1]);
	std::string source = std::string(std::istreambuf_iterator<char>(in), std::istreambuf_iterator<char>());

	std::cout << "=== source ===" << std::endl;

	std::cout << source << std::endl;

	std::cout << "=== tokens ===" << std::endl;
	
	std::vector<lexer::token> toks = lexer::tokenize(source);
	for (const auto item : toks) {
		std::cout << item.raw << std::endl;
	}

	std::cout << "===   AST  ===" << std::endl;

	context con;
	std::unique_ptr<ast_node_base> root = parser::parse(con, toks);
	if (root) {
		std::cout << root->log("") << std::endl;
	} else {
		return 1;
	}

	std::cout << "==============" << std::endl;
	runtime::evaluate_pre_process(con);
	if (con.func_table.find("main") != con.func_table.end()) {
		con.func_table["main"].block->evaluate(con);
	} else {
		std::cout << "not found main()" << std::endl;
		return 2;
	}
	if (con.stack.size()) {
		OBJECT return_code = con.stack.back();
		std::cout << "return code: " << std::visit(get_object_as_string {}, return_code) << std::endl;
	}

	return 0;
}