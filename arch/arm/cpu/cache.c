#include <common.h>
#include <init.h>
#include <asm/mmu.h>
#include <asm/cache.h>
#include <asm/system_info.h>

int arm_architecture;

struct cache_fns {
	void (*dma_clean_range)(unsigned long start, unsigned long end);
	void (*dma_flush_range)(unsigned long start, unsigned long end);
	void (*dma_inv_range)(unsigned long start, unsigned long end);
	void (*mmu_cache_on)(void);
	void (*mmu_cache_off)(void);
	void (*mmu_cache_flush)(void);
};

struct cache_fns *cache_fns;

#define DEFINE_CPU_FNS(arch) \
	void arch##_dma_clean_range(unsigned long start, unsigned long end);	\
	void arch##_dma_flush_range(unsigned long start, unsigned long end);	\
	void arch##_dma_inv_range(unsigned long start, unsigned long end);	\
	void arch##_mmu_cache_on(void);						\
	void arch##_mmu_cache_off(void);					\
	void arch##_mmu_cache_flush(void);					\
										\
	static struct cache_fns __maybe_unused cache_fns_arm##arch = {		\
		.dma_clean_range = arch##_dma_clean_range,			\
		.dma_flush_range = arch##_dma_flush_range,			\
		.dma_inv_range = arch##_dma_inv_range,				\
		.mmu_cache_on = arch##_mmu_cache_on,				\
		.mmu_cache_off = arch##_mmu_cache_off,				\
		.mmu_cache_flush = arch##_mmu_cache_flush,			\
	};

DEFINE_CPU_FNS(v4)
DEFINE_CPU_FNS(v5)
DEFINE_CPU_FNS(v6)
DEFINE_CPU_FNS(v7)

void __dma_clean_range(unsigned long start, unsigned long end)
{
	cache_fns->dma_clean_range(start, end);
}

void __dma_flush_range(unsigned long start, unsigned long end)
{
	cache_fns->dma_flush_range(start, end);
}

void __dma_inv_range(unsigned long start, unsigned long end)
{
	cache_fns->dma_inv_range(start, end);
}

void __mmu_cache_on(void)
{
	cache_fns->mmu_cache_on();
}

void __mmu_cache_off(void)
{
	cache_fns->mmu_cache_off();
}

void __mmu_cache_flush(void)
{
	cache_fns->mmu_cache_flush();
}

int arm_set_cache_functions(void)
{
	switch (cpu_architecture()) {
#ifdef CONFIG_CPU_32v4T
	case CPU_ARCH_ARMv4T:
		cache_fns = &cache_fns_armv4;
		break;
#endif
#ifdef CONFIG_CPU_32v5
	case CPU_ARCH_ARMv5:
	case CPU_ARCH_ARMv5T:
	case CPU_ARCH_ARMv5TE:
	case CPU_ARCH_ARMv5TEJ:
		cache_fns = &cache_fns_armv5;
		break;
#endif
#ifdef CONFIG_CPU_32v6
	case CPU_ARCH_ARMv6:
		cache_fns = &cache_fns_armv6;
		break;
#endif
#ifdef CONFIG_CPU_32v7
	case CPU_ARCH_ARMv7:
		cache_fns = &cache_fns_armv7;
		break;
#endif
	default:
		BUG();
	}

	return 0;
}
