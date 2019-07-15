#ifndef __KF_OBJECT_H__
#define __KF_OBJECT_H__

#include "KFData.h"
#include "KFMap.h"

namespace KFrame
{
    class KFObject : public KFData
    {
    public:
        KFObject() = default;
        virtual ~KFObject() = default;

        // 重置
        virtual void Reset();

        // 是否有效
        virtual bool IsValid();

        // 数量
        virtual uint32 Size();

        // 初始化数值
        virtual void InitData();

        // 获得key
        virtual uint64 GetKeyID();
        virtual void SetKeyID( uint64 id );
        //////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual KFData* FirstData();
        virtual KFData* NextData();

        virtual void CopyFrom( KFData* kfother );
        virtual void SaveTo( KFData* kfother );
        //////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual KFData* FindData( const std::string& dataname );
        virtual KFData* FindData( const std::string& dataname, uint64 key );
        virtual KFData* FindData( const std::string& parentname, const std::string& childname );
        virtual KFData* FindData( const std::string& parentname, uint64 key, const std::string& childname );
        virtual KFData* FindData( const std::string& parentname, const std::string& childname, uint64 key );
        virtual KFData* FindData( const std::string& parentname, const std::string& childname, const std::string& dataname );

        virtual bool AddData( const std::string& dataname, KFData* data );
        virtual bool AddData( const std::string& dataname, uint64 key, KFData* data );
        virtual bool AddData( const std::string& parentname, const std::string& childname, KFData* data );

        virtual KFData* MoveData( const std::string& dataname );
        virtual bool RemoveData( const std::string& dataname );
        virtual bool RemoveData( const std::string& dataname, uint64 key );
        virtual bool RemoveData( const std::string& parentname, const std::string& childname );
        //////////////////////////////////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////////////////////////////////
        // 格式化成字串
        virtual std::string ToString();
        virtual void FromString( const std::string& value );

        // Map的特殊操作
        virtual void ToMap( MapString& values );
        virtual void FromMap( const MapString& values );

    protected:
        // key
        uint64 _key = 0u;

        // 列表
        KFHashMap< std::string, const std::string&, KFData > _data;
    };
}


#endif
