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
        kfsetting->_done_type = xmlnode.GetUInt32( "DoneType" );
        kfsetting->_done_value = xmlnode.GetUInt32( "DoneValue" );

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

        auto startpos = 0u;
        auto data = strlimit.data();
        auto size = ( uint32 )strlimit.length();

        // 读取属性
        auto dataname = KFAnalysis::ReadLetter( data, size, startpos );
        if ( dataname.empty() )
        {
            return false;
        }

        // 读取判断符号
        auto checktype = 0u;
        auto checkendpos = 0u;
        auto checksize = 0u;
        std::tie( checktype, checkendpos, checksize ) = KFAnalysis::ReadCheckType( data, size, startpos );
        if ( checktype == KFEnum::Null )
        {
            return false;
        }

        // 读取数值
        startpos = checkendpos + checksize + 1;
        size = ( uint32 )strlimit.length() - startpos + 1;
        auto datavalue = KFAnalysis::ReadInteger( data, size, startpos );
        if ( datavalue == 0u )
        {
            return false;
        }

        KFConditionLimit limit;
        limit._data_name = dataname;
        limit._data_value = datavalue;
        limit._operate = checktype;
        kfsetting->_limits.push_back( limit );
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
        }
    }

    uint64 KFConditionSetting::GetLimitValue( const std::string& name ) const
    {
        for ( auto& limitdata : _limits )
        {
            if ( limitdata._data_name == name )
            {
                return limitdata._data_value;
            }
        }

        return 0u;
    }

    uint64 KFConditionTrigger::CalcUpdateValue( uint64 operate, uint64 operatevalue, uint64 nowvalue ) const
    {
        if ( _trigger_type != 0u && _trigger_type != operate )
        {
            return 0u;
        }

        // 触发值
        if ( _use_type == KFConditionEnum::UseFinal )
        {
            operatevalue = nowvalue;
        }

        if ( _trigger_check != 0u )
        {
            auto ok = KFUtility::CheckOperate<uint64>( operatevalue, _trigger_check, _trigger_value );
            if ( !ok )
            {
                return 0u;
            }
        }

        // 最终使用值
        if ( _use_value != 0u )
        {
            operatevalue = _use_value;
        }

        return operatevalue;
    }
}