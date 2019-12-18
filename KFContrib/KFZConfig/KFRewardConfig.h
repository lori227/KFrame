#ifndef __KF_REWARD_CONFIG_H__
#define __KF_REWARD_CONFIG_H__

#include "KFCore/KFElement.h"
#include "KFZConfig/KFConfig.h"
#include "KFZConfig/KFSetting.h"
#include "KFProtocol/KFProtocol.h"

namespace KFrame
{
    class KFRewardSetting : public KFIntSetting
    {
    public:
        // 属性名
        std::string _name;

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
            switch ( type )
            {
            case KFMsg::CurrenyType:
                return type * 1000u + code;
            case KFMsg::ItemType:
                return type * 1000u;
            default:
                break;
            }

            return _invalid_int;
        }

        // 获取奖励字符串
        const std::string& GetRewardString( uint32 type, uint32 code, uint32 num )
        {
            static std::string _str_reward;
            _str_reward.clear();

            auto id = GetRewardId( type, code );
            auto kfsetting = FindSetting( id );
            if ( kfsetting != nullptr )
            {
                switch ( type )
                {
                case KFMsg::CurrenyType:
                    _str_reward = __FORMAT__( kfsetting->_element_template, num );
                    break;
                case KFMsg::ItemType:
                    _str_reward = __FORMAT__( kfsetting->_element_template, code, num );
                    break;
                default:
                    break;
                }
            }

            return _str_reward;
        }

        // 解析奖励字符串
        bool ParseRewards( const std::string& strdata, KFElements& kfelement )
        {
            if ( strdata.empty() )
            {
                return true;
            }

            // 将字符串解析成数组
            __JSON_PARSE_STRING__( kfjson, strdata );
            if ( !kfjson.IsArray() )
            {
                return false;
            }

            std::string strelement = "[";
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

                auto type = jsonarray[ 0 ].GetUint();
                auto code = jsonarray[ 1 ].GetUint();
                auto num = jsonarray[ 2 ].GetUint();

                auto& strreward = GetRewardString( type, code, num );
                if ( strreward.empty() )
                {
                    return false;
                }

                if ( i > 0u )
                {
                    strelement += ",";
                }

                strelement += strreward;
            }

            strelement += "]";
            return kfelement.Parse( strelement, __FUNC_LINE__ );
        }

        const KFRewardSetting* FindSettingByName( const std::string& name )
        {
            auto iter = _name_id_list.find( name );
            if ( iter == _name_id_list.end() )
            {
                return nullptr;
            }

            return FindSetting( iter->second );
        }

    protected:
        virtual void ClearSetting()
        {
            _name_id_list.clear();
            KFConfigT< KFRewardSetting >::ClearSetting();
        }

        // 读取配置
        virtual void ReadSetting( KFNode& xmlnode, KFRewardSetting* kfsetting )
        {
            kfsetting->_type = xmlnode.GetUInt32( "Type" );
            kfsetting->_code = xmlnode.GetUInt32( "Code" );
            kfsetting->_name = xmlnode.GetString( "Name" );

            auto id = GetRewardId( kfsetting->_type, kfsetting->_code );
            if ( id != kfsetting->_id )
            {
                __LOG_ERROR__( "id=[{}] reward config is error", kfsetting->_id );
            }

            kfsetting->_element_template = xmlnode.GetString( "Template" );
            _name_id_list[ kfsetting->_name ] = id;
        }

    private:
        KeyValue _name_id_list;
    };
}

#endif