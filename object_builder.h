#pragma once
#include <v8.h>

inline v8::PropertyAttribute operator | ( v8::PropertyAttribute a, v8::PropertyAttribute b ) {
	return (v8::PropertyAttribute)( (int)a | (int)b );
}

inline v8::PropertyAttribute &operator |= ( v8::PropertyAttribute &a, v8::PropertyAttribute b ) {
	return a = a | b;
}

inline v8::Local<v8::String> intern_string( v8::Isolate *isolate, char const *s ) {
	auto type = v8::NewStringType::kInternalized;
	return v8::String::NewFromUtf8( isolate, s, type ).ToLocalChecked();
}

struct ClassBuilder {
	v8::Isolate *isolate;
	v8::Local<v8::FunctionTemplate> cls;

	void set_internal_field_count( int value ) {
		cls->InstanceTemplate()->SetInternalFieldCount( value );
	}

	void add_method( char const *name, v8::FunctionCallback callback ) {
		auto signature = v8::Signature::New( isolate, cls );
		auto fun = v8::FunctionTemplate::New( isolate, callback, {}, signature );
		auto fun_name = intern_string( isolate, name );
		auto prototype = cls->PrototypeTemplate();
		prototype->Set( fun_name, fun );
	}

	void add_property( char const *name, v8::AccessorGetterCallback getter,
			v8::AccessorSetterCallback setter = NULL,
			v8::PropertyAttribute attributes = v8::DontEnum )
	{
		auto signature = v8::AccessorSignature::New( isolate, cls );
		auto prop_name = intern_string( isolate, name );
		auto instance = cls->InstanceTemplate();
		if( ! setter )
			attributes |= v8::ReadOnly;
		instance->SetAccessor( prop_name, getter, setter, {},
				v8::DEFAULT, attributes, signature );
	}
};

struct ObjectBuilder {
	v8::Isolate *isolate;
	v8::Local<v8::Object> target;

	void add_class( char const *name, v8::FunctionCallback constructor,
			void (*body)( ClassBuilder ) )
	{
		auto cls = v8::FunctionTemplate::New( isolate, constructor, {} );
		auto cls_name = intern_string( isolate, name );
		cls->SetClassName( cls_name );
		body( { isolate, cls } );
		target->Set( cls_name, cls->GetFunction() );
	}
};
