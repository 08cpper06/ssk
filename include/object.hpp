#pragma once
#include <variant>
#include <string>
#include "state.hpp"


#define OBJECT std::variant<invalid_state, bool, int, float>
inline static constexpr size_t state_index = OBJECT(invalid_state()).index();
inline static constexpr size_t bool_index = OBJECT(true).index();
inline static constexpr size_t int_index = OBJECT(int(0)).index();
inline static constexpr size_t float_index = OBJECT(float(0.f)).index();

struct get_object_type_name {
	std::string operator()(const auto& value) noexcept {
		return typeid(decltype(value)).name();
	}
};

struct get_object_return_code {
	std::optional<invalid_state> operator()(int value) noexcept {
		return std::nullopt;
	}
	std::optional<invalid_state> operator()(float value) noexcept {
		return std::nullopt;
	}
	std::optional<invalid_state> operator()(const auto&) noexcept {
		return invalid_state();
	}
};

struct get_object_as_string {
	std::string operator()(const invalid_state& value) noexcept {
		return "invalid state";
	}
	std::string operator()(const int& value) noexcept {
		return std::to_string(value);
	}
	std::string operator()(const float& value) noexcept {
		return std::to_string(value);
	}
	std::string operator()(const auto& value) noexcept {
		return "invalid state";
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
		if (!rhs) {
			return invalid_state {};
		}
		return lhs / rhs;
	}
	OBJECT operator()(int lhs, float rhs) noexcept {
		if (!rhs) {
			return invalid_state {};
		}
		return lhs / static_cast<int>(rhs);
	}
	OBJECT operator()(float lhs, int rhs) noexcept {
		if (!rhs) {
			return invalid_state {};
		}
		return static_cast<int>(lhs) / rhs;
	}
	OBJECT operator()(float lhs, float rhs) noexcept {
		if (!rhs) {
			return invalid_state {};
		}
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
