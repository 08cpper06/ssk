#pragma once


struct state {
	;
};

struct invalid_state {
	constexpr invalid_state() {}
	invalid_state(const std::string& message) :
		message(message)
	{}
	operator state() { return state(); }

	std::string message;
};
