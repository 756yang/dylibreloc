#ifndef LIB_FAST_CPY_H
#define LIB_FAST_CPY_H

/* fastcpy 动态库的重定位函数声明, 使用函数指针和数据指针用以存储符号地址 */

#include <stddef.h>
#include <stdint.h>


#ifdef __cplusplus
extern "C"
{
#endif

/* 隐式链接动态库时, 没必要重定向其导出变量, 显示加载动态库的重定向暂未完善 */
//extern size_t *__ptr_fastcpy_non_temporal_threshold;
/* 采取非临时复制的阈值, 超出此尺寸则内存复制使用 movnt 指令 */
//#define fastcpy_non_temporal_threshold (*__ptr_fastcpy_non_temporal_threshold)
#if defined(_MSC_VER) || defined(WIN64) || defined(_WIN64) || defined(__WIN64__) || defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
extern __declspec(dllimport) size_t fastcpy_non_temporal_threshold;
#else
extern __attribute__((visibility("default"))) size_t fastcpy_non_temporal_threshold;
#endif

/* 内存复制(可能超小尺寸内存复制会快些)，等效于`memcpy` */
extern void* (*fastcpy_tiny)(void *dst, const void *src, size_t size);

/* 内存复制(更推荐)，等效于`memcpy` */
extern void* (*fastcpy)(void *dst, const void *src, size_t size);

/* 字符串复制，等效于`stpcpy`，返回目标字符串的末尾 */
extern uint8_t* (*fast_stpcpy)(uint8_t *dst, const uint8_t *src);

/* 字符串复制，等效于`u16_stpcpy`，返回目标字符串的末尾 */
extern uint16_t* (*fast_wstpcpy)(uint16_t *dst, const uint16_t *src);

/* 字符串复制，等效于`u32_stpcpy`，返回目标字符串的末尾 */
extern uint32_t* (*fast_dstpcpy)(uint32_t *dst, const uint32_t *src);

/* 字符串复制，等效于`U64_stpcpy`，返回目标字符串的末尾 */
extern uint64_t* (*fast_qstpcpy)(uint64_t *dst, const uint64_t *src);

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

#ifdef __cplusplus
}
#endif


#endif // LIB_FAST_CPY_H
