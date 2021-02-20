#ifndef __KF_RECORD_H__
#define __KF_RECORD_H__

#include "KFData.h"
#include "KFMap.h"

namespace KFrame
{
    class KFRecord : public KFData
    {
    public:
        KFRecord();
        virtual ~KFRecord() = default;

        virtual void Reset();

        // 是否有效
        virtual bool IsValid();

        // common
        virtual uint32 Size();

        // 最大数量
        virtual uint32 MaxSize();

        // 是否达到了最大值
        virtual bool IsFull();
        //////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual DataPtr First();
        virtual DataPtr Next();

        virtual void CopyFrom( DataPtr& other );
        virtual void SaveTo( DataPtr& other );
        //////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual DataPtr Find( uint64 key );
        virtual DataPtr Find( uint64 key, const std::string& data_name );
        virtual DataPtr Find( uint64 parent_key, uint64 child_key );
        virtual DataPtr Find( uint64 parent_key, uint64 child_key, const std::string& data_name );
        virtual DataPtr Find( uint64 parent_key, const std::string& data_name, uint64 child_key );

        virtual bool Add( uint64 key, DataPtr& data );
        virtual bool Add( uint64 parent_key, uint64 child_key, DataPtr& data );
        virtual bool Add( uint64 key, const std::string& data_name, DataPtr& data );

        virtual DataPtr Remove( uint64 key );

        virtual void Find( const std::string& data_name, uint64 value, std::list<DataPtr>& find_list, bool find_all );
        virtual bool Check( const std::string& data_name, uint64 value, const std::string& check_name, uint64 check_value );

        // 格式化成字串
        virtual std::string ToString();
        virtual void FromString( const std::string& value );

        // Map的特殊操作
        virtual void ToMap( StringMap& values );
        virtual void FromMap( const StringMap& values );

    protected:
        // 数据列表
        KFMap<uint64, KFData> _data;
    };
}


#endif
