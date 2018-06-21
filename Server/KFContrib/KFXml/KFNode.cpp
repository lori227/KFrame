#include "KFNode.h"
#include "KFDefine.h"
#include "KFXml.h"
#include "xml/rapidxml.hpp"
#include "xml/rapidxml_print.hpp"
#include "xml/rapidxml_utils.hpp"
#include "KFUtility/KFUtility.h"
#include "KFConvert/KFConvert.h"
#include "KFLogger/KFLogger.h"

namespace KFrame
{
	KFNode::KFNode( KFXml* kfxml )
	{
		_kf_xml = kfxml;
		_node = nullptr;
	}

	KFNode::~KFNode()
	{
		_node = nullptr;
	}

	bool KFNode::IsValid()
	{
		return _node != nullptr;
	}

	void KFNode::NextNode()
	{
		auto xmlnode = reinterpret_cast<rapidxml::xml_node<>*>(_node);
		_node = xmlnode->next_sibling();
	}

	KFNode KFNode::FindNode( const char* key )
	{
		auto xmlnode = reinterpret_cast<rapidxml::xml_node<>*>(_node);
		
		KFNode kfnode( _kf_xml );
		kfnode._node = xmlnode->first_node( key );
		return kfnode;
	}

	bool KFNode::GetBoolen( const char* key, bool optional /* = false */ )
	{
		auto xmlnode = reinterpret_cast<rapidxml::xml_node<>*>(_node);

		auto attribute = xmlnode->first_attribute( key );
		if ( attribute == nullptr )
		{
			if ( !optional )
			{
				KFLogger::LogInit( KFLogger::Error, "[%s] can't find node = [%s]!", _kf_xml->GetFileName(), key );
			}
			return false;
		}

		return KFUtility::ToValue<uint32>( attribute->value() ) == 1;
	}

	uint32 KFNode::GetUInt32( const char* key, bool optional /* = false */, uint32 defaultvalue /* = 0 */ )
	{
		auto xmlnode = reinterpret_cast<rapidxml::xml_node<>*>(_node);

		auto attribute = xmlnode->first_attribute( key );
		if ( attribute == nullptr )
		{
			if ( !optional )
			{
				KFLogger::LogInit( KFLogger::Error, "[%s] can't find node = [%s]!", _kf_xml->GetFileName(), key );
			}
			return defaultvalue;
		}

		return KFUtility::ToValue<uint32>( attribute->value() );
	}
	
	std::string KFNode::GetString( const char* key, bool optional /* = false */ )
	{
		auto xmlnode = reinterpret_cast<rapidxml::xml_node<>*>(_node);

		auto attribute = xmlnode->first_attribute( key );
		if (attribute == nullptr )
		{
			if ( !optional )
			{
				KFLogger::LogInit( KFLogger::Error, "[%s] can't find node = [%s]!", _kf_xml->GetFileName(), key );
			}
			return _invalid_str;
		}

		return attribute->value();
	}

	double KFNode::GetDouble( const char* key, bool optional /* = false */, double defaultvalue /* = 0.0f */ )
	{
		auto xmlnode = reinterpret_cast< rapidxml::xml_node<>* >( _node );

		auto attribute = xmlnode->first_attribute( key );
		if ( attribute == nullptr )
		{
			if ( !optional )
			{
				KFLogger::LogInit( KFLogger::Error, "[%s] can't find node = [%s]!", _kf_xml->GetFileName(), key );
			}
			return defaultvalue;
		}

		return atof( attribute->value() );
	}

}
