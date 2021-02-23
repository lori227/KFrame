#ifndef __KF_MEMORY_DEFINE_H__
#define __KF_MEMORY_DEFINE_H__

#include "KFDefine.h"

namespace KFrame
{
    // 内存数据
    class MemoryData
    {
    public:
        // 内存名称
        std::string _name;

        // 内存大小
        uint32 _size = 0;
    };

    // 内存日志数据
    class MemoryLogData
    {
    public:
        // 内存申请次数
        uint32 _count = 0;

        // 内存使用大小
        uint64 _total_size = 0;
    };

    // 线程buffer
    template<class T>
    class KFThreadBuffer
    {
    public:
        // 长度
        uint32 _length = 0;

        // 内存数据
        T* _data = nullptr;
    };

    // 共享内存
    class KFShareMemory
    {
    public:
        // 大小
        uint32 _size = 0;

        // 内存数据
        void* _data = nullptr;
    };
}
#endif