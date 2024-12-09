#pragma once

#include <stdatomic.h>

/* 采用C11的stdatomic库，添加了一些便利宏 */

/* compiler barrier */
#define acq_compile() \
	atomic_signal_fence(memory_order_acquire)
#define rel_compile() \
	atomic_signal_fence(memory_order_release)
#define full_compile() \
	atomic_signal_fence(memory_order_seq_cst)
#define acq_rel_compile() \
	atomic_signal_fence(memory_order_acq_rel)

/* memory barrier */
#define acq_fence() \
	atomic_thread_fence(memory_order_acquire)
#define rel_fence() \
	atomic_thread_fence(memory_order_release)
#define full_fence() \
	atomic_thread_fence(memory_order_seq_cst)
#define consume_fence() \
	atomic_thread_fence(memory_order_consume)
#define acq_rel_fence() \
	atomic_thread_fence(memory_order_acq_rel)

/* atmoci_store */
#define atomic_store_relaxed(obj, val) \
	atomic_store_explicit(obj, val, memory_order_relaxed)
#define atomic_store_rel(obj, val) \
	atomic_store_explicit(obj, val, memory_order_release)
#define atomic_store_seq_cst(obj, val) \
	atomic_store_explicit(obj, val, memory_order_seq_cst)

/* atomic_load */
#define atomic_load_relaxed(obj) \
	atomic_load_explicit(obj, memory_order_relaxed)
#define atomic_load_acq(obj) \
	atomic_load_explicit(obj, memory_order_acquire)
#define atomic_load_seq_cst(obj) \
	atomic_load_explicit(obj, memory_order_seq_cst)
#define atomic_load_consume(obj) \
	atomic_load_explicit(obj, memory_order_consume)

/* atomic_exchange */
#define atomic_xchg_relaxed(obj, val) \
	atomic_exchange_explicit(obj, val, memory_order_relaxed)
#define atomic_xchg_acq(obj, val) \
	atomic_exchange_explicit(obj, val, memory_order_acquire)
#define atomic_xchg_rel(obj, val) \
	atomic_exchange_explicit(obj, val, memory_order_release)
#define atomic_xchg_seq_cst(obj, val) \
	atomic_exchange_explicit(obj, val, memory_order_seq_cst)
#define atomic_xchg_consume(obj, val) \
	atomic_exchange_explicit(obj, val, memory_order_consume)
#define atomic_xchg_acq_rel(obj, val) \
	atomic_exchange_explicit(obj, val, memory_order_acq_rel)

/* atomic_compare_exchange_strong */
#define atomic_cmpxchg_relaxed(obj, old, val) \
	atomic_compare_exchange_strong_explicit(obj, old, val, memory_order_relaxed, memory_order_relaxed)
#define atomic_cmpxchg_acq(obj, old, val) \
	atomic_compare_exchange_strong_explicit(obj, old, val, memory_order_acquire, memory_order_relaxed)
#define atomic_cmpxchg_rel(obj, old, val) \
	atomic_compare_exchange_strong_explicit(obj, old, val, memory_order_release, memory_order_relaxed)
#define atomic_cmpxchg_seq_cst(obj, old, val) \
	atomic_compare_exchange_strong_explicit(obj, old, val, memory_order_seq_cst, memory_order_relaxed)
#define atomic_cmpxchg_consume(obj, old, val) \
	atomic_compare_exchange_strong_explicit(obj, old, val, memory_order_consume, memory_order_relaxed)
#define atomic_cmpxchg_acq_rel(obj, old, val) \
	atomic_compare_exchange_strong_explicit(obj, old, val, memory_order_acq_rel, memory_order_relaxed)

/* atomic_compare_exchange_weak */
#define atomic_cmpxchg_weak_relaxed(obj, old, val) \
	atomic_compare_exchange_weak_explicit(obj, old, val, memory_order_relaxed, memory_order_relaxed)
#define atomic_cmpxchg_weak_acq(obj, old, val) \
	atomic_compare_exchange_weak_explicit(obj, old, val, memory_order_acquire, memory_order_relaxed)
#define atomic_cmpxchg_weak_rel(obj, old, val) \
	atomic_compare_exchange_weak_explicit(obj, old, val, memory_order_release, memory_order_relaxed)
#define atomic_cmpxchg_weak_seq_cst(obj, old, val) \
	atomic_compare_exchange_weak_explicit(obj, old, val, memory_order_seq_cst, memory_order_relaxed)
#define atomic_cmpxchg_weak_consume(obj, old, val) \
	atomic_compare_exchange_weak_explicit(obj, old, val, memory_order_consume, memory_order_relaxed)
#define atomic_cmpxchg_weak_acq_rel(obj, old, val) \
	atomic_compare_exchange_weak_explicit(obj, old, val, memory_order_acq_rel, memory_order_relaxed)

/* atomic_fetch_add */
#define atomic_add_relaxed(obj, val) \
	atomic_fetch_add_explicit(obj, val, memory_order_relaxed)
#define atomic_add_acq(obj, val) \
	atomic_fetch_add_explicit(obj, val, memory_order_acquire)
#define atomic_add_rel(obj, val) \
	atomic_fetch_add_explicit(obj, val, memory_order_release)
#define atomic_add_seq_cst(obj, val) \
	atomic_fetch_add_explicit(obj, val, memory_order_seq_cst)
#define atomic_add_consume(obj, val) \
	atomic_fetch_add_explicit(obj, val, memory_order_consume)
#define atomic_add_acq_rel(obj, val) \
	atomic_fetch_add_explicit(obj, val, memory_order_acq_rel)

/* atomic_fetch_sub */
#define atomic_sub_relaxed(obj, val) \
	atomic_fetch_sub_explicit(obj, val, memory_order_relaxed)
#define atomic_sub_acq(obj, val) \
	atomic_fetch_sub_explicit(obj, val, memory_order_acquire)
#define atomic_sub_rel(obj, val) \
	atomic_fetch_sub_explicit(obj, val, memory_order_release)
#define atomic_sub_seq_cst(obj, val) \
	atomic_fetch_sub_explicit(obj, val, memory_order_seq_cst)
#define atomic_sub_consume(obj, val) \
	atomic_fetch_sub_explicit(obj, val, memory_order_consume)
#define atomic_sub_acq_rel(obj, val) \
	atomic_fetch_sub_explicit(obj, val, memory_order_acq_rel)

/* atomic_fetch_and */
#define atomic_and_relaxed(obj, val) \
	atomic_fetch_and_explicit(obj, val, memory_order_relaxed)
#define atomic_and_acq(obj, val) \
	atomic_fetch_and_explicit(obj, val, memory_order_acquire)
#define atomic_and_rel(obj, val) \
	atomic_fetch_and_explicit(obj, val, memory_order_release)
#define atomic_and_seq_cst(obj, val) \
	atomic_fetch_and_explicit(obj, val, memory_order_seq_cst)
#define atomic_and_consume(obj, val) \
	atomic_fetch_and_explicit(obj, val, memory_order_consume)
#define atomic_and_acq_rel(obj, val) \
	atomic_fetch_and_explicit(obj, val, memory_order_acq_rel)

/* atomic_fetch_or */
#define atomic_or_relaxed(obj, val) \
	atomic_fetch_or_explicit(obj, val, memory_order_relaxed)
#define atomic_or_acq(obj, val) \
	atomic_fetch_or_explicit(obj, val, memory_order_acquire)
#define atomic_or_rel(obj, val) \
	atomic_fetch_or_explicit(obj, val, memory_order_release)
#define atomic_or_seq_cst(obj, val) \
	atomic_fetch_or_explicit(obj, val, memory_order_seq_cst)
#define atomic_or_consume(obj, val) \
	atomic_fetch_or_explicit(obj, val, memory_order_consume)
#define atomic_or_acq_rel(obj, val) \
	atomic_fetch_or_explicit(obj, val, memory_order_acq_rel)

/* atomic_fetch_xor */
#define atomic_xor_relaxed(obj, val) \
	atomic_fetch_xor_explicit(obj, val, memory_order_relaxed)
#define atomic_xor_acq(obj, val) \
	atomic_fetch_xor_explicit(obj, val, memory_order_acquire)
#define atomic_xor_rel(obj, val) \
	atomic_fetch_xor_explicit(obj, val, memory_order_release)
#define atomic_xor_seq_cst(obj, val) \
	atomic_fetch_xor_explicit(obj, val, memory_order_seq_cst)
#define atomic_xor_consume(obj, val) \
	atomic_fetch_xor_explicit(obj, val, memory_order_consume)
#define atomic_xor_acq_rel(obj, val) \
	atomic_fetch_xor_explicit(obj, val, memory_order_acq_rel)


/* atomic_flag_test_and_set */
#define atomic_flag_test_set_relaxed(obj) \
	atomic_flag_test_and_set_explicit(obj, memory_order_relaxed)
#define atomic_flag_test_set_acq(obj) \
	atomic_flag_test_and_set_explicit(obj, memory_order_acquire)
#define atomic_flag_test_set_rel(obj) \
	atomic_flag_test_and_set_explicit(obj, memory_order_release)
#define atomic_flag_test_set_seq_cst(obj) \
	atomic_flag_test_and_set_explicit(obj, memory_order_seq_cst)
#define atomic_flag_test_set_consume(obj) \
	atomic_flag_test_and_set_explicit(obj, memory_order_consume)
#define atomic_flag_test_set_acq_rel(obj) \
	atomic_flag_test_and_set_explicit(obj, memory_order_acq_rel)

/* atomic_flag_clear */
#define atomic_flag_clear_relaxed(obj) \
	atomic_flag_clear_explicit(obj, memory_order_relaxed)
#define atomic_flag_clear_rel(obj) \
	atomic_flag_clear_explicit(obj, memory_order_release)
#define atomic_flag_clear_seq_cst(obj) \
	atomic_flag_clear_explicit(obj, memory_order_seq_cst)

/* atomic_bit_set */
#define atomic_bitset_relaxed(obj, bit) \
	atomic_or_relaxed(obj, 1ULL << (bit))
#define atomic_bitset_acq(obj, bit) \
	atomic_or_acq(obj, 1ULL << (bit))
#define atomic_bitset_rel(obj, bit) \
	atomic_or_rel(obj, 1ULL << (bit))
#define atomic_bitset_seq_cst(obj, bit) \
	atomic_or_seq_cst(obj, 1ULL << (bit))
#define atomic_bitset_consume(obj, bit) \
	atomic_or_consume(obj, 1ULL << (bit))
#define atomic_bitset_acq_rel(obj, bit) \
	atomic_or_acq_rel(obj, 1ULL << (bit))

/* atomic_bit_clear */
#define atomic_bitclr_relaxed(obj, bit) \
	atomic_and_relaxed(obj, ~(1ULL << (bit)))
#define atomic_bitclr_acq(obj, bit) \
	atomic_and_acq(obj, ~(1ULL << (bit)))
#define atomic_bitclr_rel(obj, bit) \
	atomic_and_rel(obj, ~(1ULL << (bit)))
#define atomic_bitclr_seq_cst(obj, bit) \
	atomic_and_seq_cst(obj, ~(1ULL << (bit)))
#define atomic_bitclr_consume(obj, bit) \
	atomic_and_consume(obj, ~(1ULL << (bit)))
#define atomic_bitclr_acq_rel(obj, bit) \
	atomic_and_acq_rel(obj, ~(1ULL << (bit)))

/* atomic_bit_not */
#define atomic_bitnot_relaxed(obj, bit) \
	atomic_xor_relaxed(obj, 1ULL << (bit))
#define atomic_bitnot_acq(obj, bit) \
	atomic_xor_acq(obj, 1ULL << (bit))
#define atomic_bitnot_rel(obj, bit) \
	atomic_xor_rel(obj, 1ULL << (bit))
#define atomic_bitnot_seq_cst(obj, bit) \
	atomic_xor_seq_cst(obj, 1ULL << (bit))
#define atomic_bitnot_consume(obj, bit) \
	atomic_xor_consume(obj, 1ULL << (bit))
#define atomic_bitnot_acq_rel(obj, bit) \
	atomic_xor_acq_rel(obj, 1ULL << (bit))

/* atomic_increment */
#define atomic_inc_relaxed(obj) \
	atomic_fetch_add_explicit(obj, 1, memory_order_relaxed)
#define atomic_inc_acq(obj) \
	atomic_fetch_add_explicit(obj, 1, memory_order_acquire)
#define atomic_inc_rel(obj) \
	atomic_fetch_add_explicit(obj, 1, memory_order_release)
#define atomic_inc_seq_cst(obj) \
	atomic_fetch_add_explicit(obj, 1, memory_order_seq_cst)
#define atomic_inc_consume(obj) \
	atomic_fetch_add_explicit(obj, 1, memory_order_consume)
#define atomic_inc_acq_rel(obj) \
	atomic_fetch_add_explicit(obj, 1, memory_order_acq_rel)

/* atomic_decrement */
#define atomic_dec_relaxed(obj) \
	atomic_fetch_sub_explicit(obj, 1, memory_order_relaxed)
#define atomic_dec_acq(obj) \
	atomic_fetch_sub_explicit(obj, 1, memory_order_acquire)
#define atomic_dec_rel(obj) \
	atomic_fetch_sub_explicit(obj, 1, memory_order_release)
#define atomic_dec_seq_cst(obj) \
	atomic_fetch_sub_explicit(obj, 1, memory_order_seq_cst)
#define atomic_dec_consume(obj) \
	atomic_fetch_sub_explicit(obj, 1, memory_order_consume)
#define atomic_dec_acq_rel(obj) \
	atomic_fetch_sub_explicit(obj, 1, memory_order_acq_rel)

// atomic_not
// atomic_neg

