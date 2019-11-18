#include "KFConditionConfig.hpp"
#include "KFCore/KFAnalysis.hpp"

namespace KFrame
{
    void KFConditionDefineConfig::ReadSetting( KFNode& xmlnode, KFConditionDefine* kfsetting )
    {
        if ( kfsetting->_parent_name.empty() )
        {
            kfsetting->_parent_name = xmlnode.GetString( "ParentName" );
            kfsetting->_data_name = xmlnode.GetString( "DataName" );
            kfsetting->_init_calc_type = xmlnode.GetUInt32( "InitCalc" );
        }

        KFConditionTrigger trigger;
        trigger._call_type = xmlnode.GetUInt32( "CallType" );
        trigger._trigger_type = xmlnode.GetUInt32( "TriggerType" );
        trigger._trigger_check = xmlnode.GetUInt32( "TriggerCheck" );
        trigger._trigger_use = xmlnode.GetUInt32( "TriggerUse" );
        trigger._trigger_value = xmlnode.GetUInt32( "TriggerValue" );

        trigger._use_type = xmlnode.GetUInt32( "UseType" );
        trigger._use_value = xmlnode.GetUInt32( "UseValue" );
        trigger._use_operate = xmlnode.GetUInt32( "UseOperate" );
        kfsetting->_trigger_list.push_back( trigger );
    }
    //////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////
    static std::map< std::string, uint32 > _conditon_mask_list =
    {
        {"Calculate", KFConditionEnum::LimitStatus},
        {"PlaceLimit", KFConditionEnum::LimitPlace},
    };
    void KFConditionConfig::ReadSetting( KFNode& xmlnode, KFConditionSetting* kfsetting )
    {
        kfsetting->_limits.clear();
        kfsetting->_str_condition = xmlnode.GetString( "Condition" );
        kfsetting->_done_value = xmlnode.GetUInt32( "DoneValue" );
        kfsetting->_str_clean = xmlnode.GetString( "Clean", true );

        auto strdonetype = xmlnode.GetString( "DoneType" );
        kfsetting->_done_type = KFAnalysis::GetCheckType( strdonetype );
        if ( kfsetting->_done_type == KFEnum::Null )
        {
            kfsetting->_done_type = KFUtility::ToValue<uint32>( strdonetype );
        }

        for ( auto& iter : _conditon_mask_list )
        {
            if ( xmlnode.GetString( iter.first.c_str(), true ) == "1" )
            {
                KFUtility::AddBitMask< uint32 >( kfsetting->_limit_mask, iter.second );
            }
        }

        ListString keylist;
        xmlnode.GetKeyList( keylist );
        for ( auto& strkey : keylist )
        {
            if ( strkey.find( "Limits" ) != std::string::npos )
            {
                auto strlimit = xmlnode.GetString( strkey.c_str() );
                auto ok = AnalysisLimit( kfsetting, strlimit );
                if ( !ok )
                {
                    __LOG_ERROR__( "limit=[{}] analysis failed!", strlimit );
                }
            }
        }
    }

    bool KFConditionConfig::AnalysisLimit( KFConditionSetting* kfsetting, const std::string& strlimit )
    {
        if ( strlimit.empty() )
        {
            return true;
        }

        KFConditionLimits limits;

        auto startpos = 0u;
        auto data = strlimit.data();
        auto size = ( uint32 )strlimit.length();

        do
        {
            // 默认限制为id=1111, 如果第一个是数字, 则使用默认字段
            auto datavalue = 0u;
            uint32 checktype = KFEnum::Equal;
            auto dataname = __STRING__( id );

            if ( KFAnalysis::IsInteger( data[ startpos ] ) )
            {
                datavalue = KFAnalysis::ReadInteger( data, size, startpos );
                if ( datavalue == 0u )
                {
                    return false;
                }
            }
            else
            {
                // 读取属性
                dataname = KFAnalysis::ReadLetter( data, size, startpos );
                if ( dataname.empty() )
                {
                    return false;
                }

                // 读取判断符号
                auto checkendpos = 0u;
                auto checksize = 0u;
                std::tie( checktype, checkendpos, checksize ) = KFAnalysis::ReadCheckType( data, size, startpos );
                if ( checktype == KFEnum::Null )
                {
                    return false;
                }

                // 读取数值
                startpos = checkendpos + checksize + 1;
                size -= checksize;
                datavalue = KFAnalysis::ReadInteger( data, size, startpos );
                if ( datavalue == 0u )
                {
                    return false;
                }
            }

            // 加入列表
            KFConditionLimit limit;
            limit._data_name = dataname;
            limit._operate = checktype;
            limit._data_value = datavalue;
            limits._limit.push_back( limit );

            // 读取连接符号
            if ( size != 0u )
            {
                auto value = data[ startpos ];
                auto linktype = KFAnalysis::GetLinkType( value );
                if ( linktype == KFEnum::Null )
                {
                    return false;
                }

                limits._link_type = linktype;
                startpos += 1;
                size -= 1;
            }
        } while ( size > 0 );

        kfsetting->_limits.push_back( limits );
        return true;
    }

    void KFConditionConfig::LoadAllComplete()
    {
        for ( auto& iter : _settings._objects )
        {
            auto kfsetting = iter.second;
            kfsetting->_condition_define = KFConditionDefineConfig::Instance()->FindSetting( kfsetting->_str_condition );
            if ( kfsetting->_condition_define == nullptr )
            {
                __LOG_ERROR__( "condition=[{}] can't find define=[{}]!", kfsetting->_id, kfsetting->_str_condition );
            }

            kfsetting->_clean_define = nullptr;
            if ( !kfsetting->_str_clean.empty() )
            {
                kfsetting->_clean_define = KFConditionDefineConfig::Instance()->FindSetting( kfsetting->_str_clean );
                if ( kfsetting->_clean_define == nullptr )
                {
                    __LOG_ERROR__( "condition=[{}] can't find define=[{}]!", kfsetting->_id, kfsetting->_str_clean );
                }
            }
        }
    }

    bool KFConditionTrigger::CalcUpdateValue( uint64 operate, uint64& operatevalue, uint64 nowvalue ) const
    {
        if ( _trigger_type != 0u && _trigger_type != operate )
        {
            return false;
        }

        // 触发值
        if ( _trigger_check != 0u )
        {
            auto triggervalue = operatevalue;
            if ( _trigger_use == KFConditionEnum::UseFinal )
            {
                triggervalue = nowvalue;
            }

            auto ok = KFUtility::CheckOperate<uint64>( triggervalue, _trigger_check, _trigger_value );
            if ( !ok )
            {
                return false;
            }
        }

        // 最终使用值
        switch ( _use_type )
        {
        case KFConditionEnum::UseSetting:
            operatevalue = _use_value;
            break;
        case KFConditionEnum::UseFinal:
            operatevalue = nowvalue;
            break;
        default:
            break;
        }

        return true;
    }
}