#pragma once

#define DECL_VAR_GETTER_SETTER(type, name) \
 private: \
	type _##name; \
\
 public: \
	type get##name() const; \
	void set##name(const type &name);

#define DECL_VAR_REF_GETTER_SETTER(type, name) \
 private: \
	type _##name; \
\
 public: \
	const type &get##name() const; \
	void set##name(const type &name);

#define IMPL_GETTER_SETTER_NS(type, name, ns) \
	type ns get##name() const { return this->_##name; } \
	void ns set##name(const type &name) { this->_##name = name; }

#define IMPL_REF_GETTER_SETTER_NS(type, name, ns) \
	const type &ns get##name() const { return this->_##name; } \
	void ns set##name(const type &name) { this->_##name = name; }
