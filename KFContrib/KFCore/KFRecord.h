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

        virtual void Reset( bool isdelete = true );

        // 是否有效
        virtual bool IsValid();

        // common
        virtual uint32 Size();

        // 是否达到了最大值
        virtual bool IsFull();
        //////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual KFData* First();
        virtual KFData* Next();

        virtual void CopyFrom( KFData* kfother );
        virtual void SaveTo( KFData* kfother );
        //////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual KFData* Find( uint64 key );
        virtual KFData* Find( uint64 key, const std::string& dataname );
        virtual KFData* Find( uint64 parentkey, uint64 childkey );
        virtual KFData* Find( uint64 parentkey, uint64 childkey, const std::string& dataname );
        virtual KFData* Find( uint64 parentkey, const std::string& dataname, uint64 childkey );

        virtual bool Add( uint64 key, KFData* data );
        virtual bool Add( uint64 parentkey, uint64 childkey, KFData* data );
        virtual bool Add( uint64 key, const std::string& dataname, KFData* data );

        virtual KFData* Move( uint64 key );
        virtual bool Remove( uint64 key );

        virtual void Find( const std::string& dataname, uint64 value, std::list< KFData* >& findlist, bool findall );
        virtual bool Check( const std::string& dataname, uint64 value, const std::string& checkname, uint64 checkvalue );

        // 格式化成字串
        virtual std::string ToString();
        virtual void FromString( const std::string& value );

        // Map的特殊操作
        virtual void ToMap( StringMap& values );
        virtual void FromMap( const StringMap& values );

    protected:
        // 列表
        KFHashMap< uint64, uint64, KFData > _data;
    };
}


#endif
