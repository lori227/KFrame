#include "KFAgent.h"
#include "KFEnum.h"
#include "KFData.h"
#include "KFJson.h"

namespace KFrame
{
    KFAgentValue::KFAgentValue()
    {
        _min_value = 0;
        _max_value = 0;
        _use_value = 0;
    }

    bool KFAgentValue::SetValue( const std::string& dataname, std::string value )
    {
        _data_name = dataname;

        _min_value = KFUtility::SplitValue<uint64>( value, FUNCTION_RANGE_STRING );
        _max_value = KFUtility::SplitValue<uint64>( value, FUNCTION_RANGE_STRING );
        if ( _min_value >= _max_value )
        {
            _max_value = _min_value;
            _use_value = _min_value;
        }

        return _min_value != _max_value;
    }

    uint64 KFAgentValue::GetValue( float multiple )
    {
        if ( _min_value >= _max_value )
        {
            _use_value = _min_value;
        }
        else
        {
            _use_value = KFGlobal::Instance()->RandInRange( static_cast< uint32 >( _min_value ), static_cast< uint32 >( _max_value ), 1 );
        }

        _use_value = static_cast< uint64 >( _use_value * multiple );
        return _use_value;
    }

    uint64 KFAgentValue::GetUseValue()
    {
        return _use_value;
    }

    ///////////////////////////////////////////////////////////////////
    KFAgent::KFAgent()
    {
        _config_id = _invalid_int;
        _operate = KFOperateEnum::Add;
    }

    KFAgent::~KFAgent()
    {
        _datas.Clear();
    }

    KFAgentValue* KFAgent::FindAgentValue( const std::string& dataname )
    {
        return _datas.Find( dataname );
    }

    uint64 KFAgent::GetValue( const std::string& value )
    {
        auto kfvalue = FindAgentValue( value );
        if ( kfvalue == nullptr )
        {
            return _invalid_int;
        }

        return kfvalue->GetValue();
    }

    uint64 KFAgent::GetUseValue( const std::string& value )
    {
        auto kfvalue = FindAgentValue( value );
        if ( kfvalue == nullptr )
        {
            return _invalid_int;
        }

        return kfvalue->GetUseValue();
    }

    bool KFAgent::IsEligibilityData( KFData* kfdata, const std::string& exclude )
    {
        for ( auto& iter : _datas._objects )
        {
            auto& name = iter.first;
            if ( name == exclude )
            {
                continue;
            }

            auto agentvalue = iter.second->GetValue();
            auto value = kfdata->GetValue( name );
            if ( value < agentvalue )
            {
                return false;
            }
        }

        return true;
    }

    void KFAgent::AddValue( const std::string& dataname, std::string value )
    {
        auto kfagentvalue = _datas.Create( dataname );
        kfagentvalue->SetValue( dataname, value );

        _kf_agents->_is_add_value = true;
    }

    /////////////////////////////////////////////////////////////////////////////////////
    KFAgents::KFAgents()
    {
        _is_rand_agent = false;
        _is_add_value = false;
    }

    KFAgents::~KFAgents()
    {
        for ( auto kfagent : _agents )
        {
            __KF_DELETE__( KFAgent, kfagent );
        }
        _agents.clear();
    }

    bool KFAgents::ParseFromString( const std::string& agentstring, const char* function, uint32 line )
    {
        if ( agentstring.empty() )
        {
            return false;
        }

        _agents.clear();
        _string = agentstring;

        //{ "money":1111, "diamon":2222, "item":[{"id":1,"count":2 },{"id":3,"count":4}] }

        try
        {
            __JSON_PARSE_STRING__( kfjson, agentstring );
            if ( kfjson.HasParseError() )
            {
                return false;
            }

            //for ( auto iter = kfjson.MemberBegin(); iter != kfjson.MemberEnd(); ++i )
            //{

            //}

            //for ( auto iter = kfjson.begin(); iter != kfjson.end(); ++iter )
            //{
            //    if ( iter->empty() )
            //    {
            //        continue;
            //    }

            //    auto begin = iter->begin();
            //    auto& kfobject = *begin;
            //    if ( kfobject.isNull() )
            //    {
            //        continue;
            //    }

            //    auto kfagent = __KF_NEW__( KFAgent );
            //    kfagent->_string = iter->toStyledString();
            //    kfagent->_data_name = begin.name();

            //    for ( auto citer = kfobject.begin(); citer != kfobject.end(); ++citer )
            //    {
            //        auto name = citer.name();
            //        auto value = citer->asString();

            //        if ( name == __KF_STRING__( id ) )
            //        {
            //            kfagent->_config_id = KFUtility::ToValue< uint32 >( value );
            //        }
            //        else
            //        {
            //            auto kfvalue = __KF_NEW__( KFAgentValue );
            //            _is_rand_agent |= kfvalue->SetValue( name, value );
            //            kfagent->_datas.Insert( name, kfvalue );
            //        }
            //    }

            //    kfagent->_kf_agents = this;
            //    _agents.push_back( kfagent );
            //}
        }
        catch ( ... )
        {
            __LOG_ERROR_FUNCTION__( function, line, "[{}] parse agent failed!", agentstring );
            return false;
        }

        return true;
    }

    void KFAgents::SetOperate( uint32 operate )
    {
        for ( auto kfagent : _agents )
        {
            kfagent->_operate = operate;
        }
    }

    const std::string& KFAgents::FormatRewardAgent( float multiple ) const
    {
        if ( !_is_rand_agent && !_is_add_value && multiple == 1.0f )
        {
            return _string;
        }

        //// 重新格式化
        static std::string _show_string;
        //_show_string.clear();

        //KFJson kfjson;
        //for ( auto kfagent : _agents )
        //{
        //    KFJson kfdata;
        //    for ( auto& iter : kfagent->_datas._objects )
        //    {
        //        auto kfvalue = iter.second;
        //        kfdata[ iter.first ] = __TO_STRING__( kfvalue->GetUseValue() );
        //    }

        //    if ( kfagent->_config_id != _invalid_int )
        //    {
        //        kfdata[ __KF_STRING__( id ) ] = __TO_STRING__( kfagent->_config_id );
        //    }

        //    KFJson kfreward;
        //    kfreward[ kfagent->_data_name ] = kfdata;
        //    kfjson.append( kfreward );
        //}

        //_show_string = kfjson.Serialize();
        //KFUtility::ReplaceString( _show_string, ",", "|" );

        return _show_string;
    }
}
