/*
* Copyright (c) 2017 An Incredibly Big Red Robot
*
* Use of this source code is governed by a "BSD-style" license that can be
* found in the included LICENSE file.
*/

#ifndef PROPMAC
#define PROPMAC

#define PROPERTY_PTR(name, type) private: \
								 type* value##name;\
								 public: \
								 void Set##name(type* propertyType) { value##name = propertyType; } \
								 type* Get##name() { return value##name; }
#define PROPERTY(name, type) private: \
						 	 type value##name;\
							 public: \
							 void Set##name(type propertyType) { value##name = propertyType; } \
							 type Get##name() { return value##name; }

#define PRIVATE_PROP(name, type) private: \
						 	 	 	type value##name;\
							 		void Set##name(type propertyType) { value##name = propertyType; } \
							 		type Get##name() { return value##name; }

#endif