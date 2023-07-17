#ifndef TINYSTL_ALLOC_H_
#define TINYSTL_ALLOC_H_

// 这个头文件包含一个类 alloc，用于分配和回收内存，以内存池的方式实现

#include <new>        // placement new
#include <cstddef>    // ptrdiff_t, size_t
#include <cstdio>     // std::fprintf
#include <cstdlib>    // std::malloc, std::free

namespace tinystl {

    // 二级空间配置器的实现内容
    enum {__ALIGN = 8};  // 小型区块的上调边界
    enum {__MAX_BYTES = 128};  // 小型区块的上限
    enum {__NFREELISTS = __MAX_BYTES / __ALIGN};  // free-lists 个数

    /// @brief 共用体 FreeList，采用链表的方式管理内存碎片，分配与回收小内存区块
    //  这里未使用 volatile，因为不涉及多线程情况，详见 note2.md
    union FreeList {
        union FreeList* next;  // 指向下一个区块
        char data[1];  // 本区块的起始位置
    };

    class alloc {
        private:
            static char* start_free;  
            static char* end_free;  
            static size_t heap_size;  

            static FreeList* free_list[__NFREELISTS];  // 自由链表

        public:
            static void* allocate(size_t n);
            static void deallocate(void* p, size_t n);
            static void* reallocate(void* p, size_t old_sz, size_t new_sz);

        private:
            static size_t ROUND_UP(size_t bytes);  // 上调边界至 8 的倍数
            static size_t FREELIST_INDEX(size_t bytes);  // 根据区块大小计算 free-lists 的下标
            static void* refill(size_t n);  // 重新填充 free-lists
            static char* chunk_alloc(size_t size, size_t &nobjs);  // 从内存池中取空间给 free-lists 使用
    };

    // 静态成员变量的初始化

    char* alloc::start_free = nullptr;  // 内存池起始位置
    char* alloc::end_free = nullptr;  // 内存池结束位置
    size_t alloc::heap_size = 0;  // 申请 heap 空间附加值的大小

    FreeList* alloc::free_list[__NFREELISTS] = {
        nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,
        nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr
    };

    /// @brief 分配大小为 n 的空间
    /// @param n  分配空间的大小 
    /// @return 分配的空间的首地址
    void* alloc::allocate(size_t n) {
        FreeList** my_free_list;  // 指向对应的 free-lists 指针的指针
        FreeList* result;  // 返回的空间的首地址
        
        // 如果 n 大于 128，直接调用一级配置器
        if (n > static_cast<size_t>(__MAX_BYTES)) {
            // TODO: 一级配置器
            return std::malloc(n); 
        }

        // 找到对应的 free-lists
        my_free_list = free_list + FREELIST_INDEX(n);
        result = *my_free_list;

        // 如果 free-lists 为空，重新填充 free-lists
        if (result == nullptr) {
            void* r = refill(ROUND_UP(n));
            return r;
        }
        *my_free_list = result->next;  // 更新 free-lists 首地址，指向下一个区块
        return result;
    }

    /// @brief 释放 p 指向的大小为 n 的空间
    /// @param p  指向空间的首地址
    /// @param n  空间的大小
    void alloc::deallocate(void* p, size_t n) {
        // 如果 n 大于 128，直接调用一级配置器释放内存
        if (n > static_cast<size_t>(__MAX_BYTES)) {
            // TODO: 一级配置器 
            std::free(p);
            return;
        }
        
        // 将 p 转换为 FreeList* 类型，关于 reinterpret_cast 的使用，详见 note2.md
        FreeList* q = reinterpret_cast<FreeList*>(p);  
        FreeList** my_free_list;
        
        my_free_list = free_list + FREELIST_INDEX(n);  // 找到对应的 free-lists
        q->next = *my_free_list;  // 将 q 插入到 free-lists 的头部
        *my_free_list = q;  // 更新 free-lists 首地址
    }

    /// @brief 重新分配大小为 new_sz 的空间
    /// @param p  指向空间的首地址
    /// @param old_sz  旧空间的大小
    /// @param new_sz  新空间的大小
    /// @return  新空间的首地址
    void* alloc::reallocate(void* p, size_t old_sz, size_t new_sz) {
        deallocate(p, old_sz);
        p = allocate(new_sz);
        return p;
    }

    /// @brief 将 bytes 上调至对应区间大小
    /// @param bytes 申请区块大小
    /// @return  上调后的空间大小
    size_t alloc::ROUND_UP(size_t bytes) {
        return (((bytes) + __ALIGN - 1) & ~(__ALIGN - 1));
    }

    /// @brief 根据区块大小计算 free-lists 的下标
    /// @param bytes  区块大小
    /// @return  free-lists 的下标
    size_t alloc::FREELIST_INDEX(size_t bytes) {
        return (((bytes) + __ALIGN - 1) / __ALIGN - 1);
    }

    /// @brief 重新填充 free-lists
    /// @param n 申请区块大小
    /// @return 申请到的一个区块的首地址
    void* alloc::refill(size_t n) {
        size_t nblock = 20;  // 一次性申请的区块数量
        char* chunk = chunk_alloc(n, nblock);
        FreeList** my_free_list;
        FreeList* result, *cur, *next;  // 这里要注意 * 的修饰范围

        // 如果只申请到一个区块，直接返回
        if (nblock == 1) return chunk;

        my_free_list = free_list + FREELIST_INDEX(n);
        result = (FreeList*) chunk;  // 返回的空间的首地址
        // 由于将第一个区块作为返回值，因此需要将 free-lists 的首地址偏移 n 个字节，指向下一个区块
        *my_free_list = next = (FreeList*)(chunk + n);
        // 将剩余的区块插入到 free-lists 中
        for (size_t i = 1; ; i++) {
            cur = next;
            next = (FreeList*)((char*)next + n);  // 指向下一个区块
            if (nblock - 1 == i) {
                cur->next = nullptr;
                break;
            }
            cur->next = next;
        }

        return result;
    }

    char* alloc::chunk_alloc(size_t size, size_t& nblock) {
        char* result;
        size_t total_bytes = size * nblock;  // 需要申请的空间大小
        size_t bytes_left = end_free - start_free;  // 内存池剩余空间大小

        // 如果内存池剩余空间足够，直接返回内存池的首地址
        if (bytes_left >= total_bytes) {
            result = start_free;
            start_free += total_bytes;
            return result;
        }

        // 如果内存池剩余空间不足，但是剩余空间足够一个区块，直接返回内存池的首地址
        else if (bytes_left >= size) {
            nblock = bytes_left / size;
            total_bytes = size * nblock;
            result = start_free;
            start_free += total_bytes;
            return result;
        }

        // 如果内存池剩余大小连一个区块都无法满足
        else {
            // 如果内存池还有剩余，把剩余的空间加入到 free-list 中
            if (bytes_left > 0) {
                FreeList** my_free_list = free_list + FREELIST_INDEX(bytes_left);
                ((FreeList*) start_free)->next = *my_free_list;
                *my_free_list = (FreeList*)start_free;
            }
            
            // malloc申请 heap 中两倍+额外大小的内存
            size_t bytes_to_get = (total_bytes << 1) + ROUND_UP(heap_size >> 4);
            start_free = (char *)std::malloc(bytes_to_get);  // TODO:一级配置器

            // heap 空间不足，malloc 失败
            if (start_free == 0) {
                FreeList** my_free_list, *p;
                // 在 free-list 中查找是否有尚未用过且足够大的区块
                for (size_t i = size; i < __MAX_BYTES; i+= __ALIGN) {
                    my_free_list = free_list + FREELIST_INDEX(i);
                    p = *my_free_list;
                    // 尚有未用的区块
                    if (p != 0) {
                        *my_free_list = p->next;
                        start_free = (char*)p;
                        end_free = start_free + i;  // 可用的地址范围
                        return chunk_alloc(size, nblock);  // 递归调用
                    }
                }
                // 如果一点内存都没有了的话, 就只有调用一级配置器来申请内存了, 并且用户没有设置处理例程就抛出异常
                std::printf("out of memory!");
                end_free = nullptr;
                throw std::bad_alloc();  // TODO:一级配置器及 handle
            }

            // 申请内存成功后重新修改内存起始地址和结束地址, 重新调用chunk_alloc分配内存
	    	heap_size += bytes_to_get;
	    	end_free = start_free + bytes_to_get;
    		return(chunk_alloc(size, nblock));
        }
    }
}

#endif