#ifndef __KF_ARRAY_H__
#define __KF_ARRAY_H__

#include "KFData.h"
#include "KFVector.h"

namespace KFrame
{
    class KFArray : public KFData
    {
    public:
        KFArray();
        virtual ~KFArray() = default;

        virtual void Reset();

        // 是否有效
        virtual bool IsValid();

        // 初始化数值
        virtual void InitData();

        // common
        virtual uint32 Size();
        virtual uint32 MaxSize();

        // 重置数量
        virtual void Resize( uint32 size );

        // 是否达到了最大值
        virtual bool IsFull();

        // 获得空位置
        virtual uint32 GetEmpty();

        // 获得索引位置
        virtual uint32 GetIndex( uint64 value );

        // 添加数值
        virtual DataPtr Insert( uint64 value );
        //////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual DataPtr First();
        virtual DataPtr Next();

        virtual void CopyFrom( DataPtr& other );
        virtual void SaveTo( DataPtr& other );
        //////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual DataPtr Find( uint64 key );
        virtual DataPtr Find( uint64 key, const std::string& data_name );

        virtual bool Add( uint64 key, DataPtr& data );
        virtual bool Add( uint64 key, const std::string& data_name, DataPtr& data );

        virtual DataPtr Remove( uint64 key );
        //////////////////////////////////////////////////////////////////////////////////////////////////////

        // 格式化成字串
        virtual std::string ToString();
        virtual void FromString( const std::string& value );

    protected:
        // 列表
        KFVector< KFData > _data;
    };
}


#endif
