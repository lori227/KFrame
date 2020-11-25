#include "KFXmlNode.h"
#include "KFrame.h"
#include "xml/rapidxml.hpp"
#include "xml/rapidxml_print.hpp"
#include "xml/rapidxml_utils.hpp"
#include "KFStaticAnalysis.hpp"

namespace KFrame
{
    KFXmlNode::KFXmlNode( KFXml* kfxml )
    {
        _kf_xml = kfxml;
        _node = nullptr;
    }

    KFXmlNode::~KFXmlNode()
    {
        _node = nullptr;
    }

    bool KFXmlNode::IsValid()
    {
        return _node != nullptr;
    }

    void KFXmlNode::NextNode( const char* key /* = nullptr */ )
    {
        auto xmlnode = reinterpret_cast<rapidxml::xml_node<>*>( _node );
        _node = xmlnode->next_sibling( key );
    }

    KFXmlNode KFXmlNode::FindNode( const char* key )
    {
        auto xmlnode = reinterpret_cast<rapidxml::xml_node<>*>( _node );

        KFXmlNode kfnode( _kf_xml );
        kfnode._node = xmlnode->first_node( key );
        return kfnode;
    }

    bool KFXmlNode::ReadBoolen( const char* key, bool optional /* = false */, bool defaultvale /* = false */ )
    {
        auto xmlnode = reinterpret_cast<rapidxml::xml_node<>*>( _node );

        auto attribute = xmlnode->first_attribute( key );
        if ( attribute == nullptr )
        {
            if ( !optional )
            {
                __LOG_ERROR__( "[{}] can't find node = [{}]", _kf_xml->GetFileName(), key );
            }
            return defaultvale;
        }

        return __TO_UINT32__( attribute->value() ) == 1;
    }

    int32 KFXmlNode::ReadInt32( const char* key, bool optional /* = false */, int32 defaultvalue /* = 0 */ )
    {
        auto xmlnode = reinterpret_cast< rapidxml::xml_node<>* >( _node );

        auto attribute = xmlnode->first_attribute( key );
        if ( attribute == nullptr )
        {
            if ( !optional )
            {
                __LOG_ERROR__( "[{}] can't find node = [{}]", _kf_xml->GetFileName(), key );
            }
            return defaultvalue;
        }

        return __TO_INT32__( attribute->value() );
    }


    uint32 KFXmlNode::ReadUInt32( const char* key, bool optional /* = false */, uint32 defaultvalue /* = 0 */ )
    {
        auto xmlnode = reinterpret_cast<rapidxml::xml_node<>*>( _node );

        auto attribute = xmlnode->first_attribute( key );
        if ( attribute == nullptr )
        {
            if ( !optional )
            {
                __LOG_ERROR__( "[{}] can't find node = [{}]", _kf_xml->GetFileName(), key );
            }
            return defaultvalue;
        }

        return __TO_UINT32__( attribute->value() );
    }

    uint64 KFXmlNode::ReadUInt64( const char* key, bool optional /* = false */, uint64 defaultvalue /* = 0 */ )
    {
        auto xmlnode = reinterpret_cast< rapidxml::xml_node<>* >( _node );

        auto attribute = xmlnode->first_attribute( key );
        if ( attribute == nullptr )
        {
            if ( !optional )
            {
                __LOG_ERROR__( "[{}] can't find node = [{}]", _kf_xml->GetFileName(), key );
            }
            return defaultvalue;
        }

        return __TO_UINT64__( attribute->value() );
    }

    std::string KFXmlNode::ReadString( const char* key, bool optional /* = false */, std::string defaultvalue/* = _invalid_string */ )
    {
        auto xmlnode = reinterpret_cast<rapidxml::xml_node<>*>( _node );

        auto attribute = xmlnode->first_attribute( key );
        if ( attribute == nullptr )
        {
            if ( !optional )
            {
                __LOG_ERROR__( "[{}] can't find node = [{}]", _kf_xml->GetFileName(), key );
            }

            return defaultvalue;
        }

        return attribute->value();
    }

    double KFXmlNode::ReadDouble( const char* key, bool optional /* = false */, double defaultvalue /* = 0.0f */ )
    {
        auto xmlnode = reinterpret_cast< rapidxml::xml_node<>* >( _node );

        auto attribute = xmlnode->first_attribute( key );
        if ( attribute == nullptr )
        {
            if ( !optional )
            {
                __LOG_ERROR__( "[{}] can't find node = [{}]", _kf_xml->GetFileName(), key );
            }
            return defaultvalue;
        }

        return atof( attribute->value() );
    }

    // 读取uint32 vector
    UInt32Vector& KFXmlNode::ReadUInt32Vector( const char* key, bool optional )
    {
        static UInt32Vector _result;
        _result.clear();

        auto strdata = ReadString( key, optional );
        KFUtility::SplitList( _result, strdata, __SPLIT_STRING__ );
        return _result;
    }

    // 读取uint32 map
    UInt32Map& KFXmlNode::ReadUInt32Map( const char* key, bool optional )
    {
        static UInt32Map _result;
        _result.clear();

        auto strline = ReadString( key, optional );
        while ( !strline.empty() )
        {
            auto strdata = KFUtility::SplitString( strline, __SPLIT_STRING__ );
            if ( !strdata.empty() )
            {
                auto key = KFUtility::SplitValue<uint32>( strdata, __DOMAIN_STRING__ );
                auto value = KFUtility::SplitValue<uint32>( strdata, __DOMAIN_STRING__ );
                _result[ key ] = value;
            }
        }

        return _result;
    }

    // 读取uint32 set
    UInt32Set& KFXmlNode::ReadUInt32Set( const char* key, bool optional )
    {
        static UInt32Set _result;
        _result.clear();

        auto strdata = ReadString( key, optional );
        KFUtility::SplitSet( _result, strdata, __SPLIT_STRING__ );
        return _result;
    }

    // 读取string set
    StringSet& KFXmlNode::ReadStringSet( const char* key, bool optional )
    {
        static StringSet _result;
        _result.clear();

        auto strdata = ReadString( key, optional );
        KFUtility::SplitSet( _result, strdata, __SPLIT_STRING__ );
        return _result;
    }

    // 读取string list
    StringList& KFXmlNode::ReadStringList( const char* key, bool optional )
    {
        static StringList _result;
        _result.clear();

        auto strdata = ReadString( key, optional );
        KFUtility::SplitList( _result, strdata, __SPLIT_STRING__ );
        return _result;
    }

    // 读取string vector
    StringVector& KFXmlNode::ReadStringVector( const char* key, bool optional )
    {
        static StringVector _result;
        _result.clear();

        auto strdata = ReadString( key, optional );
        KFUtility::SplitList( _result, strdata, __SPLIT_STRING__ );
        return _result;
    }

    // 读取<string,uint64>
    StringUInt64& KFXmlNode::ReadStringUInt64( const char* key, bool optional )
    {
        static StringUInt64 _result;
        _result.clear();

        auto strline = ReadString( key, optional );
        while ( !strline.empty() )
        {
            auto strdata = KFUtility::SplitString( strline, __SPLIT_STRING__ );
            if ( !strdata.empty() )
            {
                auto key = KFUtility::SplitString( strdata, __DOMAIN_STRING__ );
                auto value = KFUtility::SplitValue<uint32>( strdata, __DOMAIN_STRING__ );
                _result[ key ] = value;
            }
        }

        return _result;
    }

    // 读取<uint64,string>
    UInt64String& KFXmlNode::ReadUInt64String( const char* key, bool optional )
    {
        static UInt64String _result;
        _result.clear();

        auto strline = ReadString( key, optional );
        while ( !strline.empty() )
        {
            auto strdata = KFUtility::SplitString( strline, __SPLIT_STRING__ );
            if ( !strdata.empty() )
            {
                auto key = KFUtility::SplitValue<uint32>( strdata, __DOMAIN_STRING__ );
                auto value = KFUtility::SplitString( strdata, __DOMAIN_STRING__ );
                _result[ key ] = value;
            }
        }

        return _result;
    }

    void KFXmlNode::GetKeyList( StringList& outlist )
    {
        outlist.clear();

        auto xmlnode = reinterpret_cast< rapidxml::xml_node<>* >( _node );
        auto attribute = xmlnode->first_attribute();
        while ( attribute != nullptr )
        {
            outlist.push_back( attribute->name() );
            attribute = attribute->next_attribute();
        }
    }

    bool KFXmlNode::HaveChild( const char* key )
    {
        auto xmlnode = reinterpret_cast< rapidxml::xml_node<>* >( _node );
        auto attribute = xmlnode->first_attribute( key );
        return attribute != nullptr;
    }

    DynamicConditionGroupPtr KFXmlNode::ReadDynamicConditionGroup( const char* key, bool optional /* = false */ )
    {
        DynamicConditionGroupPtr conditiongroup( new KFDynamicConditionGroup() );
        conditiongroup->_type = KFEnum::Or;

        auto strline = ReadString( key, optional );
        if ( !strline.empty() )
        {
            if ( strline.find( __OR_STRING__ ) != std::string::npos )
            {
                conditiongroup->_type = KFEnum::Or;
                KFUtility::SplitList( conditiongroup->_ids, strline, __OR_STRING__ );
            }
            else if ( strline.find( __AND_STRING__ ) != std::string::npos )
            {
                conditiongroup->_type = KFEnum::And;
                KFUtility::SplitList( conditiongroup->_ids, strline, __AND_STRING__ );
            }
            else
            {
                auto conditionid = __TO_UINT32__( strline );
                if ( conditionid != 0u )
                {
                    conditiongroup->_ids.push_back( conditionid );
                }
            }
        }

        return conditiongroup;
    }

    ExecuteDataPtr KFXmlNode::ReadExecuteData( const char* key, bool optional /* = false */ )
    {
        ExecuteDataPtr executedata( new KFExecuteData() );
        executedata->_name = ReadString( key );

        auto index = 1u;
        while ( true )
        {
            auto strkey = __FORMAT__( "ExecuteParam{}", index++ );
            auto ok = HaveChild( strkey.c_str() );
            if ( !ok )
            {
                break;
            }

            auto param = executedata->_param_list.AddParam();
            param->_str_value = ReadString( strkey.c_str() );
            param->_int_value = ReadUInt32( strkey.c_str() );
            param->_map_value = ReadUInt32Map( strkey.c_str() );
            param->_vector_value = ReadUInt32Vector( strkey.c_str() );
        }

        return executedata;
    }

    StaticConditionsPtr KFXmlNode::ReadStaticConditions( const char* key, bool optional /* = false */ )
    {
        auto strcondition = ReadString( key, optional );
        return KFStaticConditionAnalysis::Parse( strcondition );
    }

    StaticConditionListPtr KFXmlNode::ReadStaticConditionList( const char* key, bool optional /* = false */ )
    {
        StaticConditionListPtr conditionlist( new KFStaticConditionList() );
        conditionlist->_check_type = KFEnum::And;

        auto strcondition = ReadString( key );
        auto conditions = KFStaticConditionAnalysis::Parse( strcondition );
        conditionlist->_conditions_list.push_back( conditions );

        auto index = 1u;
        while ( true )
        {
            auto strkey = __FORMAT__( "{}{}", key, index++ );
            auto ok = HaveChild( strkey.c_str() );
            if ( !ok )
            {
                break;
            }

            auto strcondition = ReadString( strkey.c_str() );
            auto conditions = KFStaticConditionAnalysis::Parse( strcondition );
            conditionlist->_conditions_list.push_back( conditions );
        }

        return conditionlist;
    }

    uint64 KFXmlNode::ReadDate( const char* key, bool optional /* = false */ )
    {
        auto strdata = ReadString( key, optional );
        return KFDate::FromString( strdata );
    }

    uint32 KFXmlNode::ReadOperateType( const char* key, bool optional /* = false */ )
    {
        auto strtemp = ReadString( key, optional );
        if ( strtemp.empty() )
        {
            return KFEnum::Null;
        }

        auto type = KFStaticConditionAnalysis::GetOperatorType( strtemp.at( 0 ) );
        if ( type == KFEnum::Null )
        {
            type = __TO_UINT32__( strtemp );
        }

        return type;
    }

    uint32 KFXmlNode::ReadCheckType( const char* key, bool optional /* = false */ )
    {
        auto strtemp = ReadString( key, optional );
        auto type = KFStaticConditionAnalysis::GetCheckType( strtemp );
        if ( type == KFEnum::Null )
        {
            type = __TO_UINT32__( strtemp );
        }

        return type;
    }

    KFRange<uint32> KFXmlNode::ReadRange( const char* key, bool optional /* = false */ )
    {
        KFRange<uint32> range;
        auto strtemp = ReadString( key, optional );

        range._min_value = KFUtility::SplitValue<uint32>( strtemp, __RANGE_STRING__ );
        range._max_value = KFUtility::SplitValue<uint32>( strtemp, __RANGE_STRING__ );
        if ( range._max_value < range._min_value )
        {
            range._max_value = range._min_value;
        }

        return range;
    }

    std::string KFXmlNode::ReadChannelString( const char* key, bool optional /* = false */ )
    {
        auto strtemp = ReadString( key, optional );
        auto result = KFUtility::FormatConfigFile( strtemp, KFGlobal::Instance()->_channel, KFGlobal::Instance()->_service );
        return result;
    }
}
