#ifndef __KF_ARRAY_H__
#define __KF_ARRAY_H__

#include "KFData.h"
#include "KFVector.h"

namespace KFrame
{
    class KFArray : public KFData
    {
    public:
        KFArray() = default;
        virtual ~KFArray() = default;

        virtual void Reset();

        // 是否有效
        virtual bool IsValid();

        // 初始化
        virtual void Initialize( const KFClassSetting* classsetting, const KFDataSetting* datasetting );

        // 初始化数值
        virtual void InitData();

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

        virtual bool AddData( uint64 key, KFData* data );
        virtual bool AddData( uint64 key, const std::string& dataname, KFData* data );

        virtual bool RemoveData( uint64 key );
        virtual bool RemoveData( uint64 key, const std::string& dataname );

        // 格式化成字串
        virtual std::string ToString();
        virtual void FromString( const std::string& value );

    protected:
        // 列表
        KFVector< KFData > _data;
    };
}


#endif
