#ifndef LIB_FAST_CPY_H

#ifndef FASTCPY_NO_NTA_PART
#define LIB_FAST_CPY_H
#endif

#ifndef LIBRARY_GLOBAL_H
#define LIBRARY_GLOBAL_H

#if defined(_MSC_VER) || defined(WIN64) || defined(_WIN64) || defined(__WIN64__) || defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
#  define Q_DECL_EXPORT __declspec(dllexport)
#  define Q_DECL_IMPORT __declspec(dllimport)
#else
#  define Q_DECL_EXPORT     __attribute__((visibility("default")))
#  define Q_DECL_IMPORT     __attribute__((visibility("default")))
#endif

#endif // LIBRARY_GLOBAL_H

/**
 * 编译宏说明：
 * 1. FASTCPY_SHARED_LIBRARY 为动态库构建, 全局函数添加 dllexport 属性
 * 2. FASTCPY_STATIC_LIBRARY 为静态库构建, 全局函数不添加 dllexport 属性
 * 3. FASTCPY_OWNER_SOURCE 为测试源码构建, 代码段将强制4K对齐以避免性能影响
 * 4. FASTCPY_UNROLLED_4VEC 强制 fastcpy 使用循环展开四向量(通常不要定义此选项)
 * 5. DLLFASTCPY_LIBRARY 以当前环境指令集版本为後缀而命名全局函数
 * 6. FASTCPY_NO_NTA_PART 不编译 fastcpy_non_temporal_threshold 及相关函数
 */

#undef fastcpy_tiny
#undef fastcpy
#undef fast_stpcpy
#undef fast_wstpcpy
#undef fast_dstpcpy
#undef fast_qstpcpy

#ifdef DLLFASTCPY_LIBRARY
# if DLLFASTCPY_LIBRARY == 4 || (DLLFASTCPY_LIBRARY == 0 && defined(__AVX512VL__))
#   define fastcpy_tiny fastcpy_tiny_AVX512
#   define fastcpy      fastcpy_AVX512
#   define fast_stpcpy  fast_stpcpy_AVX512
#   define fast_wstpcpy fast_wstpcpy_AVX512
#   define fast_dstpcpy fast_dstpcpy_AVX512
#   define fast_qstpcpy fast_qstpcpy_AVX512
# elif DLLFASTCPY_LIBRARY == 3 || (DLLFASTCPY_LIBRARY == 0 && defined(__AVX2__))
#   define fastcpy_tiny fastcpy_tiny_AVX2
#   define fastcpy      fastcpy_AVX2
#   define fast_stpcpy  fast_stpcpy_AVX2
#   define fast_wstpcpy fast_wstpcpy_AVX2
#   define fast_dstpcpy fast_dstpcpy_AVX2
#   define fast_qstpcpy fast_qstpcpy_AVX2
# elif DLLFASTCPY_LIBRARY == 1 || (DLLFASTCPY_LIBRARY == 0 && defined(__SSE2__))
#   define fastcpy_tiny fastcpy_tiny_SSE2
#   define fastcpy      fastcpy_SSE2
#   define fast_stpcpy  fast_stpcpy_SSE2
#   define fast_wstpcpy fast_wstpcpy_SSE2
#   define fast_dstpcpy fast_dstpcpy_SSE2
#   define fast_qstpcpy fast_qstpcpy_SSE2
# else
#   define fastcpy_tiny fastcpy_tiny_NO
#   define fastcpy      fastcpy_NO
#   define fast_stpcpy  fast_stpcpy_NO
#   define fast_wstpcpy fast_wstpcpy_NO
#   define fast_dstpcpy fast_dstpcpy_NO
#   define fast_qstpcpy fast_qstpcpy_NO
# endif
#endif

#ifndef FASTCPY_EXPORT
#if defined(FASTCPY_SHARED_LIBRARY)
#  define FASTCPY_EXPORT Q_DECL_EXPORT
#elif defined(FASTCPY_STATIC_LIBRARY) || defined(FASTCPY_OWNER_SOURCE)
#  define FASTCPY_EXPORT
#else
#  define FASTCPY_EXPORT Q_DECL_IMPORT
#endif
#endif // FASTCPY_EXPORT

#include <stddef.h>
#include <stdint.h>


#ifdef __cplusplus
extern "C"
{
#endif

/* 采取非临时复制的阈值, 超出此尺寸则内存复制使用 movnt 指令 */
extern FASTCPY_EXPORT size_t fastcpy_non_temporal_threshold;

/* 内存复制(可能超小尺寸内存复制会快些)，等效于`memcpy` */
FASTCPY_EXPORT void* fastcpy_tiny(void *dst, const void *src, size_t size);

/* 内存复制(更推荐)，等效于`memcpy` */
FASTCPY_EXPORT void* fastcpy(void *dst, const void *src, size_t size);

/* 字符串复制，等效于`stpcpy`，返回目标字符串的末尾 */
FASTCPY_EXPORT uint8_t* fast_stpcpy(uint8_t *dst, const uint8_t *src);

/* 字符串复制，等效于`u16_stpcpy`，返回目标字符串的末尾 */
FASTCPY_EXPORT uint16_t* fast_wstpcpy(uint16_t *dst, const uint16_t *src);

#if defined(FASTCPY_SHARED_LIBRARY) || defined(FASTCPY_STATIC_LIBRARY) || defined(__SSE2__) || defined(__AVX2__) || defined(__AVX512VL__)

/* 字符串复制，等效于`u32_stpcpy`，返回目标字符串的末尾 */
FASTCPY_EXPORT uint32_t* fast_dstpcpy(uint32_t *dst, const uint32_t *src);

/* 字符串复制，等效于`U64_stpcpy`，返回目标字符串的末尾 */
FASTCPY_EXPORT uint64_t* fast_qstpcpy(uint64_t *dst, const uint64_t *src);
#else
inline uint32_t* fast_dstpcpy(uint32_t *dst, const uint32_t *src) {
	while((*(dst++) = *(src++)) != 0);
	return --dst;
}
inline uint64_t* fast_qstpcpy(uint64_t *dst, const uint64_t *src) {
	while((*(dst++) = *(src++)) != 0);
	return --dst;
}
#endif

#ifndef FASTCPY_NO_NTA_PART
/* 字符串复制，等效于`strcpy`，调用`fast_stpcpy`的`inline`函数 */
inline uint8_t* fast_strcpy(uint8_t *dst, const uint8_t *src) {
	fast_stpcpy(dst, src);
	return dst;
}

/* 字符串复制，等效于`u16_strcpy`，调用`fast_wstpcpy`的`inline`函数 */
inline uint16_t* fast_wstrcpy(uint16_t *dst, const uint16_t *src) {
	fast_wstpcpy(dst, src);
	return dst;
}

/* 字符串复制，等效于`u32_strcpy`，调用`fast_dstpcpy`的`inline`函数 */
inline uint32_t* fast_dstrcpy(uint32_t *dst, const uint32_t *src) {
	fast_dstpcpy(dst, src);
	return dst;
}

/* 字符串复制，等效于`U64_strcpy`，调用`fast_qstpcpy`的`inline`函数 */
inline uint64_t* fast_qstrcpy(uint64_t *dst, const uint64_t *src) {
	fast_qstpcpy(dst, src);
	return dst;
}
#endif


#ifdef __cplusplus
}
#endif

#endif // LIB_FAST_CPY_H
