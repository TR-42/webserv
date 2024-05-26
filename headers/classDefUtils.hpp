#pragma once

#define DECL_VAR_GETTER(type, name) \
 private: \
	type _##name; \
\
 public: \
	inline type get##name() const \
	{ \
		return this->_##name; \
	}

#define DECL_VAR_GETTER_SETTER(type, name) \
 private: \
	type _##name; \
\
 public: \
	inline type get##name() const \
	{ \
		return this->_##name; \
	} \
	inline void set##name(const type &name) \
	{ \
		this->_##name = name; \
	}

#define DECL_VAR_REF_GETTER(type, name) \
 private: \
	type _##name; \
\
 public: \
	const type &get##name() const;

#define DECL_VAR_REF_GETTER_SETTER(type, name) \
 private: \
	type _##name; \
\
 public: \
	const inline type &get##name() const \
	{ \
		return this->_##name; \
	} \
	inline void set##name(const type &name) \
	{ \
		this->_##name = name; \
	}

#define DECL_VAR_REF_NO_CONST_GETTER_SETTER(type, name) \
 private: \
	type _##name; \
\
 public: \
	inline type &get##name() \
	{ \
		return this->_##name; \
	} \
	inline void set##name(const type &name) \
	{ \
		this->_##name = name; \
	}
