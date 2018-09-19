#pragma once

#include <exception>
#include <unordered_map>
#include <lua.hpp>

#include "common.h"

class LuaException : public std::runtime_error {
public:
	LuaException(const std::string& what) : std::runtime_error(what) {}
	LuaException(const char* what) : std::runtime_error(what) {}
};

class LuaScript {
public:
	class StackScope {
	public:
		class ScopeDescriptor {
		public:
			ScopeDescriptor() : _scope() {}
			ScopeDescriptor(std::vector<std::string>&& scope) : _scope(std::move(scope)) {}
			ScopeDescriptor(std::initializer_list<std::string> ilist) : _scope(ilist) {}
			ScopeDescriptor(std::string&& str) { _scope.emplace_back(std::move(str)); }
			ScopeDescriptor(const char* str) { _scope.emplace_back(str); }
			ScopeDescriptor(int i) : _scope({std::to_string(i)}) {}

			ScopeDescriptor(const ScopeDescriptor& other) = delete;
			ScopeDescriptor(ScopeDescriptor&&) = default;
			ScopeDescriptor& operator=(const ScopeDescriptor&) = delete;
			ScopeDescriptor& operator=(ScopeDescriptor&&) = default;

			const std::string& back() const { return _scope.back(); }
			std::string toString() const {
				std::string res;
				for(const auto& str : _scope) res += str;
				return res;
			}
			operator std::string() const { return toString(); }

		private:
			friend class StackScope;
			std::vector<std::string> _scope;
		};

		StackScope(lua_State* state, const ScopeDescriptor& scopeDescriptor);
		StackScope(const StackScope&) = delete;
		StackScope(StackScope&& old) : L(old.L), savedLevel(old.savedLevel) { old.L = nullptr; }
		StackScope& operator=(const StackScope&) = delete;
		StackScope& operator=(StackScope&&) = delete;

		~StackScope() { if(L != nullptr) lua_settop(L, savedLevel); }

		void clearStack() { lua_settop(L, 0); }
	private:
		lua_State * L;
		int savedLevel;
	};

	LuaScript(const std::string& filename, std::unordered_map<std::string, int>&& constants = std::unordered_map<std::string, int>());
	LuaScript(const LuaScript& other) = delete;
	LuaScript& operator=(const LuaScript& other) = delete;

	~LuaScript() {
		if(L) lua_close(L);
	}

	StackScope getScope(const StackScope::ScopeDescriptor& scope) const {
		StackScope scp(L, scope);
		if(lua_isnil(L, -1)) throw LuaException(scope.toString() + " is an invalid scope");
		return scp;
	}

	template <typename T>
	T get(const StackScope::ScopeDescriptor& scope = StackScope::ScopeDescriptor()) {
		StackScope ss(L, scope);
		if(lua_isnil(L, -1)) throw LuaException(scope.toString() + " is not defined");

		try {
			return _get<T>();
		} catch(const LuaException& e) {
			throw LuaException(scope.toString() + ": " + e.what());
		}
	}

	int getLength(const StackScope::ScopeDescriptor& scope = StackScope::ScopeDescriptor()) {
		StackScope ss(L, scope);
		if(!lua_istable(L, -1)) throw LuaException(scope.toString() + " is not a table, length is undefined");
		lua_len(L, -1);
		return (int)lua_tointeger(L, -1);
	}

	bool has(const StackScope::ScopeDescriptor& scope = StackScope::ScopeDescriptor()) {
		StackScope ss(L, scope);
		return !lua_isnil(L, -1);
	}
	bool hasTrue(const StackScope::ScopeDescriptor& scope = StackScope::ScopeDescriptor()) {
		StackScope ss(L, scope);
		return lua_isboolean(L, -1) && lua_toboolean(L, -1);
	}

	template <typename T>
	void set(const StackScope::ScopeDescriptor& scope, T value) {
		StackScope ss(L, scope);
		lua_pop(L, 1);
		_set(value);

		if(lua_gettop(L) == 1) {
			lua_setglobal(L, scope.back().c_str());
		} else {
			lua_setfield(L, -2, scope.back().c_str());
		}
	}

	template <typename KeyT, typename ValueT>
	std::unordered_map<KeyT, ValueT> get(const StackScope::ScopeDescriptor& scope = StackScope::ScopeDescriptor()) {
		std::unordered_map<KeyT, ValueT> res;
		StackScope ss(L, scope);
		if(lua_isnil(L, -1)) throw LuaException(StackScope::getScopeString(scope) + " is not defined");

		lua_pushnil(L);
		while(lua_next(L, -2) != 0) {
			auto v = _get<ValueT>();
			lua_pop(L, 1);
			res.emplace(_get<KeyT>(), v);
		}
		return res;
	}

private:
	template <typename T>
	inline T _get();

	template <typename T>
	inline void _set(T value);

	lua_State* L = nullptr;
};

template <>
inline bool LuaScript::_get() {
	if(!lua_isboolean(L, -1)) throw LuaException("not a boolean");
	return lua_toboolean(L, -1);
}

template <>
inline lua_Number LuaScript::_get() {
	if(!lua_isnumber(L, -1)) throw LuaException("not a number");
	return lua_tonumber(L, -1);
}

template <>
inline float LuaScript::_get() {
	return (float)_get<lua_Number>();
}

template <>
inline lua_Integer LuaScript::_get() {
	if(!lua_isinteger(L, -1)) throw LuaException("not an integer");
	return lua_tointeger(L, -1);
}

template <>
inline int LuaScript::_get() {
	return (int)_get<lua_Integer>();
}

template <>
inline std::string LuaScript::_get() {
	if(!lua_isstring(L, -1)) throw LuaException("not a string");
	return std::string(lua_tostring(L, -1));
}


template <>
inline void LuaScript::_set(bool value) {
	lua_pushboolean(L, value);
}

template <>
inline void LuaScript::_set(lua_Number value) {
	lua_pushnumber(L, value);
}

template <>
inline void LuaScript::_set(float value) {
	_set((lua_Number)value);
}

template <>
inline void LuaScript::_set(lua_Integer value) {
	lua_pushinteger(L, value);
}

template <>
inline void LuaScript::_set(int value) {
	_set((lua_Integer)value);
}

template <>
inline void LuaScript::_set(std::string value) {
	lua_pushstring(L, value.c_str());
}

