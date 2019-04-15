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
        virtual ~KFRecord();

        virtual void Reset();

        // 是否有效
        virtual bool IsValid();

        // common
        virtual uint32 Size();


        //////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual KFData* FirstData();
        virtual KFData* NextData();

        virtual void CopyFrom( KFData* kfother );
        virtual void SaveTo( KFData* kfother );
        //////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual KFData* FindData( uint64 key );
        virtual KFData* FindData( uint64 key, const std::string& dataname );
        virtual KFData* FindData( uint64 parentkey, uint64 childkey );
        virtual KFData* FindData( uint64 parentkey, uint64 childkey, const std::string& dataname );
        virtual KFData* FindData( uint64 parentkey, const std::string& dataname, uint64 childkey );

        virtual bool AddData( uint64 key, KFData* data );
        virtual bool AddData( uint64 parentkey, uint64 childkey, KFData* data );
        virtual bool AddData( uint64 key, const std::string& dataname, KFData* data );

        virtual bool RemoveData( uint64 key );
        virtual bool RemoveData( uint64 key, const std::string& dataname );

        virtual void FindData( const std::string& dataname, uint64 value, std::list< KFData* >& findlist );
        virtual bool CheckData( const std::string& dataname, uint64 value, const std::string& checkname, uint64 checkvalue );

        // 格式化成字串
        virtual std::string ToString();
        virtual void FromString( const std::string& value );

        // Map的特殊操作
        virtual void ToMap( MapString& values );
        virtual void FromMap( const MapString& values );

    protected:
        // 列表
        KFHashMap< uint64, uint64, KFData > _data;
    };
}


#endif
