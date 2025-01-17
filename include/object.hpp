#pragma once
#include <variant>
#include <string>
#include "state.hpp"


#define OBJECT std::variant<invalid_state, bool, int, float, std::string, std::vector<bool>, std::vector<int>, std::vector<float>, std::vector<std::string>>
inline static constexpr size_t state_index = OBJECT(invalid_state()).index();
inline static constexpr size_t bool_index = OBJECT(true).index();
inline static constexpr size_t int_index = OBJECT(int(0)).index();
inline static constexpr size_t float_index = OBJECT(float(0.f)).index();
inline static constexpr size_t string_index = OBJECT(std::string()).index();

inline static constexpr size_t bool_array_index = OBJECT(std::vector<bool>()).index();
inline static constexpr size_t int_array_index = OBJECT(std::vector<int>()).index();
inline static constexpr size_t float_array_index = OBJECT(std::vector<float>()).index();
inline static constexpr size_t string_array_index = OBJECT(std::vector<std::string>()).index();

inline static std::string type_names[] = {
	"invalid_state", "bool", "int", "float", "string",
	"bool[]", "int[]", "float[]", "string[]"
};


struct get_object_type_name {
	std::string operator()(bool value) noexcept { return "bool"; }
	std::string operator()(int value) noexcept { return "int"; }
	std::string operator()(float value) noexcept { return "float"; }
	std::string operator()(const std::string& value) noexcept { return "str"; }

	std::string operator()(const std::vector<bool>& value) noexcept { return "bool[]"; }
	std::string operator()(const std::vector<int>& value) noexcept { return "int[]"; }
	std::string operator()(const std::vector<float>& value) noexcept { return "float[]"; }
	std::string operator()(const std::vector<std::string>& value) noexcept { return "str[]"; }

	std::string operator()(auto value) noexcept {
		return "invalid_state";
	}
};

struct make_array {
	OBJECT operator()(int value) noexcept {
		return std::vector<int>({ value });
	}
	OBJECT operator()(float value) noexcept {
		return std::vector<float>({ value });
	}
	OBJECT operator()(bool value) noexcept {
		return std::vector<bool>({ value });
	}
	OBJECT operator()(const std::string& value) noexcept {
		return std::vector<std::string>({ value });
	}
	OBJECT operator()(const std::vector<int>& values) noexcept {
		return values;
	}
	OBJECT operator()(const std::vector<float>& values) noexcept {
		return values;
	}
	OBJECT operator()(const std::vector<bool>& values) noexcept {
		return values;
	}
	OBJECT operator()(const std::vector<std::string>& values) noexcept {
		return values;
	}
	OBJECT operator()(const auto&) noexcept {
		return invalid_state("make list is failed");
	}
};

struct get_array_size {
	int operator()(int value) noexcept {
		return 1;
	}
	int operator()(float value) noexcept {
		return 1;
	}
	int operator()(bool value) noexcept {
		return 1;
	}
	int operator()(const std::string& value) noexcept {
		return 1;
	}

	int operator()(const std::vector<int>& values) noexcept {
		return values.size();
	}
	int operator()(const std::vector<float>& values) noexcept {
		return values.size();
	}
	int operator()(const std::vector<bool>& values) noexcept {
		return values.size();
	}
	int operator()(const std::vector<std::string>& values) noexcept {
		return values.size();
	}

	int operator()(auto) {
		return -1;
	}
};

struct insert_to_array {
	insert_to_array(int index) :
		index(index)
	{}

	std::optional<invalid_state> operator()(std::vector<int>& values, int value) noexcept {
		if (index < 0) {
			values.insert(values.begin() + (values.size() + index + 1), value);
		} else {
			values.insert(values.begin() + index, value);
		}
		return std::nullopt;
	}
	std::optional<invalid_state> operator()(std::vector<float>& values, float value) noexcept {
		if (index < 0) {
			values.insert(values.begin() + (values.size() + index + 1), value);
		} else {
			values.insert(values.begin() + index, value);
		}
		return std::nullopt;
	}
	std::optional<invalid_state> operator()(std::vector<bool>& values, bool value) noexcept {
		if (index < 0) {
			values.insert(values.begin() + (values.size() + index + 1), value);
		} else {
			values.insert(values.begin() + index, value);
		}
		return std::nullopt;
	}
	std::optional<invalid_state> operator()(std::vector<std::string>& values, const std::string& value) noexcept {
		if (index < 0) {
			values.insert(values.begin() + (values.size() + index + 1), value);
		} else {
			values.insert(values.begin() + index, value);
		}
		return std::nullopt;
	}

	std::optional<invalid_state> operator()(std::vector<int>& values) noexcept {
		if (index < 0) {
			values.insert(values.begin() + (values.size() + index + 1), int());
		} else {
			values.insert(values.begin() + index, int());
		}
		return std::nullopt;
	}
	std::optional<invalid_state> operator()(std::vector<float>& values) noexcept {
		if (index < 0) {
			values.insert(values.begin() + (values.size() + index + 1), float());
		}
		else {
			values.insert(values.begin() + index, float());
		}
		return std::nullopt;
	}
	std::optional<invalid_state> operator()(std::vector<bool>& values) noexcept {
		if (index < 0) {
			values.insert(values.begin() + (values.size() + index + 1), bool());
		}
		else {
			values.insert(values.begin() + index, bool());
		}
		return std::nullopt;
	}
	std::optional<invalid_state> operator()(std::vector<std::string>& values) noexcept {
		if (index < 0) {
			values.insert(values.begin() + (values.size() + index + 1), std::string());
		}
		else {
			values.insert(values.begin() + index, std::string());
		}
		return std::nullopt;
	}

	std::optional<invalid_state> operator()(auto) noexcept {
		return invalid_state("failed to insert to array");
	}
	std::optional<invalid_state> operator()(auto, auto) noexcept {
		return invalid_state("failed to insert to array");
	}

	int index { -1 };
};

struct get_object_return_code {
	std::optional<invalid_state> operator()(int value) noexcept {
		return std::nullopt;
	}
	std::optional<invalid_state> operator()(float value) noexcept {
		return std::nullopt;
	}
	std::optional<invalid_state> operator()(bool value) noexcept {
		return std::nullopt;
	}
	std::optional<invalid_state> operator()(const std::string& value) noexcept {
		return std::nullopt;
	}
	std::optional<invalid_state> operator()(const std::vector<int>& value) noexcept {
		return std::nullopt;
	}
	std::optional<invalid_state> operator()(const std::vector<float>& value) noexcept {
		return std::nullopt;
	}
	std::optional<invalid_state> operator()(std::vector<bool>& value) noexcept {
		return std::nullopt;
	}
	std::optional<invalid_state> operator()(const std::vector<std::string>& value) noexcept {
		return std::nullopt;
	}
	std::optional<invalid_state> operator()(auto) noexcept {
		return invalid_state("invalid return state");
	}
};

struct get_object_as_string {
	std::string operator()(const invalid_state& value) noexcept {
		return value.message;
	}
	std::string operator()(const int& value) noexcept {
		return std::to_string(value);
	}
	std::string operator()(const float& value) noexcept {
		return std::to_string(value);
	}
	std::string operator()(const bool& value) noexcept {
		return std::to_string(value);
	}
	std::string operator()(const std::string& value) noexcept {
		return value;
	}
	std::string operator()(const std::vector<bool>& values) {
		std::string str = "[";
		char sep = '\0';
		for (const bool& value : values) {
			str += std::exchange(sep, ',') + (value ? "true" : "false");
		}
		return str + "]";
	}
	std::string operator()(const std::vector<int>& values) {
		std::string str = "[";
		char sep = '\0';
		for (const int& value : values) {
			str += std::exchange(sep, ',') + std::to_string(value);
		}
		return str + "]";
	}
	std::string operator()(const std::vector<float>& values) {
		std::string str = "[";
		char sep = '\0';
		for (const float& value : values) {
			str += std::exchange(sep, ',') + std::to_string(value);
		}
		return str + "]";
	}
	std::string operator()(const std::vector<std::string>& values) {
		std::string str = "[";
		std::string sep = "";
		for (const std::string& value : values) {
			str += std::exchange(sep, '\",') + "\"" + value + "\"";
		}
		return str + "]";
	}
	std::string operator()(const auto& value) noexcept {
		return "invalid state (get_object_as_string)";
	}
};

struct cast_bool_object {
	OBJECT operator()(int value) noexcept {
		return value != 0;
	}
	OBJECT operator()(float value) noexcept {
		return value != 0.f;
	}
	OBJECT operator()(bool value) noexcept {
		return value;
	}
	OBJECT operator()(std::string& value) noexcept {
		return !value.empty();
	}
	OBJECT operator()(auto&) noexcept {
		return invalid_state("failed to cast as bool");
	}
};

struct operate_assign_object {
	operate_assign_object(int index) :
		index(index)
	{}

	OBJECT operator()(std::vector<float>& lhs, int rhs) {
		if (index < 0 || index >= lhs.size()) {
			return invalid_state("out of range (access: " + std::to_string(index) + ", size: " + std::to_string(lhs.size()) + ")");
		}
		lhs[index] = static_cast<float>(rhs);
		return lhs[index];
	}
	OBJECT operator()(std::vector<float>& lhs, float rhs) {
		if (index < 0 || index >= lhs.size()) {
			return invalid_state("out of range (access: " + std::to_string(index) + ", size: " + std::to_string(lhs.size()) + ")");
		}
		lhs[index] = rhs;
		return lhs[index];
	}
	OBJECT operator()(std::vector<int>& lhs, int rhs) {
		if (index < 0 || index >= lhs.size()) {
			return invalid_state("out of range (access: " + std::to_string(index) + ", size: " + std::to_string(lhs.size()) + ")");
		}
		lhs[index] = rhs;
		return lhs[index];
	}
	OBJECT operator()(std::vector<int>& lhs, float rhs) {
		if (index < 0 || index >= lhs.size()) {
			return invalid_state("out of range (access: " + std::to_string(index) + ", size: " + std::to_string(lhs.size()) + ")");
		}
		lhs[index] = static_cast<int>(rhs);
		return lhs[index];
	}

	OBJECT operator()(std::vector<std::string>& lhs, std::string& rhs) {
		if (index < 0 || index >= lhs.size()) {
			return invalid_state("out of range (access: " + std::to_string(index) + ", size: " + std::to_string(lhs.size()) + ")");
		}
		lhs[index] = rhs;
		return lhs[index];
	}

	OBJECT operator()(std::vector<bool>& lhs, bool rhs) {
		if (index < 0 || index >= lhs.size()) {
			return invalid_state("out of range (" + std::to_string(index) + ", " + std::to_string(lhs.size()) + ")");
		}
		lhs[index] = rhs;
		return lhs[index];
	}

	OBJECT operator()(auto&, auto&) {
		return invalid_state("failed to assign object");
	}

	int index;
};

struct operate_index_ref_object {
	operate_index_ref_object(int index) :
		index(index)
	{}

	OBJECT operator()(std::vector<float>& value) {
		if (index < 0 || index >= value.size()) {
			return invalid_state("out of range (access: " + std::to_string(index) + ", size: " + std::to_string(value.size()) + ")");
		}
		return float(value[index]);
	}
	OBJECT operator()(std::vector<int>& value) {
		if (index < 0 || index >= value.size()) {
			return invalid_state("out of range (access: " + std::to_string(index) + ", size: " + std::to_string(value.size()) + ")");
		}
		return int(value[index]);
	}
	OBJECT operator()(std::vector<bool>& value) {
		if (index < 0 || index >= value.size()) {
			return invalid_state("out of range (access: " + std::to_string(index) + ", size: " + std::to_string(value.size()) + ")");
		}
		return bool(value[index]);
	}
	OBJECT operator()(std::vector<std::string>& value) {
		if (index < 0 || index >= value.size()) {
			return invalid_state("out of range (access: " + std::to_string(index) + ", size: " + std::to_string(value.size()) + ")");
		}
		return value[index];
	}
	OBJECT operator()(auto) {
		return invalid_state("failed to refer array (index: " + std::to_string(index) + ")");
	}

	int index;
};

struct operate_repeat_object {
	OBJECT operator()(int bgn, int end) noexcept {
		std::vector<int> list;
		list.reserve(std::abs(end - bgn));
		int inc = end > bgn ? 1 : -1;
		for (int i = bgn; i != end; i += inc) {
			list.push_back(i);
		}
		list.push_back(end);
		return list;
	}

	OBJECT operator()(const std::string& bgn, const std::string& end) noexcept {
		if (bgn.size() != 1 || end.size() != 1) {
			return invalid_state("failed to evaluate repeat expression (str should be a charactor)");
		}
		char bgn_c = bgn[0];
		char end_c = end[0];
		std::vector<std::string> list;
		list.reserve(std::abs(end_c - bgn_c));
		char inc = end > bgn ? 1 : -1;
		for (char i = bgn_c; i != end_c; i += inc) {
			list.push_back(i + std::string(""));
		}
		list.push_back(end_c + std::string(""));
		return list;
	}

	OBJECT operator()(auto, auto) noexcept {
		return invalid_state("failed to evaluate repeat expression");
	}
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
	OBJECT operator()(const std::string& lhs, const std::string& rhs) noexcept {
		return lhs + rhs;
	}
	OBJECT operator()(const std::vector<int>& lhs, const std::vector<int>& rhs) {
		if (lhs_index < 0 && rhs_index < 0) {
			std::vector<int> ret = lhs;
			ret.insert(ret.end(), rhs.begin(), rhs.end());
			return ret;
		}
		if (lhs_index < 0) {
			return invalid_state("out of range (access: " + std::to_string(lhs_index) + ")");
		}
		if (rhs_index < 0) {
			return invalid_state("out of range (access: " + std::to_string(lhs_index) + ")");
		}
		if (lhs_index >= lhs.size()) {
			return invalid_state("out of range (access: " + std::to_string(lhs_index) + ", size: " + std::to_string(lhs.size()) + ")");
		}
		if (rhs_index >= rhs.size()) {
			return invalid_state("out of range (access: " + std::to_string(rhs_index) + ", size: " + std::to_string(rhs.size()) + ")");
		}
		return lhs[lhs_index] + rhs[rhs_index];
	}
	OBJECT operator()(const std::vector<int>& lhs, const std::vector<float>& rhs) {
		if (lhs_index < 0 && rhs_index < 0) {
			std::vector<int> ret = lhs;
			ret.reserve(ret.size() + rhs.size());
			for (const float& item : rhs) {
				ret.push_back(static_cast<int>(item));
			}
			return ret;
		}
		if (lhs_index < 0) {
			return invalid_state("out of range (access: " + std::to_string(lhs_index) + ")");
		}
		if (rhs_index < 0) {
			return invalid_state("out of range (access: " + std::to_string(lhs_index) + ")");
		}
		if (lhs_index >= lhs.size()) {
			return invalid_state("out of range (access: " + std::to_string(lhs_index) + ", size: " + std::to_string(lhs.size()) + ")");
		}
		if (rhs_index >= rhs.size()) {
			return invalid_state("out of range (access: " + std::to_string(rhs_index) + ", size: " + std::to_string(rhs.size()) + ")");
		}
		return lhs[lhs_index] + static_cast<int>(rhs[rhs_index]);
	}
	OBJECT operator()(const std::vector<float>& lhs, const std::vector<int>& rhs) {
		if (lhs_index < 0 && rhs_index < 0) {
			std::vector<int> ret;
			ret.reserve(lhs.size() + rhs.size());
			for (const float& item : lhs) {
				ret.push_back(static_cast<int>(item));
			}
			ret.insert(ret.begin(), rhs.begin(), rhs.end());
			return ret;
		}
		if (lhs_index < 0) {
			return invalid_state("out of range (access: " + std::to_string(lhs_index) + ")");
		}
		if (rhs_index < 0) {
			return invalid_state("out of range (access: " + std::to_string(lhs_index) + ")");
		}
		if (lhs_index >= lhs.size()) {
			return invalid_state("out of range (access: " + std::to_string(lhs_index) + ", size: " + std::to_string(lhs.size()) + ")");
		}
		if (rhs_index >= rhs.size()) {
			return invalid_state("out of range (access: " + std::to_string(rhs_index) + ", size: " + std::to_string(rhs.size()) + ")");
		}
		return static_cast<int>(lhs[lhs_index]) + rhs[rhs_index];
	}
	OBJECT operator()(const std::vector<float>& lhs, const std::vector<float>& rhs) {
		if (lhs_index < 0 && rhs_index < 0) {
			std::vector<float> ret = lhs;
			ret.insert(ret.end(), rhs.begin(), rhs.end());
			return ret;
		}
		if (lhs_index < 0) {
			return invalid_state("out of range (access: " + std::to_string(lhs_index) + ")");
		}
		if (rhs_index < 0) {
			return invalid_state("out of range (access: " + std::to_string(lhs_index) + ")");
		}
		if (lhs_index >= lhs.size()) {
			return invalid_state("out of range (access: " + std::to_string(lhs_index) + ", size: " + std::to_string(lhs.size()) + ")");
		}
		if (rhs_index >= rhs.size()) {
			return invalid_state("out of range (access: " + std::to_string(rhs_index) + ", size: " + std::to_string(rhs.size()) + ")");
		}
		return lhs[lhs_index] + rhs[rhs_index];
	}

	OBJECT operator()(const std::vector<std::string>& lhs, const std::vector<std::string>& rhs) {
		if (lhs_index < 0 && rhs_index < 0) {
			std::vector<std::string> ret = lhs;
			ret.insert(ret.end(), rhs.begin(), rhs.end());
			return ret;
		}
		if (lhs_index < 0) {
			return invalid_state("out of range (access: " + std::to_string(lhs_index) + ")");
		}
		if (rhs_index < 0) {
			return invalid_state("out of range (access: " + std::to_string(lhs_index) + ")");
		}
		if (lhs_index >= lhs.size()) {
			return invalid_state("out of range (access: " + std::to_string(lhs_index) + ", size: " + std::to_string(lhs.size()) + ")");
		}
		if (rhs_index >= rhs.size()) {
			return invalid_state("out of range (access: " + std::to_string(rhs_index) + ", size: " + std::to_string(rhs.size()) + ")");
		}
		return lhs[lhs_index] + rhs[rhs_index];
	}

	OBJECT operator()(auto, auto) noexcept {
		return invalid_state("failed to add operation");
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
		if (lhs_index < 0) {
			return invalid_state("out of range (" + std::to_string(lhs_index) + ")");
		}
		if (rhs_index < 0) {
			return invalid_state("out of range (" + std::to_string(lhs_index) + ")");
		}
		if (lhs_index >= lhs.size()) {
			return invalid_state("out of range (" + std::to_string(lhs_index) + ", " + std::to_string(lhs.size()) + ")");
		}
		if (rhs_index >= rhs.size()) {
			return invalid_state("out of range (" + std::to_string(rhs_index) + ", " + std::to_string(rhs.size()) + ")");
		}
		return lhs[lhs_index] - rhs[rhs_index];
	}
	OBJECT operator()(std::vector<int>& lhs, std::vector<float>& rhs) {
		if (lhs_index < 0) {
			return invalid_state("out of range (" + std::to_string(lhs_index) + ")");
		}
		if (rhs_index < 0) {
			return invalid_state("out of range (" + std::to_string(lhs_index) + ")");
		}
		if (lhs_index >= lhs.size()) {
			return invalid_state("out of range (" + std::to_string(lhs_index) + ", " + std::to_string(lhs.size()) + ")");
		}
		if (rhs_index >= rhs.size()) {
			return invalid_state("out of range (" + std::to_string(rhs_index) + ", " + std::to_string(rhs.size()) + ")");
		}
		return lhs[lhs_index] - static_cast<int>(rhs[rhs_index]);
	}
	OBJECT operator()(std::vector<float>& lhs, std::vector<int>& rhs) {
		if (lhs_index < 0) {
			return invalid_state("out of range (" + std::to_string(lhs_index) + ")");
		}
		if (rhs_index < 0) {
			return invalid_state("out of range (" + std::to_string(lhs_index) + ")");
		}
		if (lhs_index >= lhs.size()) {
			return invalid_state("out of range (" + std::to_string(lhs_index) + ", " + std::to_string(lhs.size()) + ")");
		}
		if (rhs_index >= rhs.size()) {
			return invalid_state("out of range (" + std::to_string(rhs_index) + ", " + std::to_string(rhs.size()) + ")");
		}
		return static_cast<int>(lhs[lhs_index]) - rhs[rhs_index];
	}
	OBJECT operator()(std::vector<float>& lhs, std::vector<float>& rhs) {
		if (lhs_index < 0) {
			return invalid_state("out of range (" + std::to_string(lhs_index) + ")");
		}
		if (rhs_index < 0) {
			return invalid_state("out of range (" + std::to_string(lhs_index) + ")");
		}
		if (lhs_index >= lhs.size()) {
			return invalid_state("out of range (" + std::to_string(lhs_index) + ", " + std::to_string(lhs.size()) + ")");
		}
		if (rhs_index >= rhs.size()) {
			return invalid_state("out of range (" + std::to_string(rhs_index) + ", " + std::to_string(rhs.size()) + ")");
		}
		return lhs[lhs_index] - rhs[rhs_index];
	}

	OBJECT operator()(auto, auto) noexcept {
		return invalid_state("failed to substrate operation");
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
		if (lhs_index < 0) {
			return invalid_state("out of range (" + std::to_string(lhs_index) + ")");
		}
		if (rhs_index < 0) {
			return invalid_state("out of range (" + std::to_string(lhs_index) + ")");
		}
		if (lhs_index >= lhs.size()) {
			return invalid_state("out of range (" + std::to_string(lhs_index) + ", " + std::to_string(lhs.size()) + ")");
		}
		if (rhs_index >= rhs.size()) {
			return invalid_state("out of range (" + std::to_string(rhs_index) + ", " + std::to_string(rhs.size()) + ")");
		}
		return lhs[lhs_index] * rhs[rhs_index];
	}
	OBJECT operator()(std::vector<int>& lhs, std::vector<float>& rhs) {
		if (lhs_index < 0) {
			return invalid_state("out of range (" + std::to_string(lhs_index) + ")");
		}
		if (rhs_index < 0) {
			return invalid_state("out of range (" + std::to_string(lhs_index) + ")");
		}
		if (lhs_index >= lhs.size()) {
			return invalid_state("out of range (" + std::to_string(lhs_index) + ", " + std::to_string(lhs.size()) + ")");
		}
		if (rhs_index >= rhs.size()) {
			return invalid_state("out of range (" + std::to_string(rhs_index) + ", " + std::to_string(rhs.size()) + ")");
		}
		return lhs[lhs_index] * static_cast<int>(rhs[rhs_index]);
	}
	OBJECT operator()(std::vector<float>& lhs, std::vector<int>& rhs) {
		if (lhs_index < 0) {
			return invalid_state("out of range (" + std::to_string(lhs_index) + ")");
		}
		if (rhs_index < 0) {
			return invalid_state("out of range (" + std::to_string(lhs_index) + ")");
		}
		if (lhs_index >= lhs.size()) {
			return invalid_state("out of range (" + std::to_string(lhs_index) + ", " + std::to_string(lhs.size()) + ")");
		}
		if (rhs_index >= rhs.size()) {
			return invalid_state("out of range (" + std::to_string(rhs_index) + ", " + std::to_string(rhs.size()) + ")");
		}
		return static_cast<int>(lhs[lhs_index]) * rhs[rhs_index];
	}
	OBJECT operator()(std::vector<float>& lhs, std::vector<float>& rhs) {
		if (lhs_index < 0) {
			return invalid_state("out of range (" + std::to_string(lhs_index) + ")");
		}
		if (rhs_index < 0) {
			return invalid_state("out of range (" + std::to_string(lhs_index) + ")");
		}
		if (lhs_index >= lhs.size()) {
			return invalid_state("out of range (" + std::to_string(lhs_index) + ", " + std::to_string(lhs.size()) + ")");
		}
		if (rhs_index >= rhs.size()) {
			return invalid_state("out of range (" + std::to_string(rhs_index) + ", " + std::to_string(rhs.size()) + ")");
		}
		return lhs[lhs_index] * rhs[rhs_index];
	}

	OBJECT operator()(auto, auto) noexcept {
		return invalid_state("failed to multiply operation");
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
		if (lhs_index < 0) {
			return invalid_state("out of range (" + std::to_string(lhs_index) + ")");
		}
		if (rhs_index < 0) {
			return invalid_state("out of range (" + std::to_string(lhs_index) + ")");
		}
		if (lhs_index >= lhs.size()) {
			return invalid_state("out of range (" + std::to_string(lhs_index) + ", " + std::to_string(lhs.size()) + ")");
		}
		if (rhs_index >= rhs.size()) {
			return invalid_state("out of range (" + std::to_string(rhs_index) + ", " + std::to_string(rhs.size()) + ")");
		}
		return lhs[lhs_index] / rhs[rhs_index];
	}
	OBJECT operator()(std::vector<int>& lhs, std::vector<float>& rhs) {
		if (lhs_index < 0) {
			return invalid_state("out of range (" + std::to_string(lhs_index) + ")");
		}
		if (rhs_index < 0) {
			return invalid_state("out of range (" + std::to_string(lhs_index) + ")");
		}
		if (lhs_index >= lhs.size()) {
			return invalid_state("out of range (" + std::to_string(lhs_index) + ", " + std::to_string(lhs.size()) + ")");
		}
		if (rhs_index >= rhs.size()) {
			return invalid_state("out of range (" + std::to_string(rhs_index) + ", " + std::to_string(rhs.size()) + ")");
		}
		return lhs[lhs_index] / static_cast<int>(rhs[rhs_index]);
	}
	OBJECT operator()(std::vector<float>& lhs, std::vector<int>& rhs) {
		if (lhs_index < 0) {
			return invalid_state("out of range (" + std::to_string(lhs_index) + ")");
		}
		if (rhs_index < 0) {
			return invalid_state("out of range (" + std::to_string(lhs_index) + ")");
		}
		if (lhs_index >= lhs.size()) {
			return invalid_state("out of range (" + std::to_string(lhs_index) + ", " + std::to_string(lhs.size()) + ")");
		}
		if (rhs_index >= rhs.size()) {
			return invalid_state("out of range (" + std::to_string(rhs_index) + ", " + std::to_string(rhs.size()) + ")");
		}
		return static_cast<int>(lhs[lhs_index]) / rhs[rhs_index];
	}
	OBJECT operator()(std::vector<float>& lhs, std::vector<float>& rhs) {
		if (lhs_index < 0) {
			return invalid_state("out of range (" + std::to_string(lhs_index) + ")");
		}
		if (rhs_index < 0) {
			return invalid_state("out of range (" + std::to_string(lhs_index) + ")");
		}
		if (lhs_index >= lhs.size()) {
			return invalid_state("out of range (" + std::to_string(lhs_index) + ", " + std::to_string(lhs.size()) + ")");
		}
		if (rhs_index >= rhs.size()) {
			return invalid_state("out of range (" + std::to_string(rhs_index) + ", " + std::to_string(rhs.size()) + ")");
		}
		return lhs[lhs_index] / rhs[rhs_index];
	}

	OBJECT operator()(auto, auto) noexcept {
		return invalid_state("failed to divide operation");
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
	OBJECT operator()(const std::string& lhs, const std::string& rhs) noexcept {
		return lhs == rhs;
	}

	OBJECT operator()(std::vector<int>& lhs, std::vector<int>& rhs) {
		if (lhs_index < 0) {
			return invalid_state("out of range (" + std::to_string(lhs_index) + ")");
		}
		if (rhs_index < 0) {
			return invalid_state("out of range (" + std::to_string(lhs_index) + ")");
		}
		if (lhs_index >= lhs.size()) {
			return invalid_state("out of range (" + std::to_string(lhs_index) + ", " + std::to_string(lhs.size()) + ")");
		}
		if (rhs_index >= rhs.size()) {
			return invalid_state("out of range (" + std::to_string(rhs_index) + ", " + std::to_string(rhs.size()) + ")");
		}
		return lhs[lhs_index] == rhs[rhs_index];
	}
	OBJECT operator()(std::vector<int>& lhs, std::vector<float>& rhs) {
		if (lhs_index < 0) {
			return invalid_state("out of range (" + std::to_string(lhs_index) + ")");
		}
		if (rhs_index < 0) {
			return invalid_state("out of range (" + std::to_string(lhs_index) + ")");
		}
		if (lhs_index >= lhs.size()) {
			return invalid_state("out of range (" + std::to_string(lhs_index) + ", " + std::to_string(lhs.size()) + ")");
		}
		if (rhs_index >= rhs.size()) {
			return invalid_state("out of range (" + std::to_string(rhs_index) + ", " + std::to_string(rhs.size()) + ")");
		}
		return lhs[lhs_index] == static_cast<int>(rhs[rhs_index]);
	}
	OBJECT operator()(std::vector<float>& lhs, std::vector<int>& rhs) {
		if (lhs_index < 0) {
			return invalid_state("out of range (" + std::to_string(lhs_index) + ")");
		}
		if (rhs_index < 0) {
			return invalid_state("out of range (" + std::to_string(lhs_index) + ")");
		}
		if (lhs_index >= lhs.size()) {
			return invalid_state("out of range (" + std::to_string(lhs_index) + ", " + std::to_string(lhs.size()) + ")");
		}
		if (rhs_index >= rhs.size()) {
			return invalid_state("out of range (" + std::to_string(rhs_index) + ", " + std::to_string(rhs.size()) + ")");
		}
		return static_cast<int>(lhs[lhs_index]) == rhs[rhs_index];
	}
	OBJECT operator()(std::vector<float>& lhs, std::vector<float>& rhs) {
		if (lhs_index < 0) {
			return invalid_state("out of range (" + std::to_string(lhs_index) + ")");
		}
		if (rhs_index < 0) {
			return invalid_state("out of range (" + std::to_string(lhs_index) + ")");
		}
		if (lhs_index >= lhs.size()) {
			return invalid_state("out of range (" + std::to_string(lhs_index) + ", " + std::to_string(lhs.size()) + ")");
		}
		if (rhs_index >= rhs.size()) {
			return invalid_state("out of range (" + std::to_string(rhs_index) + ", " + std::to_string(rhs.size()) + ")");
		}
		return lhs[lhs_index] == rhs[rhs_index];
	}
	OBJECT operator()(std::vector<std::string>& lhs, std::vector<std::string>& rhs) {
		if (lhs_index < 0) {
			return invalid_state("out of range (" + std::to_string(lhs_index) + ")");
		}
		if (rhs_index < 0) {
			return invalid_state("out of range (" + std::to_string(lhs_index) + ")");
		}
		if (lhs_index >= lhs.size()) {
			return invalid_state("out of range (" + std::to_string(lhs_index) + ", " + std::to_string(lhs.size()) + ")");
		}
		if (rhs_index >= rhs.size()) {
			return invalid_state("out of range (" + std::to_string(rhs_index) + ", " + std::to_string(rhs.size()) + ")");
		}
		return lhs[lhs_index] == rhs[rhs_index];
	}

	OBJECT operator()(auto, auto) noexcept {
		return invalid_state("failed to equality operation");
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
	OBJECT operator()(const std::string& lhs, const std::string& rhs) noexcept {
		return lhs != rhs;
	}

	OBJECT operator()(const std::vector<int>& lhs, const std::vector<int>& rhs) {
		if (lhs_index < 0) {
			return invalid_state("out of range (" + std::to_string(lhs_index) + ")");
		}
		if (rhs_index < 0) {
			return invalid_state("out of range (" + std::to_string(lhs_index) + ")");
		}
		if (lhs_index >= lhs.size()) {
			return invalid_state("out of range (" + std::to_string(lhs_index) + ", " + std::to_string(lhs.size()) + ")");
		}
		if (rhs_index >= rhs.size()) {
			return invalid_state("out of range (" + std::to_string(rhs_index) + ", " + std::to_string(rhs.size()) + ")");
		}
		return lhs[lhs_index] != rhs[rhs_index];
	}
	OBJECT operator()(const std::vector<int>& lhs, const std::vector<float>& rhs) {
		if (lhs_index < 0) {
			return invalid_state("out of range (" + std::to_string(lhs_index) + ")");
		}
		if (rhs_index < 0) {
			return invalid_state("out of range (" + std::to_string(lhs_index) + ")");
		}
		if (lhs_index >= lhs.size()) {
			return invalid_state("out of range (" + std::to_string(lhs_index) + ", " + std::to_string(lhs.size()) + ")");
		}
		if (rhs_index >= rhs.size()) {
			return invalid_state("out of range (" + std::to_string(rhs_index) + ", " + std::to_string(rhs.size()) + ")");
		}
		return lhs[lhs_index] != static_cast<int>(rhs[rhs_index]);
	}
	OBJECT operator()(const std::vector<float>& lhs, const std::vector<int>& rhs) {
		if (lhs_index < 0) {
			return invalid_state("out of range (" + std::to_string(lhs_index) + ")");
		}
		if (rhs_index < 0) {
			return invalid_state("out of range (" + std::to_string(lhs_index) + ")");
		}
		if (lhs_index >= lhs.size()) {
			return invalid_state("out of range (" + std::to_string(lhs_index) + ", " + std::to_string(lhs.size()) + ")");
		}
		if (rhs_index >= rhs.size()) {
			return invalid_state("out of range (" + std::to_string(rhs_index) + ", " + std::to_string(rhs.size()) + ")");
		}
		return static_cast<int>(lhs[lhs_index]) != rhs[rhs_index];
	}
	OBJECT operator()(const std::vector<float>& lhs, const std::vector<float>& rhs) {
		if (lhs_index < 0) {
			return invalid_state("out of range (" + std::to_string(lhs_index) + ")");
		}
		if (rhs_index < 0) {
			return invalid_state("out of range (" + std::to_string(lhs_index) + ")");
		}
		if (lhs_index >= lhs.size()) {
			return invalid_state("out of range (" + std::to_string(lhs_index) + ", " + std::to_string(lhs.size()) + ")");
		}
		if (rhs_index >= rhs.size()) {
			return invalid_state("out of range (" + std::to_string(rhs_index) + ", " + std::to_string(rhs.size()) + ")");
		}
		return lhs[lhs_index] != rhs[rhs_index];
	}
	OBJECT operator()(const std::vector<std::string>& lhs, const std::vector<std::string>& rhs) {
		if (lhs_index < 0) {
			return invalid_state("out of range (" + std::to_string(lhs_index) + ")");
		}
		if (rhs_index < 0) {
			return invalid_state("out of range (" + std::to_string(lhs_index) + ")");
		}
		if (lhs_index >= lhs.size()) {
			return invalid_state("out of range (" + std::to_string(lhs_index) + ", " + std::to_string(lhs.size()) + ")");
		}
		if (rhs_index >= rhs.size()) {
			return invalid_state("out of range (" + std::to_string(rhs_index) + ", " + std::to_string(rhs.size()) + ")");
		}
		return lhs[lhs_index] != rhs[rhs_index];
	}

	OBJECT operator()(auto, auto) noexcept {
		return invalid_state("failed to not equality operation");
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
	OBJECT operator()(const std::string& lhs, const std::string& rhs) noexcept {
		return lhs < rhs;
	}

	OBJECT operator()(std::vector<int>& lhs, std::vector<int>& rhs) {
		if (lhs_index < 0) {
			return invalid_state("out of range (" + std::to_string(lhs_index) + ")");
		}
		if (rhs_index < 0) {
			return invalid_state("out of range (" + std::to_string(lhs_index) + ")");
		}
		if (lhs_index >= lhs.size()) {
			return invalid_state("out of range (" + std::to_string(lhs_index) + ", " + std::to_string(lhs.size()) + ")");
		}
		if (rhs_index >= rhs.size()) {
			return invalid_state("out of range (" + std::to_string(rhs_index) + ", " + std::to_string(rhs.size()) + ")");
		}
		return lhs[lhs_index] < rhs[rhs_index];
	}
	OBJECT operator()(std::vector<int>& lhs, std::vector<float>& rhs) {
		if (lhs_index < 0) {
			return invalid_state("out of range (" + std::to_string(lhs_index) + ")");
		}
		if (rhs_index < 0) {
			return invalid_state("out of range (" + std::to_string(lhs_index) + ")");
		}
		if (lhs_index >= lhs.size()) {
			return invalid_state("out of range (" + std::to_string(lhs_index) + ", " + std::to_string(lhs.size()) + ")");
		}
		if (rhs_index >= rhs.size()) {
			return invalid_state("out of range (" + std::to_string(rhs_index) + ", " + std::to_string(rhs.size()) + ")");
		}
		return lhs[lhs_index] < static_cast<int>(rhs[rhs_index]);
	}
	OBJECT operator()(std::vector<float>& lhs, std::vector<int>& rhs) {
		if (lhs_index < 0) {
			return invalid_state("out of range (" + std::to_string(lhs_index) + ")");
		}
		if (rhs_index < 0) {
			return invalid_state("out of range (" + std::to_string(lhs_index) + ")");
		}
		if (lhs_index >= lhs.size()) {
			return invalid_state("out of range (" + std::to_string(lhs_index) + ", " + std::to_string(lhs.size()) + ")");
		}
		if (rhs_index >= rhs.size()) {
			return invalid_state("out of range (" + std::to_string(rhs_index) + ", " + std::to_string(rhs.size()) + ")");
		}
		return static_cast<int>(lhs[lhs_index]) < rhs[rhs_index];
	}
	OBJECT operator()(std::vector<float>& lhs, std::vector<float>& rhs) {
		if (lhs_index < 0) {
			return invalid_state("out of range (" + std::to_string(lhs_index) + ")");
		}
		if (rhs_index < 0) {
			return invalid_state("out of range (" + std::to_string(lhs_index) + ")");
		}
		if (lhs_index >= lhs.size()) {
			return invalid_state("out of range (" + std::to_string(lhs_index) + ", " + std::to_string(lhs.size()) + ")");
		}
		if (rhs_index >= rhs.size()) {
			return invalid_state("out of range (" + std::to_string(rhs_index) + ", " + std::to_string(rhs.size()) + ")");
		}
		return lhs[lhs_index] < rhs[rhs_index];
	}
	OBJECT operator()(std::vector<std::string>& lhs, std::vector<std::string>& rhs) {
		if (lhs_index < 0) {
			return invalid_state("out of range (" + std::to_string(lhs_index) + ")");
		}
		if (rhs_index < 0) {
			return invalid_state("out of range (" + std::to_string(lhs_index) + ")");
		}
		if (lhs_index >= lhs.size()) {
			return invalid_state("out of range (" + std::to_string(lhs_index) + ", " + std::to_string(lhs.size()) + ")");
		}
		if (rhs_index >= rhs.size()) {
			return invalid_state("out of range (" + std::to_string(rhs_index) + ", " + std::to_string(rhs.size()) + ")");
		}
		return lhs[lhs_index] < rhs[rhs_index];
	}

	OBJECT operator()(auto, auto) noexcept {
		return invalid_state("failed to less than operation");
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
	OBJECT operator()(const std::string& lhs, const std::string& rhs) noexcept {
		return lhs <= rhs;
	}

	OBJECT operator()(const std::vector<int>& lhs, const std::vector<int>& rhs) {
		if (lhs_index < 0) {
			return invalid_state("out of range (" + std::to_string(lhs_index) + ")");
		}
		if (rhs_index < 0) {
			return invalid_state("out of range (" + std::to_string(lhs_index) + ")");
		}
		if (lhs_index >= lhs.size()) {
			return invalid_state("out of range (" + std::to_string(lhs_index) + ", " + std::to_string(lhs.size()) + ")");
		}
		if (rhs_index >= rhs.size()) {
			return invalid_state("out of range (" + std::to_string(rhs_index) + ", " + std::to_string(rhs.size()) + ")");
		}
		return lhs[lhs_index] <= rhs[rhs_index];
	}
	OBJECT operator()(const std::vector<int>& lhs, const std::vector<float>& rhs) {
		if (lhs_index < 0) {
			return invalid_state("out of range (" + std::to_string(lhs_index) + ")");
		}
		if (rhs_index < 0) {
			return invalid_state("out of range (" + std::to_string(lhs_index) + ")");
		}
		if (lhs_index >= lhs.size()) {
			return invalid_state("out of range (" + std::to_string(lhs_index) + ", " + std::to_string(lhs.size()) + ")");
		}
		if (rhs_index >= rhs.size()) {
			return invalid_state("out of range (" + std::to_string(rhs_index) + ", " + std::to_string(rhs.size()) + ")");
		}
		return lhs[lhs_index] <= static_cast<int>(rhs[rhs_index]);
	}
	OBJECT operator()(const std::vector<float>& lhs, const std::vector<int>& rhs) {
		if (lhs_index < 0) {
			return invalid_state("out of range (" + std::to_string(lhs_index) + ")");
		}
		if (rhs_index < 0) {
			return invalid_state("out of range (" + std::to_string(lhs_index) + ")");
		}
		if (lhs_index >= lhs.size()) {
			return invalid_state("out of range (" + std::to_string(lhs_index) + ", " + std::to_string(lhs.size()) + ")");
		}
		if (rhs_index >= rhs.size()) {
			return invalid_state("out of range (" + std::to_string(rhs_index) + ", " + std::to_string(rhs.size()) + ")");
		}
		return static_cast<int>(lhs[lhs_index]) <= rhs[rhs_index];
	}
	OBJECT operator()(const std::vector<float>& lhs, const std::vector<float>& rhs) {
		if (lhs_index < 0) {
			return invalid_state("out of range (" + std::to_string(lhs_index) + ")");
		}
		if (rhs_index < 0) {
			return invalid_state("out of range (" + std::to_string(lhs_index) + ")");
		}
		if (lhs_index >= lhs.size()) {
			return invalid_state("out of range (" + std::to_string(lhs_index) + ", " + std::to_string(lhs.size()) + ")");
		}
		if (rhs_index >= rhs.size()) {
			return invalid_state("out of range (" + std::to_string(rhs_index) + ", " + std::to_string(rhs.size()) + ")");
		}
		return lhs[lhs_index] <= rhs[rhs_index];
	}
	OBJECT operator()(const std::vector<std::string>& lhs, const std::vector<std::string>& rhs) {
		if (lhs_index < 0) {
			return invalid_state("out of range (" + std::to_string(lhs_index) + ")");
		}
		if (rhs_index < 0) {
			return invalid_state("out of range (" + std::to_string(lhs_index) + ")");
		}
		if (lhs_index >= lhs.size()) {
			return invalid_state("out of range (" + std::to_string(lhs_index) + ", " + std::to_string(lhs.size()) + ")");
		}
		if (rhs_index >= rhs.size()) {
			return invalid_state("out of range (" + std::to_string(rhs_index) + ", " + std::to_string(rhs.size()) + ")");
		}
		return lhs[lhs_index] <= rhs[rhs_index];
	}

	OBJECT operator()(auto, auto) noexcept {
		return invalid_state("failed to less than or equal operation");
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
	OBJECT operator()(const std::string& lhs, const std::string& rhs) noexcept {
		return lhs > rhs;
	}

	OBJECT operator()(const std::vector<int>& lhs, const std::vector<int>& rhs) {
		if (lhs_index < 0) {
			return invalid_state("out of range (" + std::to_string(lhs_index) + ")");
		}
		if (rhs_index < 0) {
			return invalid_state("out of range (" + std::to_string(lhs_index) + ")");
		}
		if (lhs_index >= lhs.size()) {
			return invalid_state("out of range (" + std::to_string(lhs_index) + ", " + std::to_string(lhs.size()) + ")");
		}
		if (rhs_index >= rhs.size()) {
			return invalid_state("out of range (" + std::to_string(rhs_index) + ", " + std::to_string(rhs.size()) + ")");
		}
		return lhs[lhs_index] > rhs[rhs_index];
	}
	OBJECT operator()(const std::vector<int>& lhs, const std::vector<float>& rhs) {
		if (lhs_index < 0) {
			return invalid_state("out of range (" + std::to_string(lhs_index) + ")");
		}
		if (rhs_index < 0) {
			return invalid_state("out of range (" + std::to_string(lhs_index) + ")");
		}
		if (lhs_index >= lhs.size()) {
			return invalid_state("out of range (" + std::to_string(lhs_index) + ", " + std::to_string(lhs.size()) + ")");
		}
		if (rhs_index >= rhs.size()) {
			return invalid_state("out of range (" + std::to_string(rhs_index) + ", " + std::to_string(rhs.size()) + ")");
		}
		return lhs[lhs_index] > static_cast<int>(rhs[rhs_index]);
	}
	OBJECT operator()(const std::vector<float>& lhs, const std::vector<int>& rhs) {
		if (lhs_index < 0) {
			return invalid_state("out of range (" + std::to_string(lhs_index) + ")");
		}
		if (rhs_index < 0) {
			return invalid_state("out of range (" + std::to_string(lhs_index) + ")");
		}
		if (lhs_index >= lhs.size()) {
			return invalid_state("out of range (" + std::to_string(lhs_index) + ", " + std::to_string(lhs.size()) + ")");
		}
		if (rhs_index >= rhs.size()) {
			return invalid_state("out of range (" + std::to_string(rhs_index) + ", " + std::to_string(rhs.size()) + ")");
		}
		return static_cast<int>(lhs[lhs_index]) > rhs[rhs_index];
	}
	OBJECT operator()(const std::vector<float>& lhs, const std::vector<float>& rhs) {
		if (lhs_index < 0) {
			return invalid_state("out of range (" + std::to_string(lhs_index) + ")");
		}
		if (rhs_index < 0) {
			return invalid_state("out of range (" + std::to_string(lhs_index) + ")");
		}
		if (lhs_index >= lhs.size()) {
			return invalid_state("out of range (" + std::to_string(lhs_index) + ", " + std::to_string(lhs.size()) + ")");
		}
		if (rhs_index >= rhs.size()) {
			return invalid_state("out of range (" + std::to_string(rhs_index) + ", " + std::to_string(rhs.size()) + ")");
		}
		return lhs[lhs_index] > rhs[rhs_index];
	}
	OBJECT operator()(const std::vector<std::string>& lhs, const std::vector<std::string>& rhs) {
		if (lhs_index < 0) {
			return invalid_state("out of range (" + std::to_string(lhs_index) + ")");
		}
		if (rhs_index < 0) {
			return invalid_state("out of range (" + std::to_string(lhs_index) + ")");
		}
		if (lhs_index >= lhs.size()) {
			return invalid_state("out of range (" + std::to_string(lhs_index) + ", " + std::to_string(lhs.size()) + ")");
		}
		if (rhs_index >= rhs.size()) {
			return invalid_state("out of range (" + std::to_string(rhs_index) + ", " + std::to_string(rhs.size()) + ")");
		}
		return lhs[lhs_index] > rhs[rhs_index];
	}

	OBJECT operator()(auto, auto) noexcept {
		return invalid_state("failed to greater than operation");
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
	OBJECT operator()(const std::string& lhs, const std::string& rhs) noexcept {
		return lhs >= rhs;
	}

	OBJECT operator()(const std::vector<int>& lhs, const std::vector<int>& rhs) {
		if (lhs_index < 0) {
			return invalid_state("out of range (" + std::to_string(lhs_index) + ")");
		}
		if (rhs_index < 0) {
			return invalid_state("out of range (" + std::to_string(lhs_index) + ")");
		}
		if (lhs_index >= lhs.size()) {
			return invalid_state("out of range (" + std::to_string(lhs_index) + ", " + std::to_string(lhs.size()) + ")");
		}
		if (rhs_index >= rhs.size()) {
			return invalid_state("out of range (" + std::to_string(rhs_index) + ", " + std::to_string(rhs.size()) + ")");
		}
		return lhs[lhs_index] >= rhs[rhs_index];
	}
	OBJECT operator()(const std::vector<int>& lhs, const std::vector<float>& rhs) {
		if (lhs_index < 0) {
			return invalid_state("out of range (" + std::to_string(lhs_index) + ")");
		}
		if (rhs_index < 0) {
			return invalid_state("out of range (" + std::to_string(lhs_index) + ")");
		}
		if (lhs_index >= lhs.size()) {
			return invalid_state("out of range (" + std::to_string(lhs_index) + ", " + std::to_string(lhs.size()) + ")");
		}
		if (rhs_index >= rhs.size()) {
			return invalid_state("out of range (" + std::to_string(rhs_index) + ", " + std::to_string(rhs.size()) + ")");
		}
		return lhs[lhs_index] >= static_cast<int>(rhs[rhs_index]);
	}
	OBJECT operator()(const std::vector<float>& lhs, const std::vector<int>& rhs) {
		if (lhs_index < 0) {
			return invalid_state("out of range (" + std::to_string(lhs_index) + ")");
		}
		if (rhs_index < 0) {
			return invalid_state("out of range (" + std::to_string(lhs_index) + ")");
		}
		if (lhs_index >= lhs.size()) {
			return invalid_state("out of range (" + std::to_string(lhs_index) + ", " + std::to_string(lhs.size()) + ")");
		}
		if (rhs_index >= rhs.size()) {
			return invalid_state("out of range (" + std::to_string(rhs_index) + ", " + std::to_string(rhs.size()) + ")");
		}
		return static_cast<int>(lhs[lhs_index]) >= rhs[rhs_index];
	}
	OBJECT operator()(const std::vector<float>& lhs, const std::vector<float>& rhs) {
		if (lhs_index < 0) {
			return invalid_state("out of range (" + std::to_string(lhs_index) + ")");
		}
		if (rhs_index < 0) {
			return invalid_state("out of range (" + std::to_string(lhs_index) + ")");
		}
		if (lhs_index >= lhs.size()) {
			return invalid_state("out of range (" + std::to_string(lhs_index) + ", " + std::to_string(lhs.size()) + ")");
		}
		if (rhs_index >= rhs.size()) {
			return invalid_state("out of range (" + std::to_string(rhs_index) + ", " + std::to_string(rhs.size()) + ")");
		}
		return lhs[lhs_index] >= rhs[rhs_index];
	}
	OBJECT operator()(const std::vector<std::string>& lhs, const std::vector<std::string>& rhs) {
		if (lhs_index < 0) {
			return invalid_state("out of range (" + std::to_string(lhs_index) + ")");
		}
		if (rhs_index < 0) {
			return invalid_state("out of range (" + std::to_string(lhs_index) + ")");
		}
		if (lhs_index >= lhs.size()) {
			return invalid_state("out of range (" + std::to_string(lhs_index) + ", " + std::to_string(lhs.size()) + ")");
		}
		if (rhs_index >= rhs.size()) {
			return invalid_state("out of range (" + std::to_string(rhs_index) + ", " + std::to_string(rhs.size()) + ")");
		}
		return lhs[lhs_index] >= rhs[rhs_index];
	}

	OBJECT operator()(auto, auto) noexcept {
		return invalid_state("failed to greater than or equal operation");
	}

	int lhs_index;
	int rhs_index;
};
