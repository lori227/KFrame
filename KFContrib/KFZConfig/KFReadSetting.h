#ifndef __KF_READ_SETTING_H__
#define __KF_READ_SETTING_H__

#include "KFExecuteData.h"

namespace KFrame
{
    class KFReadSetting
    {
    public:
        // 解析条件列表
        static uint32 ParseConditionList( std::string strcondition, VectorUInt32& conditionlist )
        {
            auto conditiontype = KFEnum::ABit;
            if ( !strcondition.empty() )
            {
                if ( strcondition.find( DEFAULT_SPLIT_STRING ) != std::string::npos )
                {
                    conditiontype = KFEnum::ABit;
                    conditionlist = KFUtility::SplitList< VectorUInt32 >( strcondition, DEFAULT_SPLIT_STRING );
                }
                else if ( strcondition.find( FUNCTION_AND_STRING ) != std::string::npos )
                {
                    conditiontype = KFEnum::And;
                    conditionlist = KFUtility::SplitList< VectorUInt32 >( strcondition, FUNCTION_AND_STRING );
                }
                else
                {
                    conditiontype = KFEnum::ABit;
                    auto conditionid = KFUtility::ToValue<uint32>( strcondition );
                    if ( conditionid != 0u )
                    {
                        conditionlist.push_back( conditionid );
                    }
                }
            }

            return conditiontype;
        }

        // 读取MapUInt32
        static void ParseMapUInt32( std::string strvalue, MapUInt32& values )
        {
            values.clear();
            while ( !strvalue.empty() )
            {
                auto strchild = KFUtility::SplitString( strvalue, DEFAULT_SPLIT_STRING );
                if ( !strchild.empty() )
                {
                    auto key = KFUtility::SplitValue<uint32>( strchild, FUNCTION_LINK_STRING );
                    auto value = KFUtility::SplitValue<uint32>( strchild, FUNCTION_LINK_STRING );
                    if ( key != 0u && value != 0u )
                    {
                        values[ key ] = value;
                    }
                }
            }
        }
        //////////////////////////////////////////////////////////////////////////////////////////////////
        // 读取执行配置
        static void ReadExecuteData( KFNode& xmlnode, KFExecuteData* kfsetting )
        {
            kfsetting->Reset();
            kfsetting->_name = xmlnode.GetString( "ExecuteName" );

            auto index = 1u;
            while ( true )
            {
                auto strkey = __FORMAT__( "ExecuteParam{}", index++ );
                auto ok = xmlnode.HaveChild( strkey.c_str() );
                if ( !ok )
                {
                    break;
                }

                auto strvalue = xmlnode.GetString( strkey.c_str() );
                auto param = kfsetting->_param_list.AddParam();
                param->_str_value = strvalue;
                param->_int_value = KFUtility::ToValue<uint32>( strvalue );
                ParseMapUInt32( strvalue, param->_map_value );
            }
        }
    };

}

#endif