#pragma once
#include <variant>
#include <string>

struct invalid_state {
	;
};

#define OBJECT std::variant<invalid_state, int, float>
inline static constexpr size_t invalid_state_index = 0;
inline static constexpr size_t int_index = 1;
inline static constexpr size_t float_index = 2;

struct get_object_name {
	std::string operator()(const auto& value) noexcept {
		return typeid(decltype(value)).name();
	}
};

struct get_object_return_code {
	int operator()(int value) noexcept {
		return value;
	}
	int operator()(float value) noexcept {
		return static_cast<int>(value);
	}
	int operator()(const auto&) noexcept {
		return 0;
	}
};

struct operate_add_object {
	OBJECT operator()(int lhs, int rhs) noexcept {
		return lhs + rhs;
	}
	OBJECT operator()(int lhs, float rhs) noexcept {
		return lhs + static_cast<int>(rhs);
	}
	OBJECT operator()(float lhs, int rhs) noexcept {
		return static_cast<int>(lhs) + rhs;
	}
	OBJECT operator()(float lhs, float rhs) noexcept {
		return lhs + rhs;
	}
	OBJECT operator()(auto&, auto&) noexcept {
		return invalid_state{};
	}
};

struct operate_sub_object {
	OBJECT operator()(int lhs, int rhs) noexcept {
		return lhs - rhs;
	}
	OBJECT operator()(int lhs, float rhs) noexcept {
		return lhs - static_cast<int>(rhs);
	}
	OBJECT operator()(float lhs, int rhs) noexcept {
		return static_cast<int>(lhs) - rhs;
	}
	OBJECT operator()(float lhs, float rhs) noexcept {
		return lhs - rhs;
	}
	OBJECT operator()(auto&, auto&) noexcept {
		return invalid_state{};
	}
};

struct operate_mul_object {
	OBJECT operator()(int lhs, int rhs) noexcept {
		return lhs * rhs;
	}
	OBJECT operator()(int lhs, float rhs) noexcept {
		return lhs * static_cast<int>(rhs);
	}
	OBJECT operator()(float lhs, int rhs) noexcept {
		return static_cast<int>(lhs) * rhs;
	}
	OBJECT operator()(float lhs, float rhs) noexcept {
		return lhs * rhs;
	}
	OBJECT operator()(auto&, auto&) noexcept {
		return invalid_state{};
	}
};

struct operate_div_object {
	OBJECT operator()(int lhs, int rhs) noexcept {
		return lhs / rhs;
	}
	OBJECT operator()(int lhs, float rhs) noexcept {
		return lhs / static_cast<int>(rhs);
	}
	OBJECT operator()(float lhs, int rhs) noexcept {
		return static_cast<int>(lhs) / rhs;
	}
	OBJECT operator()(float lhs, float rhs) noexcept {
		return lhs / rhs;
	}
	OBJECT operator()(auto&, auto&) noexcept {
		return invalid_state {};
	}
};
