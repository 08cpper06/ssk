#pragma once
#include <variant>
#include <string>

struct invalid_state {
	;
};

#define OBJECT std::variant<invalid_state, bool, int, float>
inline static constexpr size_t invalid_state_index = OBJECT(invalid_state()).index();
inline static constexpr size_t bool_index = OBJECT(true).index();
inline static constexpr size_t int_index = OBJECT(int(0)).index();
inline static constexpr size_t float_index = OBJECT(float(0.f)).index();

struct get_object_type_name {
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

struct cast_bool_object {
	OBJECT operator()(int value) noexcept {
		return value;
	}
	OBJECT operator()(float value) noexcept {
		return value;
	}
	OBJECT operator()(bool value) noexcept {
		return value;
	}
	OBJECT operator()(auto&) noexcept {
		return invalid_state{};
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

struct operate_equal_object {
	OBJECT operator()(int lhs, int rhs) noexcept {
		return lhs == rhs;
	}
	OBJECT operator()(int lhs, float rhs) noexcept {
		return lhs == static_cast<int>(rhs);
	}
	OBJECT operator()(float lhs, int rhs) noexcept {
		return static_cast<int>(lhs) == rhs;
	}
	OBJECT operator()(float lhs, float rhs) noexcept {
		return lhs == rhs;
	}
	OBJECT operator()(auto&, auto&) noexcept {
		return invalid_state{};
	}
};

struct operate_not_object {
	OBJECT operator()(int lhs, int rhs) noexcept {
		return lhs != rhs;
	}
	OBJECT operator()(int lhs, float rhs) noexcept {
		return lhs != static_cast<int>(rhs);
	}
	OBJECT operator()(float lhs, int rhs) noexcept {
		return static_cast<int>(lhs) != rhs;
	}
	OBJECT operator()(float lhs, float rhs) noexcept {
		return lhs != rhs;
	}
	OBJECT operator()(auto&, auto&) noexcept {
		return invalid_state{};
	}
};

struct operate_less_than_object {
	OBJECT operator()(int lhs, int rhs) noexcept {
		return lhs < rhs;
	}
	OBJECT operator()(int lhs, float rhs) noexcept {
		return lhs < static_cast<int>(rhs);
	}
	OBJECT operator()(float lhs, int rhs) noexcept {
		return static_cast<int>(lhs) < rhs;
	}
	OBJECT operator()(float lhs, float rhs) noexcept {
		return lhs < rhs;
	}
	OBJECT operator()(auto&, auto&) noexcept {
		return invalid_state{};
	}
};

struct operate_less_than_or_equal_object {
	OBJECT operator()(int lhs, int rhs) noexcept {
		return lhs <= rhs;
	}
	OBJECT operator()(int lhs, float rhs) noexcept {
		return lhs <= static_cast<int>(rhs);
	}
	OBJECT operator()(float lhs, int rhs) noexcept {
		return static_cast<int>(lhs) <= rhs;
	}
	OBJECT operator()(float lhs, float rhs) noexcept {
		return lhs <= rhs;
	}
	OBJECT operator()(auto&, auto&) noexcept {
		return invalid_state{};
	}
};

struct operate_greater_than_object {
	OBJECT operator()(int lhs, int rhs) noexcept {
		return lhs > rhs;
	}
	OBJECT operator()(int lhs, float rhs) noexcept {
		return lhs > static_cast<int>(rhs);
	}
	OBJECT operator()(float lhs, int rhs) noexcept {
		return static_cast<int>(lhs) > rhs;
	}
	OBJECT operator()(float lhs, float rhs) noexcept {
		return lhs > rhs;
	}
	OBJECT operator()(auto&, auto&) noexcept {
		return invalid_state{};
	}
};

struct operate_greater_than_or_equal_object {
	OBJECT operator()(int lhs, int rhs) noexcept {
		return lhs >= rhs;
	}
	OBJECT operator()(int lhs, float rhs) noexcept {
		return lhs >= static_cast<int>(rhs);
	}
	OBJECT operator()(float lhs, int rhs) noexcept {
		return static_cast<int>(lhs) >= rhs;
	}
	OBJECT operator()(float lhs, float rhs) noexcept {
		return lhs >= rhs;
	}
	OBJECT operator()(auto&, auto&) noexcept {
		return invalid_state{};
	}
};
