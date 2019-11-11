#ifndef __KF_REWARD_CONFIG_H__
#define __KF_REWARD_CONFIG_H__

#include "KFZConfig/KFConfig.h"
#include "KFZConfig/KFSetting.h"
#include "KFCore/KFElement.h"
#include "KFProtocol/KFProtocol.h"

namespace KFrame
{
    class KFRewardSetting : public KFIntSetting
    {
    public:
        // 类型
        uint32 _type;

        // 编码
        uint32 _code;

        // 格式化字串
        std::string _element_template;
    };

    class KFRewardConfig : public KFConfigT< KFRewardSetting >, public KFInstance< KFRewardConfig >
    {
    public:
        KFRewardConfig()
        {
            _file_name = "reward";
        }

        // 获取奖励对应id
        uint32 GetRewardId( uint32 type, uint32 code )
        {
            if ( type == KFMsg::CurrenyType )
            {
                return type * 1000u + code;
            }
            else if ( type == KFMsg::ItemType )
            {
                return type * 1000u;
            }

            return _invalid_int;
        }

        // 获取奖励字符串
        bool GetRewardStr( uint32 type, uint32 code, uint32 num, std::string& rewardstr )
        {
            auto id = GetRewardId( type, code );
            auto kfsetting = FindSetting( id );
            if ( kfsetting == nullptr )
            {
                return false;
            }

            if ( type == KFMsg::CurrenyType )
            {
                rewardstr = __FORMAT__( kfsetting->_element_template, num );
            }
            else if ( type == KFMsg::ItemType )
            {
                rewardstr = __FORMAT__( kfsetting->_element_template, code, num );
            }

            return true;
        }

        // 解析奖励字符串
        bool ParseRewards( const std::string& name, KFElements& kfelement )
        {
            if ( name.empty() )
            {
                return true;
            }

            // 将字符串解析成数组
            __JSON_PARSE_STRING__( kfjson, name );
            if ( !kfjson.IsArray() )
            {
                return false;
            }

            std::string elementstr;
            auto size = __JSON_ARRAY_SIZE__( kfjson );
            for ( uint32 i = 0u; i < size; ++i )
            {
                auto& jsonarray = __JSON_ARRAY_INDEX__( kfjson, i );
                if ( !jsonarray.IsArray() )
                {
                    return false;
                }

                auto len = __JSON_ARRAY_SIZE__( jsonarray );
                if ( len != 3u )
                {
                    return false;
                }

                auto type = jsonarray[0].GetUint();
                auto code = jsonarray[1].GetUint();
                auto num = jsonarray[2].GetUint();

                std::string rewardstr;
                if ( !GetRewardStr( type, code, num, rewardstr ) )
                {
                    return false;
                }

                if ( i == 0u )
                {
                    elementstr += "[";
                }

                if ( i == size - 1u )
                {
                    elementstr += ( rewardstr + "]" );
                }
                else
                {
                    elementstr += ( rewardstr + "," );
                }

            }

            if ( !kfelement.Parse( elementstr, __FUNC_LINE__ ) )
            {
                return false;
            }

            return true;
        }


    protected:
        // 读取配置
        virtual void ReadSetting( KFNode& xmlnode, KFRewardSetting* kfsetting )
        {
            kfsetting->_type = xmlnode.GetUInt32( "Type" );
            kfsetting->_code = xmlnode.GetUInt32( "Code" );

            auto id = GetRewardId( kfsetting->_type, kfsetting->_code );
            if ( id != kfsetting->_id )
            {
                __LOG_ERROR__( "id=[{}] reward config is error", kfsetting->_id );
            }

            kfsetting->_element_template = xmlnode.GetString( "Template" );
        }
    };
}

#endif