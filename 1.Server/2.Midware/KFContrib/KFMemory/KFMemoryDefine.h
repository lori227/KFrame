#ifndef __KF_MEMORY_DEFINE_H__
#define __KF_MEMORY_DEFINE_H__

#include "KFInclude.h"
namespace KFrame
{
    // 内存信息
    class MemoryData
    {
    public:
        // 名字
        std::string _name;

        // 分配大小
        uint32 _size{ 0 };
    };

    // 内存日志信息
    class MemoryLogData
    {
    public:
        // 分配次数
        uint32 _count{ 0 };

        // 分配大小
        uint64 _total_size{ 0 };
    };

    // 线程buffer
    template< class T >
    class KFThreadBuffer
    {
    public:
        // 长度
        uint32 _length{ 0 };

        // 内存地址
        T* _buffer{ nullptr };
    };

    // 共享内存
    class KFShareMemory
    {
    public:
        // 长度
        uint32 _size{ 0 };

        // 共享内存指针
        void* _memory{ nullptr };
    };
}

#endif