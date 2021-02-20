#ifndef __KF_OBJECT_H__
#define __KF_OBJECT_H__

#include "KFData.h"
#include "KFMap.h"

namespace KFrame
{
    class KFObject : public KFData
    {
    public:
        KFObject();
        virtual ~KFObject() = default;

        // 重置
        virtual void Reset();

        // 是否有效
        virtual bool IsValid();

        // 数量
        virtual uint32 Size();

        // 初始化数值
        virtual void InitData();

        // config id
        virtual uint32 GetConfigID();

        // 获得key
        virtual uint64 GetKeyID();
        virtual void SetKeyID( uint64 id );
        //////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual DataPtr First();
        virtual DataPtr Next();

        virtual void CopyFrom( DataPtr& other );
        virtual void SaveTo( DataPtr& other );
        //////////////////////////////////////////////////////////////////////////////////////////////////////

        virtual DataPtr Find( const std::string& data_name );
        virtual DataPtr Find( const std::string& data_name, uint64 key );
        virtual DataPtr Find( const std::string& parent_name, const std::string& child_name );
        virtual DataPtr Find( const std::string& parent_name, uint64 key, const std::string& child_name );
        virtual DataPtr Find( const std::string& parent_name, const std::string& child_name, uint64 key );
        virtual DataPtr Find( const std::string& parent_name, const std::string& child_name, const std::string& data_name );

        virtual bool Add( const std::string& data_name, DataPtr& data );
        virtual bool Add( const std::string& data_name, uint64 key, DataPtr& data );
        virtual bool Add( const std::string& parent_name, const std::string& child_name, DataPtr& data );

        virtual DataPtr Remove( const std::string& data_name );
        virtual DataPtr Remove( const std::string& data_name, uint64 key );

        //////////////////////////////////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////////////////////////////////
        // 格式化成字串
        virtual std::string ToString();
        virtual void FromString( const std::string& value );

        // Map的特殊操作
        virtual void ToMap( StringMap& values );
        virtual void FromMap( const StringMap& values );

    protected:
        // key
        uint64 _key = 0u;

        // 列表
        KFHashMap< std::string, KFData > _data;
    };
}


#endif
