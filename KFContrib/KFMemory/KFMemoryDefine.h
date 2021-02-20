#ifndef __KF_MEMORY_DEFINE_H__
#define __KF_MEMORY_DEFINE_H__

#include "KFDefine.h"
namespace KFrame
{
    // 内存数据
    class MemoryData
    {
    public:
        // 对象名
        std::string _name;

        // 内存大小
        uint32 _size{ 0 };
    };

    // 内存日志数据
    class MemoryLogData
    {
    public:
        // 当前使用数量
        uint32 _count{ 0 };

        // 当前使用的总量
        uint64 _total_size{ 0 };
    };

    // 线程buffer
    template<class T>
    class KFThreadBuffer
    {
    public:
        // 长度
        uint32 _length{ 0 };

        // 数据块
        T* _buffer{ nullptr };
    };

    // 共享内存
    class KFShareMemory
    {
    public:
        // 大小
        uint32 _size{ 0 };

        // 内存数据
        void* _memory{ nullptr };
    };
}

#endif