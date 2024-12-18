#define DYLIB_RELOC_IMPL dllfastcpy_init
#define DYLIB_RELOC_LEVEL dllfastcpy_level
#include "dylibreloc.h"

#define FASTCPY_NO_NTA_PART
#define DLLFASTCPY_LIBRARY 4
#include "fastcpy.h"
#undef DLLFASTCPY_LIBRARY
#define DLLFASTCPY_LIBRARY 1
#include "fastcpy.h"
#undef FASTCPY_NO_NTA_PART
#undef DLLFASTCPY_LIBRARY
#define DLLFASTCPY_LIBRARY 3
#include "fastcpy.h"

/* 重要!!! 头文件的宏名或变量名不能与源文件相冲突, 故需取消这些定义 */
#undef fastcpy_tiny
#undef fastcpy
#undef fast_stpcpy
#undef fast_wstpcpy
#undef fast_dstpcpy
#undef fast_qstpcpy


#include <stddef.h>
#include <stdint.h>

#include "stdatomic_def.h"

#define X86_INSTRUCTION_SET (X86_64_V1|X86_64_V3|X86_64_V4)

// 指令集支持级别
_Atomic int32_t DYLIB_RELOC_LEVEL = ATOMIC_VAR_INIT(_L_RELOC_DEFAULT);

/* 定义动态库全局变量的导入表 */

/* 隐式链接动态库时, 没必要重定向其导出变量, 显示加载动态库的重定向暂未完善 */
//size_t *__ptr_fastcpy_non_temporal_threshold = NULL;

/* 定义动态库函数的导入表 */

typedef void* _T_RELOC(fastcpy_tiny)(void *dst, const void *src, size_t size);
#define FUNCTION_X86_NAME fastcpy_tiny
#define FUNCTION_X86_NO fastcpy_tiny_NO
#define FUNCTION_X86_64_V1 fastcpy_tiny_SSE2
#define FUNCTION_X86_64_V3 fastcpy_tiny_AVX2
#define FUNCTION_X86_64_V4 fastcpy_tiny_AVX512
#include "dylibreloc.h"
#define FUNCTION_X86_OLD fastcpy_tiny

typedef void* _T_RELOC(fastcpy)(void *dst, const void *src, size_t size);
#if 0/* 惰性初始化函数指针, 你也可以重写一个函数以代替宏, 通常不建议 */
IMPORT_TABLE_FUNC(_T_RELOC(fastcpy), fastcpy, fastcpy_NO, fastcpy_SSE2, fastcpy_SSE2, fastcpy_AVX2, fastcpy_AVX512);
#else/* 此方式的导出表更小, 检测函数会设置 FUNCTION_X86_NAME 函数指针 */
#define FUNCTION_X86_NAME fastcpy
#define FUNCTION_X86_NO fastcpy_NO
#define FUNCTION_X86_64_V1 fastcpy_SSE2
#define FUNCTION_X86_64_V3 fastcpy_AVX2
#define FUNCTION_X86_64_V4 fastcpy_AVX512
#include "dylibreloc.h"
#define FUNCTION_X86_OLD fastcpy
#endif

typedef uint8_t* _T_RELOC(fast_stpcpy)(uint8_t *dst, const uint8_t *src);
#define FUNCTION_X86_NAME fast_stpcpy
#define FUNCTION_X86_NO fast_stpcpy_NO
#define FUNCTION_X86_64_V1 fast_stpcpy_SSE2
#define FUNCTION_X86_64_V3 fast_stpcpy_AVX2
#define FUNCTION_X86_64_V4 fast_stpcpy_AVX512
#include "dylibreloc.h"
#define FUNCTION_X86_OLD fast_stpcpy

typedef uint16_t* _T_RELOC(fast_wstpcpy)(uint16_t *dst, const uint16_t *src);
#define FUNCTION_X86_NAME fast_wstpcpy
#define FUNCTION_X86_NO fast_wstpcpy_NO
#define FUNCTION_X86_64_V1 fast_wstpcpy_SSE2
#define FUNCTION_X86_64_V3 fast_wstpcpy_AVX2
#define FUNCTION_X86_64_V4 fast_wstpcpy_AVX512
#include "dylibreloc.h"
#define FUNCTION_X86_OLD fast_wstpcpy

typedef uint32_t* _T_RELOC(fast_dstpcpy)(uint32_t *dst, const uint32_t *src);
#define FUNCTION_X86_NAME fast_dstpcpy
#define FUNCTION_X86_NO fast_dstpcpy_NO
#define FUNCTION_X86_64_V1 fast_dstpcpy_SSE2
#define FUNCTION_X86_64_V3 fast_dstpcpy_AVX2
#define FUNCTION_X86_64_V4 fast_dstpcpy_AVX512
#include "dylibreloc.h"
#define FUNCTION_X86_OLD fast_dstpcpy

typedef uint64_t* _T_RELOC(fast_qstpcpy)(uint64_t *dst, const uint64_t *src);
#define FUNCTION_X86_NAME fast_qstpcpy
#define FUNCTION_X86_NO fast_qstpcpy_NO
#define FUNCTION_X86_64_V1 fast_qstpcpy_SSE2
#define FUNCTION_X86_64_V3 fast_qstpcpy_AVX2
#define FUNCTION_X86_64_V4 fast_qstpcpy_AVX512
#include "dylibreloc.h"
#define FUNCTION_X86_OLD fast_qstpcpy


#ifndef __cpuid
#define __cpuid(level, a, b, c, d)					\
  __asm__ __volatile__ ("cpuid\n\t"					\
			: "=a" (a), "=b" (b), "=c" (c), "=d" (d)	\
			: "0" (level))
#endif
#ifndef __cpuid_count
#define __cpuid_count(level, count, a, b, c, d)				\
  __asm__ __volatile__ ("cpuid\n\t"					\
			: "=a" (a), "=b" (b), "=c" (c), "=d" (d)	\
			: "0" (level), "2" (count))
#endif

/* 此函数是动态库符号重定位的具体操作, 通常仅需执行一次 */
INLINE static int32_t _TC_CAT(DYLIB_RELOC_IMPL,_once)(void) {
	/* 检测CPU指令集 */
	int32_t reloc_level = _L_RELOC_NO;
	/* has_cpu_flags lm cmov cx8 fpu fxsr mmx syscall sse2 && level = 1
	has_cpu_flags cx16 lahf_lm popcnt sse4_1 sse4_2 ssse3 && level = 2
	has_cpu_flags avx avx2 bmi1 bmi2 f16c fma abm movbe xsave && level = 3
	has_cpu_flags avx512f avx512bw avx512cd avx512dq avx512vl && level = 4 */
	register uint32_t eax, ebx, ecx, edx;
	__cpuid(0, eax, ebx, ecx, edx);
	if(eax >= 7) {
		__cpuid(7, eax, ebx, ecx, edx);
		if(ebx & (1<<5)) reloc_level |= _L_RELOC_AVX2;
		if(ebx & (1<<16)) reloc_level |= _L_RELOC_AVX512F;
		if((ebx & 0xD0030000) == 0xD0030000) reloc_level |= _L_RELOC_AVX512SKL;
	}
	__cpuid(1, eax, ebx, ecx, edx);
	if(ecx & (1<<12)) reloc_level |= _L_RELOC_FMA;
	if(ecx & (1<<28)) reloc_level |= _L_RELOC_AVX;
	if(ecx & (1<<20)) reloc_level |= _L_RELOC_SSE42;
	if(ecx & (1<<19)) reloc_level |= _L_RELOC_SSE41;
	if(ecx & (1<<9)) reloc_level |= _L_RELOC_SSSE3;
	if(ecx & (1<<0)) reloc_level |= _L_RELOC_SSE3;
	if(edx & (1<<26)) reloc_level |= _L_RELOC_SSE2;
	if(edx & (1<<25)) reloc_level |= _L_RELOC_SSE;

	/* 根据检测情况设置函数指针和数据指针 */
  do {
#if X86_INSTRUCTION_SET & X86_64_V4
	if(!((X86_INSTRUCTION_SET & X86_NO) || (X86_INSTRUCTION_SET & X86_64_V1) || (X86_INSTRUCTION_SET & X86_64_V2) || (X86_INSTRUCTION_SET & X86_64_V3)) || reloc_level >= _L_RELOC_AVX512SKL) {
		/* 为 x86-64-v4 的动态库函数在这里重定位 */
		(STATEMENT_X86_64_V4)();
		break;
	}
#endif

#if X86_INSTRUCTION_SET & X86_64_V3
	if(!((X86_INSTRUCTION_SET & X86_NO) || (X86_INSTRUCTION_SET & X86_64_V1) || (X86_INSTRUCTION_SET & X86_64_V2)) || reloc_level >= _L_RELOC_AVX2) {
		/* 为 x86-64-v3 的动态库函数在这里重定位 */
		(STATEMENT_X86_64_V3)();
		break;
	}
#endif

#if X86_INSTRUCTION_SET & X86_64_V2
	if(!((X86_INSTRUCTION_SET & X86_NO) || (X86_INSTRUCTION_SET & X86_64_V1)) || reloc_level >= _L_RELOC_SSE42) {
		/* 为 x86-64-v2 的动态库函数在这里重定位 */
		(STATEMENT_X86_64_V2)();
		break;
	}
#endif

#if X86_INSTRUCTION_SET & X86_64_V1
	if(!(X86_INSTRUCTION_SET & X86_NO) || reloc_level >= _L_RELOC_SSE2) {
		/* 为 x86-64-v1 的动态库函数在这里重定位 */
		(STATEMENT_X86_64_V1)();
		break;
	}
#endif

#if X86_INSTRUCTION_SET & X86_NO
	/* 无 SIMD 的动态库函数在这里重定位 */
	(STATEMENT_X86_NO)();
#endif
  } while(0);
	return reloc_level;
}

INLINE static void _TC_CAT(DYLIB_RELOC_IMPL,_call_once)(void);

/* 动态库符号重定位实现, 函数名称可用小括号括起来, 这允许函数名与带参宏名相同 */
NOINLINE __attribute__((used)) void (DYLIB_RELOC_IMPL)(void) {
	__asm__ __volatile__(_STR_CAT(
		cmpl	$(_L_RELOC_DEFAULT), DYLIB_RELOC_LEVEL(%rip)\n\t
		jle 	_TC_CAT(DYLIB_RELOC_IMPL,_check)\n\t
	));// 无扩展 __asm__ 不需要以 %%rip 引用寄存器名称
}

NOINLINE __attribute__((used)) static void _TC_CAT(DYLIB_RELOC_IMPL,_check)(void) {
	/* 保存寄存器, 需兼容矢量调用 */
#if defined(_WIN32) || defined(__CYGWIN__)/* ms_abi */
#if defined(__AVX512F__)
	char stack_save[0x1A0];
#elif defined(__AVX__)
	char stack_save[0xE0];
#elif 1 || defined(__SSE__)
	char stack_save[0x80];
#endif
	__asm__ __volatile__(_STR_CAT(
		movq	%%rcx, -0x80(%0)\n\t
		movq	%%rdx, -0x78(%0)\n\t
		movq	%%r8, -0x70(%0)\n\t
		movq	%%r9, -0x68(%0)\n\t
#if defined(__AVX512F__)
		vmovups	%%zmm0, -0x60(%0)\n\t
		vmovups	%%zmm1, -0x20(%0)\n\t
		vmovups	%%zmm2, 0x20(%0)\n\t
		vmovups	%%zmm3, 0x60(%0)\n\t
		vmovups	%%zmm4, 0xA0(%0)\n\t
		vmovups	%%zmm5, 0xE0(%0)\n\t
#elif defined(__AVX__)
		vmovups	%%ymm0, -0x60(%0)\n\t
		vmovups	%%ymm1, -0x40(%0)\n\t
		vmovups	%%ymm2, -0x20(%0)\n\t
		vmovups	%%ymm3, 0x0(%0)\n\t
		vmovups	%%ymm4, 0x20(%0)\n\t
		vmovups	%%ymm5, 0x40(%0)\n\t
#elif 1 || defined(__SSE__)
		movups	%%xmm0, -0x60(%0)\n\t
		movups	%%xmm1, -0x50(%0)\n\t
		movups	%%xmm2, -0x40(%0)\n\t
		movups	%%xmm3, -0x30(%0)\n\t
		movups	%%xmm4, -0x20(%0)\n\t
		movups	%%xmm5, -0x10(%0)\n\t
#endif
	)::"a"(stack_save + 0x80));
#else/* sysv_abi */
#if defined(__AVX512F__)
	char stack_save[0x240];
#elif defined(__AVX__)
	char stack_save[0x140];
#elif 1 || defined(__SSE__)
	char stack_save[0xC0];
#endif
	char *pstack_save asm("r11") = stack_save + 0x80;
	__asm__ __volatile__(_STR_CAT(
		movq	%%rdi, -0x80(%0)\n\t
		movq	%%rsi, -0x78(%0)\n\t
		movq	%%rdx, -0x70(%0)\n\t
		movq	%%rcx, -0x68(%0)\n\t
		movq	%%rax, -0x60(%0)\n\t
		movq	%%r8, -0x58(%0)\n\t
		movq	%%r9, -0x50(%0)\n\t
		movq	%%r10, -0x48(%0)\n\t
#if defined(__AVX512F__)
		vmovups	%%zmm0, -0x40(%0)\n\t
		vmovups	%%zmm1, 0x0(%0)\n\t
		vmovups	%%zmm2, 0x40(%0)\n\t
		vmovups	%%zmm3, 0x80(%0)\n\t
		vmovups	%%zmm4, 0xC0(%0)\n\t
		vmovups	%%zmm5, 0x100(%0)\n\t
		vmovups	%%zmm6, 0x140(%0)\n\t
		vmovups	%%zmm7, 0x180(%0)\n\t
#elif defined(__AVX__)
		vmovups	%%ymm0, -0x40(%0)\n\t
		vmovups	%%ymm1, -0x20(%0)\n\t
		vmovups	%%ymm2, 0x0(%0)\n\t
		vmovups	%%ymm3, 0x20(%0)\n\t
		vmovups	%%ymm4, 0x40(%0)\n\t
		vmovups	%%ymm5, 0x60(%0)\n\t
		vmovups	%%ymm6, 0x80(%0)\n\t
		vmovups	%%ymm7, 0xA0(%0)\n\t
#elif 1 || defined(__SSE__)
		movups	%%xmm0, -0x40(%0)\n\t
		movups	%%xmm1, -0x30(%0)\n\t
		movups	%%xmm2, -0x20(%0)\n\t
		movups	%%xmm3, -0x10(%0)\n\t
		movups	%%xmm4, 0x0(%0)\n\t
		movups	%%xmm5, 0x10(%0)\n\t
		movups	%%xmm6, 0x20(%0)\n\t
		movups	%%xmm7, 0x30(%0)\n\t
#endif
	)::"r"(pstack_save));
#endif

	_TC_CAT(DYLIB_RELOC_IMPL,_call_once)();

	/* 恢复寄存器, 需兼容矢量调用 */
#if defined(_WIN32) || defined(__CYGWIN__)/* ms_abi */
	__asm__ __volatile__(_STR_CAT(
		movq	-0x80(%0), %%rcx\n\t
		movq	-0x78(%0), %%rdx\n\t
		movq	-0x70(%0), %%r8\n\t
		movq	-0x68(%0), %%r9\n\t
#if defined(__AVX512F__)
		vmovups	-0x60(%0), %%zmm0\n\t
		vmovups	-0x20(%0), %%zmm1\n\t
		vmovups	0x20(%0), %%zmm2\n\t
		vmovups	0x60(%0), %%zmm3\n\t
		vmovups	0xA0(%0), %%zmm4\n\t
		vmovups	0xE0(%0), %%zmm5\n\t
#elif defined(__AVX__)
		vmovups	-0x60(%0), %%ymm0\n\t
		vmovups	-0x40(%0), %%ymm1\n\t
		vmovups	-0x20(%0), %%ymm2\n\t
		vmovups	0x0(%0), %%ymm3\n\t
		vmovups	0x20(%0), %%ymm4\n\t
		vmovups	0x40(%0), %%ymm5\n\t
#elif 1 || defined(__SSE__)
		movups	-0x60(%0), %%xmm0\n\t
		movups	-0x50(%0), %%xmm1\n\t
		movups	-0x40(%0), %%xmm2\n\t
		movups	-0x30(%0), %%xmm3\n\t
		movups	-0x20(%0), %%xmm4\n\t
		movups	-0x10(%0), %%xmm5\n\t
#endif
	)::"a"(stack_save + 0x80));
#else/* sysv_abi */
	__asm__ __volatile__(_STR_CAT(
		movq	-0x80(%0), %%rdi\n\t
		movq	-0x78(%0), %%rsi\n\t
		movq	-0x70(%0), %%rdx\n\t
		movq	-0x68(%0), %%rcx\n\t
		movq	-0x60(%0), %%rax\n\t
		movq	-0x58(%0), %%r8\n\t
		movq	-0x50(%0), %%r9\n\t
		movq	-0x48(%0), %%r10\n\t
#if defined(__AVX512F__)
		vmovups	-0x40(%0), %%zmm0\n\t
		vmovups	0x0(%0), %%zmm1\n\t
		vmovups	0x40(%0), %%zmm2\n\t
		vmovups	0x80(%0), %%zmm3\n\t
		vmovups	0xC0(%0), %%zmm4\n\t
		vmovups	0x100(%0), %%zmm5\n\t
		vmovups	0x140(%0), %%zmm6\n\t
		vmovups	0x180(%0), %%zmm7\n\t
#elif defined(__AVX__)
		vmovups	-0x40(%0), %%ymm0\n\t
		vmovups	-0x20(%0), %%ymm1\n\t
		vmovups	0x0(%0), %%ymm2\n\t
		vmovups	0x20(%0), %%ymm3\n\t
		vmovups	0x40(%0), %%ymm4\n\t
		vmovups	0x60(%0), %%ymm5\n\t
		vmovups	0x80(%0), %%ymm6\n\t
		vmovups	0xA0(%0), %%ymm7\n\t
#elif 1 || defined(__SSE__)
		movups	-0x40(%0), %%xmm0\n\t
		movups	-0x30(%0), %%xmm1\n\t
		movups	-0x20(%0), %%xmm2\n\t
		movups	-0x10(%0), %%xmm3\n\t
		movups	0x0(%0), %%xmm4\n\t
		movups	0x10(%0), %%xmm5\n\t
		movups	0x20(%0), %%xmm6\n\t
		movups	0x30(%0), %%xmm7\n\t
#endif
	)::"r"(pstack_save));
#endif
}


#if defined(DYLIB_RELOC_USE_MUTEX)
/* 通常, 需重定位动态库的符号数量较少而耗时极短则不建议使用互斥锁 */
# if defined(__linux) || defined(__linux__) || defined(linux)
/* 注意, 此处以 Linux futex 实现互斥锁, 需避免持锁线程异常终止 */
#   ifndef _GNU_SOURCE
#     define _GNU_SOURCE
#   endif
#   include <linux/futex.h>
#   include <sys/syscall.h>
#   include <unistd.h>

/* _mm_pause() use head file */
#if !(defined(__GNUC__) && defined(__has_builtin) &&                           \
      __has_builtin(__builtin_ia32_pause))
# if defined(__x86_64__) || defined(_M_X64) || defined(i386) ||                 \
    defined(__i386__) || defined(__i386) || defined(_M_IX86)
#   include <emmintrin.h>
#   define xsun_cpu_pause _mm_pause
# endif
#else
# define xsun_cpu_pause __builtin_ia32_pause
#endif

INLINE static int32_t futex_wait(int32_t *uaddr, int32_t val) {
	int32_t ret;
	__asm__ __volatile__(
		"xor %%r10d, %%r10d\n\t"
		"syscall\n\t"
		: "=a"(ret)
		: "0"(SYS_futex), "D"(uaddr), "S"(FUTEX_WAIT), "d"(val)
		: "rcx", "r10", "r11"
	);
	return ret;
} //若uaddr指向的值是val，则等待timeout时间，调用成功返回0，否则返回非0

INLINE static int32_t futex_wake(int32_t *uaddr, int32_t n) {
	int32_t ret;
	__asm__ __volatile__(
		"syscall\n\t"
		: "=a"(ret)
		: "0"(SYS_futex), "D"(uaddr), "S"(FUTEX_WAKE), "d"(n)
		: "rcx", "r11"
	);
	return ret;
} //唤醒在uaddr等待的n组等待者，此是相对时间等待

// 在此 futex 等待的线程数量
static _Atomic int32_t _TC_CAT(DYLIB_RELOC_LEVEL,_waiters) = ATOMIC_VAR_INIT(0);

INLINE static void _TC_CAT(DYLIB_RELOC_IMPL,_lock)(void) {
	int32_t spin_count, val = _L_RELOC_DEFAULT;
	if(xsun_likely(atomic_cmpxchg_acq(&DYLIB_RELOC_LEVEL, &val, 0x80000000))) return;
	atomic_inc_relaxed(&_TC_CAT(DYLIB_RELOC_LEVEL,_waiters));
	while(1) {
		spin_count = 0;
		do_loop:
		if(xsun_likely(--spin_count & 0x7f)) xsun_cpu_pause();
		else futex_wait(&DYLIB_RELOC_LEVEL, val);
		val = atomic_load_relaxed(&DYLIB_RELOC_LEVEL);
		__asm__ __volatile__ goto(_STR_CAT(
			testl	%k[val], %k[val]\n\t
			js  	%l[do_loop]\n\t
			jz  	%l[done_loop]\n\t
		)::[val]"r"(val) ::do_loop, done_loop);
		atomic_dec_relaxed(&_TC_CAT(DYLIB_RELOC_LEVEL,_waiters));
		__asm__ __volatile__(_STR_CAT(
			jmp 	_TC_CAT(.L_restore_,DYLIB_RELOC_IMPL)\n\t
		));
		done_loop:
		if(xsun_likely(atomic_cmpxchg_acq(&DYLIB_RELOC_LEVEL, &val, 0x80000000))) {
			atomic_dec_relaxed(&_TC_CAT(DYLIB_RELOC_LEVEL,_waiters));
			return;
		}
	}
}

INLINE static void _TC_CAT(DYLIB_RELOC_IMPL,_unlock)(int32_t val) {
	atomic_store_seq_cst(&DYLIB_RELOC_LEVEL, val);
	if(xsun_likely(atomic_load_relaxed(&_TC_CAT(DYLIB_RELOC_LEVEL,_waiters)) == 0)) return;
	else futex_wake(&DYLIB_RELOC_LEVEL, 1);
}

INLINE static void _TC_CAT(DYLIB_RELOC_IMPL,_call_once)(void) {
	/* 获取互斥锁 */
	_TC_CAT(DYLIB_RELOC_IMPL,_lock)();
	/* 解除互斥锁并设置 DYLIB_RELOC_LEVEL */
	_TC_CAT(DYLIB_RELOC_IMPL,_unlock)(_TC_CAT(DYLIB_RELOC_IMPL,_once)());
	__asm__ __volatile__(_STR_CAT(_TC_CAT(.L_restore_,DYLIB_RELOC_IMPL):));
}

# elif defined(_WIN32) || defined(__WIN32__) || defined(WIN32) || defined(__CYGWIN__)
/* Windows 平台, 仅使用 Windows 内核函数, 以避免编译器API兼容问题 */
#   include <windows.h>

// Global variable for one-time initialization structure
static INIT_ONCE _TC_CAT(DYLIB_RELOC_IMPL,_once_flag) = INIT_ONCE_STATIC_INIT; // Static initialization

static BOOL _TC_CAT(DYLIB_RELOC_IMPL,_init_once)(PINIT_ONCE InitOnce, PVOID Parameter, PVOID *Context) {
	atomic_store_rel(&DYLIB_RELOC_LEVEL, _TC_CAT(DYLIB_RELOC_IMPL,_once)());
	return TRUE;
}

INLINE static void _TC_CAT(DYLIB_RELOC_IMPL,_call_once)(void) {
	InitOnceExecuteOnce(&_TC_CAT(DYLIB_RELOC_IMPL,_once_flag), &_TC_CAT(DYLIB_RELOC_IMPL,_init_once), NULL, NULL);
}

# else/* 其他平台 */
#   include <threads.h>

static once_flag _TC_CAT(DYLIB_RELOC_IMPL,_once_flag) = ONCE_FLAG_INIT;

static void _TC_CAT(DYLIB_RELOC_IMPL,_once_store)(void) {
	atomic_store_rel(&DYLIB_RELOC_LEVEL, _TC_CAT(DYLIB_RELOC_IMPL,_once)());
}

INLINE static void _TC_CAT(DYLIB_RELOC_IMPL,_call_once)(void) {
	call_once(&_TC_CAT(DYLIB_RELOC_IMPL,_once_flag), &_TC_CAT(DYLIB_RELOC_IMPL,_once_store));
}

# endif

#else/* 不使用互斥锁也能正常工作 */

INLINE static void _TC_CAT(DYLIB_RELOC_IMPL,_call_once)(void) {
	atomic_store_rel(&DYLIB_RELOC_LEVEL, _TC_CAT(DYLIB_RELOC_IMPL,_once)());
}

#endif

