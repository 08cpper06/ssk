#pragma once


struct invalid_state {
	constexpr invalid_state() {}
	invalid_state(const std::string& message) :
		message(message)
	{}

	std::string message;
};
