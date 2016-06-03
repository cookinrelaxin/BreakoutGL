//
// Copyright (c) 2013-2016 Pavel Medvedev. All rights reserved.
//
// This file is part of v8pp (https://github.com/pmed/v8pp) project.
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef V8PP_MODULE_HPP_INCLUDED
#define V8PP_MODULE_HPP_INCLUDED

#include <v8.h>

#include "v8pp/config.hpp"
#include "v8pp/function.hpp"
#include "v8pp/property.hpp"

namespace v8pp {

template<typename T>
class class_;

/// Module (similar to v8::ObjectTemplate)
class module
{
public:
	explicit module(v8::Isolate* isolate)
		: isolate_(isolate)
		, obj_(v8::ObjectTemplate::New(isolate))
	{
	}

	explicit module(v8::Isolate* isolate, v8::Handle<v8::ObjectTemplate> obj)
		: isolate_(isolate)
		, obj_(obj)
	{
	}

	/// v8::Isolate where the module belongs
	v8::Isolate* isolate() { return isolate_; }

	/// Set a V8 value in the module with specified name
	template<typename Data>
	module& set(char const* name, v8::Handle<Data> value)
	{
		obj_->Set(v8pp::to_v8(isolate_, name), value);
		return *this;
	}

	/// Set another module in the module with specified name
	module& set(char const* name, module& m)
	{
		return set(name, m.obj_);
	}

	/// Set wrapped C++ class in the module with specified name
	template<typename T>
	module& set(char const* name, class_<T>& cl)
	{
		v8::HandleScope scope(isolate_);

		cl.class_function_template()->SetClassName(v8pp::to_v8(isolate_, name));
		return set(name, cl.js_function_template());
	}

	/// Set a C++ function in the module with specified name
	template<typename Function>
	typename std::enable_if<detail::is_callable<Function>::value, module&>::type
	set(char const* name, Function func)
	{
		return set(name, wrap_function_template(isolate_, func));
	}

	/// Set a C++ variable in the module with specified name
	template<typename Variable>
	typename std::enable_if<!detail::is_callable<Variable>::value, module&>::type
	set(char const *name, Variable& var, bool readonly = false)
	{
		v8::HandleScope scope(isolate_);

		v8::AccessorGetterCallback getter = &var_get<Variable>;
		v8::AccessorSetterCallback setter = &var_set<Variable>;
		if (readonly)
		{
			setter = nullptr;
		}

		v8::Handle<v8::Value> data = detail::set_external_data(isolate_, &var);
		v8::PropertyAttribute const prop_attrs = v8::PropertyAttribute(v8::DontDelete | (setter ? 0 : v8::ReadOnly));

		obj_->SetAccessor(v8pp::to_v8(isolate_, name), getter, setter, data, v8::DEFAULT, prop_attrs);
		return *this;
	}

	/// Set v8pp::property in the module with specified name
	template<typename GetFunction, typename SetFunction>
	module& set(char const *name, property_<GetFunction, SetFunction> prop)
	{
		v8::HandleScope scope(isolate_);

		v8::AccessorGetterCallback getter = property_<GetFunction, SetFunction>::get;
		v8::AccessorSetterCallback setter = property_<GetFunction, SetFunction>::set;
		if (property_<GetFunction, SetFunction>::is_readonly)
		{
			setter = nullptr;
		}

		v8::Handle<v8::Value> data = detail::set_external_data(isolate_, prop);
		v8::PropertyAttribute const prop_attrs = v8::PropertyAttribute(v8::DontDelete | (setter? 0 : v8::ReadOnly));

		obj_->SetAccessor(v8pp::to_v8(isolate_, name), getter, setter, data, v8::DEFAULT, prop_attrs);
		return *this;
	}

	/// Set a value convertible to JavaScript as a read-only property
	template<typename Value>
	module& set_const(char const* name, Value value)
	{
		v8::HandleScope scope(isolate_);

		obj_->Set(v8pp::to_v8(isolate_, name), to_v8(isolate_, value),
			v8::PropertyAttribute(v8::ReadOnly | v8::DontDelete));
		return *this;
	}

	/// Create a new module instance in V8
	v8::Local<v8::Object> new_instance() { return obj_->NewInstance(); }

private:
	template<typename Variable>
	static void var_get(v8::Local<v8::String>, v8::PropertyCallbackInfo<v8::Value> const& info)
	{
		v8::Isolate* isolate = info.GetIsolate();

		Variable* var = detail::get_external_data<Variable*>(info.Data());
		info.GetReturnValue().Set(to_v8(isolate, *var));
	}

	template<typename Variable>
	static void var_set(v8::Local<v8::String>, v8::Local<v8::Value> value, v8::PropertyCallbackInfo<void> const& info)
	{
		v8::Isolate* isolate = info.GetIsolate();

		Variable* var = detail::get_external_data<Variable*>(info.Data());
		*var = v8pp::from_v8<Variable>(isolate, value);
	}

	v8::Isolate* isolate_;
	v8::Handle<v8::ObjectTemplate> obj_;
};

} // namespace v8pp

#endif // V8PP_MODULE_HPP_INCLUDED
