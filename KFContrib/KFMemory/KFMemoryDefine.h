#ifndef __KF_MEMORY_DEFINE_H__
#define __KF_MEMORY_DEFINE_H__

#include "KFDefine.h"
namespace KFrame
{
    // �ڴ���Ϣ
    class MemoryData
    {
    public:
        // ����
        std::string _name;

        // �����С
        uint32 _size{ 0 };
    };

    // �ڴ���־��Ϣ
    class MemoryLogData
    {
    public:
        // �������
        uint32 _count{ 0 };

        // �����С
        uint64 _total_size{ 0 };
    };

    // �߳�buffer
    template< class T >
    class KFThreadBuffer
    {
    public:
        // ����
        uint32 _length{ 0 };

        // �ڴ��ַ
        T* _buffer{ nullptr };
    };

    // �����ڴ�
    class KFShareMemory
    {
    public:
        // ����
        uint32 _size{ 0 };

        // �����ڴ�ָ��
        void* _memory{ nullptr };
    };
}

#endif