#pragma once
#include <type_traits>
#include <exception>

namespace variant {
namespace utils {
template <typename First, typename... Rest>
struct GetMaxSize {
	static constexpr size_t _first_size = sizeof(First);
	static constexpr size_t _second_size = GetMaxSize<Rest...>::max_size;
	static constexpr size_t max_size = _first_size > _second_size ? _first_size : _second_size;
};

template <typename First>
struct GetMaxSize<First> {
	static constexpr size_t _first_size = sizeof(First);
	static constexpr size_t max_size = sizeof(First);
};

template <typename... Rest>
struct GetMaxSize<void, Rest...> {
	static constexpr size_t _first_size = 0;
	static constexpr size_t _second_size = GetMaxSize<Rest...>::_first_size;
	static constexpr size_t max_size = _first_size > _second_size ? _first_size : _second_size;
};

template <typename First, typename... Rest>
struct GetIndex {
	using first_type = First;
	static constexpr size_t getIndex(size_t current = 0) {
		return std::is_same<First, GetIndex<Rest...>::first_type>::value ? current : GetIndex<Rest...>::getIndex(current + 1);
	}
	static constexpr size_t index = getIndex();
};

template <typename T>
struct GetIndex<T> {
	using first_type = T;
	static constexpr size_t getIndex(size_t current) {
		return current;
	}
};

template <typename... Ts>
struct ContainsT : ContainsT<Ts...> {};

template <typename T, typename U, typename... Rest>
struct ContainsT<typename T, typename U, Rest...> {
	using first_type = T;
	using second_type = U;
	static constexpr bool containsT() {
		return std::is_same<first_type, second_type>::value ? true : ContainsT<first_type, Rest...>::containsT();
	}
	static constexpr bool value = containsT();
};

template <typename T>
struct ContainsT<T> {
	using first_type = T;
	using second_type = T*;
	static constexpr bool containsT() {
		return false;
	}
};

template <typename... Ts>
struct VariantHolder {
	static constexpr size_t max_size = GetMaxSize<Ts...>::max_size;
	char storage[max_size] = { '\0' };
	size_t current_index = -1;
};

}

namespace detail {
inline namespace v1 {

template <typename... Ts>
struct Variant : utils::VariantHolder<Ts...> {
	template <typename T>
	Variant(T&& t) {
		static_assert(utils::ContainsT<T, Ts...>::value, "Variant does not contains the given type");
		*reinterpret_cast<T*>(storage) = t;
		current_index = index<T>();
	}
	Variant() = default;

	template <typename T>
	bool valid() const {
		return utils::GetIndex<T, Ts...>::index == current_index;
	}

	template <typename T>
	size_t index() const {
		return utils::GetIndex<T, Ts...>::index;
	}
};

template <typename T, typename... Types>
T get(const Variant<Types...>& v) {
	if (!v.valid<T>()) {
		throw std::logic_error("Got invalid type for variant");
	}
	return *reinterpret_cast<const T*>(v.storage);
}

}
}
}