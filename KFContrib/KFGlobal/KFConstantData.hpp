#ifndef __KF_CONSTANT_DATA_H__
#define __KF_CONSTANT_DATA_H__

#include "KFMap.h"
#include "KFConstant.h"

namespace KFrame
{
    class KFConstantData
    {
    public:
        //////////////////////////////////////////////////////////////////////////////////////////////////
        // 添加常量
        void AddConstant( const std::string& name, uint32 key, uint32 value );
        void AddConstant( const std::string& name, uint32 key, double value );
        void AddConstant( const std::string& name, uint32 key, const std::string& value );
        //////////////////////////////////////////////////////////////////////////////////////////////////
        // 获得常量
        std::shared_ptr<const KFConstant> FindConstant( const std::string& name, uint32 key = 0u );
        //////////////////////////////////////////////////////////////////////////////////////////////////
        // uint32配置
        uint32 GetUInt32( const std::string& name, uint32 key = 0u );

        // double配置
        double GetDouble( const std::string& name, uint32 key = 0u );

        // string配置
        const std::string& GetString( const std::string& name, uint32 key = 0u );
        //////////////////////////////////////////////////////////////////////////////////////////////////
    protected:
        typedef std::pair< std::string, uint32 > ConstantKey;

        // 常量列表
        KFMap< ConstantKey, KFConstant > _constant_list;
    };
}

#endif
