#ifndef __KF_FILE_H__
#define __KF_FILE_H__

#include "KFInclude.h"

namespace KFrame
{
    class KFAttribute
    {
    public:
        std::string _name;
        std::string _type;
        std::string _comment;
        bool _is_client = true;
        bool _is_server = true;
    };

    class KFClass
    {
    public:
        KFAttribute* GetAttribute( uint32 index )
        {
            auto iter = _attributes.find( index );
            if ( iter == _attributes.end() )
            {
                return nullptr;
            }

            return &iter->second;
        }

        void AddAttribute( uint32 index, KFAttribute& attribute )
        {
            _attributes[ index ] = attribute;
        }

    public:
        // 类名
        std::string _class_name;

        // 属性列表
        std::unordered_map< uint32, KFAttribute > _attributes;
    };

    /////////////////////////////////////////////////////////////////////////////////////////////
    class KFData
    {
    public:
        void AddData( uint32 index, const std::string& value )
        {
            _datas[ index ] = value;
        }

        bool IsValid()
        {
            for ( auto& iter : _datas )
            {
                if ( !iter.second.empty() )
                {
                    return true;
                }
            }

            return false;
        }

    public:
        std::unordered_map< uint32, std::string > _datas;
    };
    /////////////////////////////////////////////////////////////////////////////////////////////

    /////////////////////////////////////////////////////////////////////////////////////////////
    class KFFile
    {
    public:
        KFFile()
        {
        }

        // 添加数据属性
        bool AddData( KFData& data );
        bool AddData( uint32 index, const std::string& value );

    public:
        // 类定义
        KFClass _class;

        // 列表
        std::vector< KFData > _datas;
    };
    /////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////

}
#endif
