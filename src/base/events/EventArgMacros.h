/*
* Copyright (c) 2018 An Incredibly Big Red Robot
*
* Use of this source code is governed by a "PRIVATE-CODE-IS-PRIVATE" license that can't be
* found in the included LICENSE file.
*/

#ifndef EVENTARG
#define EVENTARG

#define EVENTARGS(name, type) \
	struct name##EventArgs \
	{\
		public:\
			type GetValue() { return arg##name; }\
			name##EventArgs(type value##name) { arg##name = value##name; } \
		private:\
			type arg##name;\
	};
#define EVENTARGS2(classname, arg1, arg2) \
	struct classname##EventArgs \
	{\
		public:\
			arg1 GetFirstArgument() { return a1##classname; }\
			arg2 GetSecondArgument() { return a2##classname; }\
			classname##EventArgs(arg1 a1, arg2 a2) {\
				a1##classname = a1;\
				a2##classname = a2;\
			}\
		private:\
			arg1 a1##classname;\
			arg2 a2##classname;\
	};
#define EVENTARGS3(classname, arg1, arg2, arg3) \
	struct classname##EventArgs \
	{\
		public:\
			arg1 GetFirstArgument() { return a1##classname; }\
			arg2 GetSecondArgument() { return a2##classname; }\
			arg3 GetThirdArgument() { return a3##classname; }\
			classname##EventArgs(arg1 a1, arg2 a2, arg3 a3) {\
				a1##classname = a1;\
				a2##classname = a2;\
				a3##classname = a3;\
			}\
		private:\
			arg1 a1##classname;\
			arg2 a2##classname;\
			arg3 a3##classname;\
	};

#endif