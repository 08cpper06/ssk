#pragma once
#include <variant>
#include <string>
#include "state.hpp"


#define OBJECT std::variant<invalid_state, bool, int, float, std::vector<bool>, std::vector<int>, std::vector<float>>
inline static constexpr size_t state_index = OBJECT(invalid_state()).index();
inline static constexpr size_t bool_index = OBJECT(true).index();
inline static constexpr size_t int_index = OBJECT(int(0)).index();
inline static constexpr size_t float_index = OBJECT(float(0.f)).index();

inline static constexpr size_t bool_array_index = OBJECT(std::vector<bool>()).index();
inline static constexpr size_t int_array_index = OBJECT(std::vector<int>()).index();
inline static constexpr size_t float_array_index = OBJECT(std::vector<float>()).index();

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

struct operate_assign_object {
	operate_assign_object(int index) :
		index(index)
	{}

	OBJECT operator()(std::vector<float>& lhs, int rhs) {
		if (index < 0 || index >= lhs.size()) {
			return invalid_state {};
		}
		lhs[index] = static_cast<float>(rhs);
		return lhs[index];
	}
	OBJECT operator()(std::vector<float>& lhs, float rhs) {
		if (index < 0 || index >= lhs.size()) {
			return invalid_state {};
		}
		lhs[index] = rhs;
		return lhs[index];
	}
	OBJECT operator()(std::vector<int>& lhs, int rhs) {
		if (index < 0 || index >= lhs.size()) {
			return invalid_state{};
		}
		lhs[index] = rhs;
		return lhs[index];
	}
	OBJECT operator()(std::vector<int>& lhs, float rhs) {
		if (index < 0 || index >= lhs.size()) {
			return invalid_state{};
		}
		lhs[index] = static_cast<int>(rhs);
		return lhs[index];
	}

	OBJECT operator()(std::vector<bool>& lhs, bool rhs) {
		if (index < 0 || index >= lhs.size()) {
			return invalid_state{};
		}
		lhs[index] = rhs;
		return lhs[index];
	}

	OBJECT operator()(auto&, auto&) {
		return invalid_state {};
	}

	int index;
};

struct operate_index_ref_object {
	operate_index_ref_object(int index) :
		index(index)
	{}

	OBJECT operator()(std::vector<float>& value) {
		if (index < 0 || index >= value.size()) {
			return invalid_state {};
		}
		return value[index];
	}
	OBJECT operator()(std::vector<int>& value) {
		if (index < 0 || index >= value.size()) {
			return invalid_state {};
		}
		return value[index];
	}
	OBJECT operator()(std::vector<bool>& value) {
		if (index < 0 || index >= value.size()) {
			return invalid_state {};
		}
		return value[index];
	}
	OBJECT operator()(auto&) {
		return invalid_state {};
	}

	int index;
};

struct operate_add_object {
	operate_add_object(int lhs_index, int rhs_index) :
		lhs_index(lhs_index),
		rhs_index(rhs_index)
	{}

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

	OBJECT operator()(std::vector<int>& lhs, std::vector<int>& rhs) {
		if (lhs_index <= 0 || rhs_index <= 0) {
			return invalid_state {};
		}
		if (lhs_index >= lhs.size() || rhs_index >= rhs.size()) {
			return invalid_state {};
		}
		return lhs[lhs_index] + rhs[rhs_index];
	}
	OBJECT operator()(std::vector<int>& lhs, std::vector<float>& rhs) {
		if (lhs_index <= 0 || rhs_index <= 0) {
			return invalid_state {};
		}
		if (lhs_index >= lhs.size() || rhs_index >= rhs.size()) {
			return invalid_state {};
		}
		return lhs[lhs_index] + static_cast<int>(rhs[rhs_index]);
	}
	OBJECT operator()(std::vector<float>& lhs, std::vector<int>& rhs) {
		if (lhs_index <= 0 || rhs_index <= 0) {
			return invalid_state{};
		}
		if (lhs_index >= lhs.size() || rhs_index >= rhs.size()) {
			return invalid_state{};
		}
		return static_cast<int>(lhs[lhs_index]) + rhs[rhs_index];
	}
	OBJECT operator()(std::vector<float>& lhs, std::vector<float>& rhs) {
		if (lhs_index <= 0 || rhs_index <= 0) {
			return invalid_state{};
		}
		if (lhs_index >= lhs.size() || rhs_index >= rhs.size()) {
			return invalid_state{};
		}
		return lhs[lhs_index] + rhs[rhs_index];
	}

	OBJECT operator()(auto&, auto&) noexcept {
		return invalid_state{};
	}

	int lhs_index;
	int rhs_index;
};

struct operate_sub_object {
	operate_sub_object(int lhs_index, int rhs_index) :
		lhs_index(lhs_index),
		rhs_index(rhs_index)
	{}

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

	OBJECT operator()(std::vector<int>& lhs, std::vector<int>& rhs) {
		if (lhs_index <= 0 || rhs_index <= 0) {
			return invalid_state{};
		}
		if (lhs_index >= lhs.size() || rhs_index >= rhs.size()) {
			return invalid_state{};
		}
		return lhs[lhs_index] - rhs[rhs_index];
	}
	OBJECT operator()(std::vector<int>& lhs, std::vector<float>& rhs) {
		if (lhs_index <= 0 || rhs_index <= 0) {
			return invalid_state{};
		}
		if (lhs_index >= lhs.size() || rhs_index >= rhs.size()) {
			return invalid_state{};
		}
		return lhs[lhs_index] - static_cast<int>(rhs[rhs_index]);
	}
	OBJECT operator()(std::vector<float>& lhs, std::vector<int>& rhs) {
		if (lhs_index <= 0 || rhs_index <= 0) {
			return invalid_state{};
		}
		if (lhs_index >= lhs.size() || rhs_index >= rhs.size()) {
			return invalid_state{};
		}
		return static_cast<int>(lhs[lhs_index]) - rhs[rhs_index];
	}
	OBJECT operator()(std::vector<float>& lhs, std::vector<float>& rhs) {
		if (lhs_index <= 0 || rhs_index <= 0) {
			return invalid_state{};
		}
		if (lhs_index >= lhs.size() || rhs_index >= rhs.size()) {
			return invalid_state{};
		}
		return lhs[lhs_index] - rhs[rhs_index];
	}

	OBJECT operator()(auto&, auto&) noexcept {
		return invalid_state{};
	}

	int lhs_index;
	int rhs_index;
};

struct operate_mul_object {
	operate_mul_object(int lhs_index, int rhs_index) :
		lhs_index(lhs_index),
		rhs_index(rhs_index)
	{}

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

	OBJECT operator()(std::vector<int>& lhs, std::vector<int>& rhs) {
		if (lhs_index <= 0 || rhs_index <= 0) {
			return invalid_state{};
		}
		if (lhs_index >= lhs.size() || rhs_index >= rhs.size()) {
			return invalid_state{};
		}
		return lhs[lhs_index] * rhs[rhs_index];
	}
	OBJECT operator()(std::vector<int>& lhs, std::vector<float>& rhs) {
		if (lhs_index <= 0 || rhs_index <= 0) {
			return invalid_state{};
		}
		if (lhs_index >= lhs.size() || rhs_index >= rhs.size()) {
			return invalid_state{};
		}
		return lhs[lhs_index] * static_cast<int>(rhs[rhs_index]);
	}
	OBJECT operator()(std::vector<float>& lhs, std::vector<int>& rhs) {
		if (lhs_index <= 0 || rhs_index <= 0) {
			return invalid_state{};
		}
		if (lhs_index >= lhs.size() || rhs_index >= rhs.size()) {
			return invalid_state{};
		}
		return static_cast<int>(lhs[lhs_index]) * rhs[rhs_index];
	}
	OBJECT operator()(std::vector<float>& lhs, std::vector<float>& rhs) {
		if (lhs_index <= 0 || rhs_index <= 0) {
			return invalid_state{};
		}
		if (lhs_index >= lhs.size() || rhs_index >= rhs.size()) {
			return invalid_state{};
		}
		return lhs[lhs_index] * rhs[rhs_index];
	}

	OBJECT operator()(auto&, auto&) noexcept {
		return invalid_state{};
	}

	int lhs_index;
	int rhs_index;
};

struct operate_div_object {
	operate_div_object(int lhs_index, int rhs_index) :
		lhs_index(lhs_index),
		rhs_index(rhs_index)
	{}

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

	OBJECT operator()(std::vector<int>& lhs, std::vector<int>& rhs) {
		if (lhs_index <= 0 || rhs_index <= 0) {
			return invalid_state{};
		}
		if (lhs_index >= lhs.size() || rhs_index >= rhs.size()) {
			return invalid_state{};
		}
		return lhs[lhs_index] * rhs[rhs_index];
	}
	OBJECT operator()(std::vector<int>& lhs, std::vector<float>& rhs) {
		if (lhs_index <= 0 || rhs_index <= 0) {
			return invalid_state{};
		}
		if (lhs_index >= lhs.size() || rhs_index >= rhs.size()) {
			return invalid_state{};
		}
		return lhs[lhs_index] * static_cast<int>(rhs[rhs_index]);
	}
	OBJECT operator()(std::vector<float>& lhs, std::vector<int>& rhs) {
		if (lhs_index <= 0 || rhs_index <= 0) {
			return invalid_state{};
		}
		if (lhs_index >= lhs.size() || rhs_index >= rhs.size()) {
			return invalid_state{};
		}
		return static_cast<int>(lhs[lhs_index]) * rhs[rhs_index];
	}
	OBJECT operator()(std::vector<float>& lhs, std::vector<float>& rhs) {
		if (lhs_index <= 0 || rhs_index <= 0) {
			return invalid_state{};
		}
		if (lhs_index >= lhs.size() || rhs_index >= rhs.size()) {
			return invalid_state{};
		}
		return lhs[lhs_index] * rhs[rhs_index];
	}

	OBJECT operator()(auto&, auto&) noexcept {
		return invalid_state {};
	}

	int lhs_index;
	int rhs_index;
};

struct operate_equal_object {
	operate_equal_object(int lhs_index, int rhs_index) :
		lhs_index(lhs_index),
		rhs_index(rhs_index)
	{}

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

	OBJECT operator()(std::vector<int>& lhs, std::vector<int>& rhs) {
		if (lhs_index <= 0 || rhs_index <= 0) {
			return invalid_state{};
		}
		if (lhs_index >= lhs.size() || rhs_index >= rhs.size()) {
			return invalid_state{};
		}
		return lhs[lhs_index] == rhs[rhs_index];
	}
	OBJECT operator()(std::vector<int>& lhs, std::vector<float>& rhs) {
		if (lhs_index <= 0 || rhs_index <= 0) {
			return invalid_state{};
		}
		if (lhs_index >= lhs.size() || rhs_index >= rhs.size()) {
			return invalid_state{};
		}
		return lhs[lhs_index] == static_cast<int>(rhs[rhs_index]);
	}
	OBJECT operator()(std::vector<float>& lhs, std::vector<int>& rhs) {
		if (lhs_index <= 0 || rhs_index <= 0) {
			return invalid_state{};
		}
		if (lhs_index >= lhs.size() || rhs_index >= rhs.size()) {
			return invalid_state{};
		}
		return static_cast<int>(lhs[lhs_index]) == rhs[rhs_index];
	}
	OBJECT operator()(std::vector<float>& lhs, std::vector<float>& rhs) {
		if (lhs_index <= 0 || rhs_index <= 0) {
			return invalid_state{};
		}
		if (lhs_index >= lhs.size() || rhs_index >= rhs.size()) {
			return invalid_state{};
		}
		return lhs[lhs_index] == rhs[rhs_index];
	}

	OBJECT operator()(auto&, auto&) noexcept {
		return invalid_state{};
	}

	int lhs_index;
	int rhs_index;
};

struct operate_not_object {
	operate_not_object(int lhs_index, int rhs_index) :
		lhs_index(lhs_index),
		rhs_index(rhs_index)
	{}

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

	OBJECT operator()(std::vector<int>& lhs, std::vector<int>& rhs) {
		if (lhs_index <= 0 || rhs_index <= 0) {
			return invalid_state{};
		}
		if (lhs_index >= lhs.size() || rhs_index >= rhs.size()) {
			return invalid_state{};
		}
		return lhs[lhs_index] != rhs[rhs_index];
	}
	OBJECT operator()(std::vector<int>& lhs, std::vector<float>& rhs) {
		if (lhs_index <= 0 || rhs_index <= 0) {
			return invalid_state{};
		}
		if (lhs_index >= lhs.size() || rhs_index >= rhs.size()) {
			return invalid_state{};
		}
		return lhs[lhs_index] != static_cast<int>(rhs[rhs_index]);
	}
	OBJECT operator()(std::vector<float>& lhs, std::vector<int>& rhs) {
		if (lhs_index <= 0 || rhs_index <= 0) {
			return invalid_state{};
		}
		if (lhs_index >= lhs.size() || rhs_index >= rhs.size()) {
			return invalid_state{};
		}
		return static_cast<int>(lhs[lhs_index]) != rhs[rhs_index];
	}
	OBJECT operator()(std::vector<float>& lhs, std::vector<float>& rhs) {
		if (lhs_index <= 0 || rhs_index <= 0) {
			return invalid_state{};
		}
		if (lhs_index >= lhs.size() || rhs_index >= rhs.size()) {
			return invalid_state{};
		}
		return lhs[lhs_index] != rhs[rhs_index];
	}

	OBJECT operator()(auto&, auto&) noexcept {
		return invalid_state{};
	}

	int lhs_index;
	int rhs_index;
};

struct operate_less_than_object {
	operate_less_than_object(int lhs_index, int rhs_index) :
		lhs_index(lhs_index),
		rhs_index(rhs_index)
	{}

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

	OBJECT operator()(std::vector<int>& lhs, std::vector<int>& rhs) {
		if (lhs_index <= 0 || rhs_index <= 0) {
			return invalid_state{};
		}
		if (lhs_index >= lhs.size() || rhs_index >= rhs.size()) {
			return invalid_state{};
		}
		return lhs[lhs_index] < rhs[rhs_index];
	}
	OBJECT operator()(std::vector<int>& lhs, std::vector<float>& rhs) {
		if (lhs_index <= 0 || rhs_index <= 0) {
			return invalid_state{};
		}
		if (lhs_index >= lhs.size() || rhs_index >= rhs.size()) {
			return invalid_state{};
		}
		return lhs[lhs_index] < static_cast<int>(rhs[rhs_index]);
	}
	OBJECT operator()(std::vector<float>& lhs, std::vector<int>& rhs) {
		if (lhs_index <= 0 || rhs_index <= 0) {
			return invalid_state{};
		}
		if (lhs_index >= lhs.size() || rhs_index >= rhs.size()) {
			return invalid_state{};
		}
		return static_cast<int>(lhs[lhs_index]) < rhs[rhs_index];
	}
	OBJECT operator()(std::vector<float>& lhs, std::vector<float>& rhs) {
		if (lhs_index <= 0 || rhs_index <= 0) {
			return invalid_state{};
		}
		if (lhs_index >= lhs.size() || rhs_index >= rhs.size()) {
			return invalid_state{};
		}
		return lhs[lhs_index] < rhs[rhs_index];
	}

	OBJECT operator()(auto&, auto&) noexcept {
		return invalid_state{};
	}

	int lhs_index;
	int rhs_index;
};

struct operate_less_than_or_equal_object {
	operate_less_than_or_equal_object(int lhs_index, int rhs_index) :
		lhs_index(lhs_index),
		rhs_index(rhs_index)
	{}

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

	OBJECT operator()(std::vector<int>& lhs, std::vector<int>& rhs) {
		if (lhs_index <= 0 || rhs_index <= 0) {
			return invalid_state{};
		}
		if (lhs_index >= lhs.size() || rhs_index >= rhs.size()) {
			return invalid_state{};
		}
		return lhs[lhs_index] <= rhs[rhs_index];
	}
	OBJECT operator()(std::vector<int>& lhs, std::vector<float>& rhs) {
		if (lhs_index <= 0 || rhs_index <= 0) {
			return invalid_state{};
		}
		if (lhs_index >= lhs.size() || rhs_index >= rhs.size()) {
			return invalid_state{};
		}
		return lhs[lhs_index] <= static_cast<int>(rhs[rhs_index]);
	}
	OBJECT operator()(std::vector<float>& lhs, std::vector<int>& rhs) {
		if (lhs_index <= 0 || rhs_index <= 0) {
			return invalid_state{};
		}
		if (lhs_index >= lhs.size() || rhs_index >= rhs.size()) {
			return invalid_state{};
		}
		return static_cast<int>(lhs[lhs_index]) <= rhs[rhs_index];
	}
	OBJECT operator()(std::vector<float>& lhs, std::vector<float>& rhs) {
		if (lhs_index <= 0 || rhs_index <= 0) {
			return invalid_state{};
		}
		if (lhs_index >= lhs.size() || rhs_index >= rhs.size()) {
			return invalid_state{};
		}
		return lhs[lhs_index] <= rhs[rhs_index];
	}

	OBJECT operator()(auto&, auto&) noexcept {
		return invalid_state{};
	}

	int lhs_index;
	int rhs_index;
};

struct operate_greater_than_object {
	operate_greater_than_object(int lhs_index, int rhs_index) :
		lhs_index(lhs_index),
		rhs_index(rhs_index)
	{}

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

	OBJECT operator()(std::vector<int>& lhs, std::vector<int>& rhs) {
		if (lhs_index <= 0 || rhs_index <= 0) {
			return invalid_state{};
		}
		if (lhs_index >= lhs.size() || rhs_index >= rhs.size()) {
			return invalid_state{};
		}
		return lhs[lhs_index] > rhs[rhs_index];
	}
	OBJECT operator()(std::vector<int>& lhs, std::vector<float>& rhs) {
		if (lhs_index <= 0 || rhs_index <= 0) {
			return invalid_state{};
		}
		if (lhs_index >= lhs.size() || rhs_index >= rhs.size()) {
			return invalid_state{};
		}
		return lhs[lhs_index] > static_cast<int>(rhs[rhs_index]);
	}
	OBJECT operator()(std::vector<float>& lhs, std::vector<int>& rhs) {
		if (lhs_index <= 0 || rhs_index <= 0) {
			return invalid_state{};
		}
		if (lhs_index >= lhs.size() || rhs_index >= rhs.size()) {
			return invalid_state{};
		}
		return static_cast<int>(lhs[lhs_index]) > rhs[rhs_index];
	}
	OBJECT operator()(std::vector<float>& lhs, std::vector<float>& rhs) {
		if (lhs_index <= 0 || rhs_index <= 0) {
			return invalid_state{};
		}
		if (lhs_index >= lhs.size() || rhs_index >= rhs.size()) {
			return invalid_state{};
		}
		return lhs[lhs_index] > rhs[rhs_index];
	}

	OBJECT operator()(auto&, auto&) noexcept {
		return invalid_state{};
	}

	int lhs_index;
	int rhs_index;
};

struct operate_greater_than_or_equal_object {
	operate_greater_than_or_equal_object(int lhs_index, int rhs_index) :
		lhs_index(lhs_index),
		rhs_index(rhs_index)
	{}

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

	OBJECT operator()(std::vector<int>& lhs, std::vector<int>& rhs) {
		if (lhs_index <= 0 || rhs_index <= 0) {
			return invalid_state{};
		}
		if (lhs_index >= lhs.size() || rhs_index >= rhs.size()) {
			return invalid_state{};
		}
		return lhs[lhs_index] >= rhs[rhs_index];
	}
	OBJECT operator()(std::vector<int>& lhs, std::vector<float>& rhs) {
		if (lhs_index <= 0 || rhs_index <= 0) {
			return invalid_state{};
		}
		if (lhs_index >= lhs.size() || rhs_index >= rhs.size()) {
			return invalid_state{};
		}
		return lhs[lhs_index] >= static_cast<int>(rhs[rhs_index]);
	}
	OBJECT operator()(std::vector<float>& lhs, std::vector<int>& rhs) {
		if (lhs_index <= 0 || rhs_index <= 0) {
			return invalid_state{};
		}
		if (lhs_index >= lhs.size() || rhs_index >= rhs.size()) {
			return invalid_state{};
		}
		return static_cast<int>(lhs[lhs_index]) >= rhs[rhs_index];
	}
	OBJECT operator()(std::vector<float>& lhs, std::vector<float>& rhs) {
		if (lhs_index <= 0 || rhs_index <= 0) {
			return invalid_state{};
		}
		if (lhs_index >= lhs.size() || rhs_index >= rhs.size()) {
			return invalid_state{};
		}
		return lhs[lhs_index] >= rhs[rhs_index];
	}

	OBJECT operator()(auto&, auto&) noexcept {
		return invalid_state{};
	}

	int lhs_index;
	int rhs_index;
};
