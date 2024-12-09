#ifndef DYLIB_RELOC_H
#define DYLIB_RELOC_H

/* 运行时动态库符号重定位实现, 可跨平台使用 */

#ifndef INLINE
# if defined(__clang__) || defined(__GNUC__) || defined(__GNUG__)
#   define INLINE __inline__ __attribute__((always_inline))
# elif defined(_MSC_VER)
#   define INLINE __forceinline
# else
#   define INLINE inline
# endif
#endif

#ifndef NOINLINE
# if defined(__clang__) || defined(__GNUC__) || defined(__GNUG__)
#   define NOINLINE __attribute__((__noinline__))
# elif defined(_MSC_VER)
#   define NOINLINE __declspec(noinline)
# else
#   define NOINLINE
# endif
#endif

#ifndef xsun_likely
# if defined(__GNUC__) && defined(__has_builtin) && __has_builtin(__builtin_expect)
#   define xsun_likely(x) __builtin_expect(!!(x), 1)
# elif defined(__has_cpp_attribute) && __has_cpp_attribute(likely)
#   define xsun_likely(x) \
    ([](long a) -> long { if(a) [[likely]] return 1; return 0; })(x)
# else
#   define xsun_likely(x) (!!(x))
# endif
#endif

#ifndef xsun_unlikely
# define xsun_unlikely(x) (!xsun_likely(!(x)))
#endif

#ifndef __STR_CAT
# define __STR_CAT(...) #__VA_ARGS__
# define _STR_CAT(...) __STR_CAT(__VA_ARGS__)
#endif

#ifndef __TC_CAT
# define __TC_CAT(x,y) x##y
# define _TC_CAT(x,y) __TC_CAT(x,y)
#endif

/* 标准 x86-64 指令集版本 */
#define X86_NO 0x1
#define X86_64_V1 0x2
#define X86_64_V2 0x4
#define X86_64_V3 0x8
#define X86_64_V4 0x10

#define FUNCTION_X86_OLD

/* 检测完成後相应指令集版本应执行的语句 */
#define STATEMENT_X86_NO    _TC_CAT(STATEMENT_X86_NO_, FUNCTION_X86_OLD)
#define STATEMENT_X86_64_V1 _TC_CAT(STATEMENT_X86_64_V1_, FUNCTION_X86_OLD)
#define STATEMENT_X86_64_V2 _TC_CAT(STATEMENT_X86_64_V2_, FUNCTION_X86_OLD)
#define STATEMENT_X86_64_V3 _TC_CAT(STATEMENT_X86_64_V3_, FUNCTION_X86_OLD)
#define STATEMENT_X86_64_V4 _TC_CAT(STATEMENT_X86_64_V4_, FUNCTION_X86_OLD)

/* C/C++ 无法依赖宏的原值而改变宏, 因此采用 inline 配合宏替换完成语句扩展,
详细讨论: https://stackoverflow.com/questions/32923945/extending-a-macro-in-c */

INLINE static void (STATEMENT_X86_NO)(void) {};
INLINE static void (STATEMENT_X86_64_V1)(void) {};
INLINE static void (STATEMENT_X86_64_V2)(void) {};
INLINE static void (STATEMENT_X86_64_V3)(void) {};
INLINE static void (STATEMENT_X86_64_V4)(void) {};

/* 检测级别 */
#ifndef DYLIB_RELOC_LEVEL
# define DYLIB_RELOC_LEVEL dylibreloc_level
#endif

#define _L_RELOC_DEFAULT	0x0
#define _L_RELOC_NO 		0x1
#define _L_RELOC_SSE		0x2
#define _L_RELOC_SSE2		0x4
#define _L_RELOC_SSE3		0x8
#define _L_RELOC_SSSE3		0x10
#define _L_RELOC_SSE41		0x20
#define _L_RELOC_SSE42		0x40
#define _L_RELOC_AVX		0x80
#define _L_RELOC_FMA		0x100
#define _L_RELOC_AVX2		0x200
#define _L_RELOC_AVX512F	0x400
#define _L_RELOC_AVX512SKL	0x800

/* C 源文件的无参宏名後紧接小括号会被替换, 但带参宏名後无小括号则不会被替换 */

/* 检测函数 */
#ifndef DYLIB_RELOC_IMPL
# define DYLIB_RELOC_IMPL dylibreloc_impl
#endif

/* 类型名, 需加入导入表的函数原型, 请以 typedef 定义 */
#define _T_RELOC(name) __TC_CAT(type_, name)

/* 函数调用导致堆栈对齐16字节调整, 然而这对此是多余的, 故以内联汇编避免之 */
#define IMPORT_TABLE(type, fun) \
	static void __TC_CAT(reloc_, fun)(void); \
	type *fun = (type*)&__TC_CAT(reloc_, fun); \
	static void __TC_CAT(reloc_, fun)(void) { \
		/*(DYLIB_RELOC_IMPL)();*/ \
		__asm__ __volatile__(_STR_CAT(call DYLIB_RELOC_IMPL)); \
		(*(void (*)(void))fun)(); \
	}

/* 若不想在检测函数完成时导出符号的重定位, 可采取此以惰性初始化函数指针 */
#define IMPORT_TABLE_FUNC(type, fun, fun_no, fun_v1, fun_v2, fun_v3, fun_v4) \
	static void __TC_CAT(reloc_, fun)(void); \
	type *fun = (type*)&__TC_CAT(reloc_, fun); \
	static void __TC_CAT(reloc_, fun)(void) { \
		register void (*func)(void) asm("r11"); \
		__asm__ __volatile__(_STR_CAT( \
			call	DYLIB_RELOC_IMPL\n\t \
			movl	DYLIB_RELOC_LEVEL(%%rip), %%r11d\n\t \
		):"=r"(func)::); \
		if((X86_INSTRUCTION_SET & X86_64_V4) && ((X86_INSTRUCTION_SET & X86_NO) || (X86_INSTRUCTION_SET & X86_64_V1) || (X86_INSTRUCTION_SET & X86_64_V2) || (X86_INSTRUCTION_SET & X86_64_V3))) \
		__asm__ __volatile__(_STR_CAT( \
			cmpl	$(_L_RELOC_AVX512SKL), %%r11d\n\t \
			jae 	3f\n\t \
		):"+r"(func)::); \
		if((X86_INSTRUCTION_SET & X86_64_V3) && ((X86_INSTRUCTION_SET & X86_NO) || (X86_INSTRUCTION_SET & X86_64_V1) || (X86_INSTRUCTION_SET & X86_64_V2))) \
		__asm__ __volatile__(_STR_CAT( \
			cmpl	$(_L_RELOC_AVX2), %%r11d\n\t \
			jae 	2f\n\t \
		):"+r"(func)::); \
		if((X86_INSTRUCTION_SET & X86_64_V2) && ((X86_INSTRUCTION_SET & X86_NO) || (X86_INSTRUCTION_SET & X86_64_V1))) \
		__asm__ __volatile__(_STR_CAT( \
			cmpl	$(_L_RELOC_SSE42), %%r11d\n\t \
			jae 	1f\n\t \
		):"+r"(func)::); \
		if((X86_INSTRUCTION_SET & X86_64_V1) && (X86_INSTRUCTION_SET & X86_NO)) \
		__asm__ __volatile__(_STR_CAT( \
			cmpl	$(_L_RELOC_SSE2), %%r11d\n\t \
			jb  	0f\n\t \
			mov 	fun_v1(%%rip), %%r11\n\t \
			jmp 	4f\n\t \
			0:\n\t \
		):"+r"(func)::); \
		__asm__ __volatile__(_STR_CAT( \
			mov 	fun_no(%%rip), %%r11\n\t \
		):"+r"(func)::); \
		if((X86_INSTRUCTION_SET & X86_64_V2) && ((X86_INSTRUCTION_SET & X86_NO) || (X86_INSTRUCTION_SET & X86_64_V1))) \
		__asm__ __volatile__(_STR_CAT( \
			jmp 	4f\n\t \
			1:\n\t \
			mov 	fun_v2(%%rip), %%r11\n\t \
		):"+r"(func)::); \
		if((X86_INSTRUCTION_SET & X86_64_V3) && ((X86_INSTRUCTION_SET & X86_NO) || (X86_INSTRUCTION_SET & X86_64_V1) || (X86_INSTRUCTION_SET & X86_64_V2))) \
		__asm__ __volatile__(_STR_CAT( \
			jmp 	4f\n\t \
			2:\n\t \
			mov 	fun_v3(%%rip), %%r11\n\t \
		):"+r"(func)::); \
		if((X86_INSTRUCTION_SET & X86_64_V4) && ((X86_INSTRUCTION_SET & X86_NO) || (X86_INSTRUCTION_SET & X86_64_V1) || (X86_INSTRUCTION_SET & X86_64_V2) || (X86_INSTRUCTION_SET & X86_64_V3))) \
		__asm__ __volatile__(_STR_CAT( \
			jmp 	4f\n\t \
			3:\n\t \
			mov 	fun_v4(%%rip), %%r11\n\t \
		):"+r"(func)::); \
		__asm__ __volatile__(_STR_CAT( \
			4:\n\t \
			mov 	%%r11, fun(%%rip)\n\t \
		):"+r"(func)::); \
		(*func)(); \
	}


#else

#if !defined(X86_INSTRUCTION_SET) || !(X86_INSTRUCTION_SET & 0x1f)
# error Unsupported or Undefined X86_INSTRUCTION_SET!
#endif

IMPORT_TABLE(_T_RELOC(FUNCTION_X86_NAME), FUNCTION_X86_NAME);

#define _STATEMENT_X86_NO    _TC_CAT(STATEMENT_X86_NO_, FUNCTION_X86_NAME)
#define _STATEMENT_X86_64_V1 _TC_CAT(STATEMENT_X86_64_V1_, FUNCTION_X86_NAME)
#define _STATEMENT_X86_64_V2 _TC_CAT(STATEMENT_X86_64_V2_, FUNCTION_X86_NAME)
#define _STATEMENT_X86_64_V3 _TC_CAT(STATEMENT_X86_64_V3_, FUNCTION_X86_NAME)
#define _STATEMENT_X86_64_V4 _TC_CAT(STATEMENT_X86_64_V4_, FUNCTION_X86_NAME)

#if X86_INSTRUCTION_SET & X86_NO
INLINE static void (_STATEMENT_X86_NO)(void) {
	(STATEMENT_X86_NO)();
	FUNCTION_X86_NAME = &(FUNCTION_X86_NO);
}
#endif

#if X86_INSTRUCTION_SET & X86_64_V1
INLINE static void (_STATEMENT_X86_64_V1)(void) {
	(STATEMENT_X86_64_V1)();
	FUNCTION_X86_NAME = &(FUNCTION_X86_64_V1);
}
#endif

#if X86_INSTRUCTION_SET & X86_64_V2
INLINE static void (_STATEMENT_X86_64_V2)(void) {
	(STATEMENT_X86_64_V2)();
	FUNCTION_X86_NAME = &(FUNCTION_X86_64_V2);
}
#endif

#if X86_INSTRUCTION_SET & X86_64_V3
INLINE static void (_STATEMENT_X86_64_V3)(void) {
	(STATEMENT_X86_64_V3)();
	FUNCTION_X86_NAME = &(FUNCTION_X86_64_V3);
}
#endif

#if X86_INSTRUCTION_SET & X86_64_V4
INLINE static void (_STATEMENT_X86_64_V4)(void) {
	(STATEMENT_X86_64_V4)();
	FUNCTION_X86_NAME = &(FUNCTION_X86_64_V4);
}
#endif

#undef _STATEMENT_X86_NO
#undef _STATEMENT_X86_64_V1
#undef _STATEMENT_X86_64_V2
#undef _STATEMENT_X86_64_V3
#undef _STATEMENT_X86_64_V4


#undef FUNCTION_X86_NAME
#undef FUNCTION_X86_NO
#undef FUNCTION_X86_64_V1
#undef FUNCTION_X86_64_V2
#undef FUNCTION_X86_64_V3
#undef FUNCTION_X86_64_V4
#undef FUNCTION_X86_OLD

#endif
