#include "KFXml.h"
#include "xml/rapidxml.hpp"
#include "xml/rapidxml_print.hpp"
#include "xml/rapidxml_utils.hpp"

namespace KFrame
{
    KFXml::KFXml( const std::string& file )
    {
        _file = file;
        _data = new rapidxml::file<>();
        _document = new rapidxml::xml_document<>();

        Parse();
    }

    KFXml::~KFXml()
    {
        delete reinterpret_cast<rapidxml::file<>*>( _data );
        delete reinterpret_cast<rapidxml::xml_document<>*>( _document );
    }

    const char* KFXml::GetFileName() const
    {
        return _file.c_str();
    }

    const char* KFXml::Data()
    {
        auto xmlfile = reinterpret_cast< rapidxml::file<>* >( _data );
        return xmlfile->data();
    }

    uint32 KFXml::Size()
    {
        auto xmlfile = reinterpret_cast< rapidxml::file<>* >( _data );
        return static_cast< uint32 >( xmlfile->size() );
    }

    void KFXml::Parse()
    {
        auto xmlfile = reinterpret_cast<rapidxml::file<>*>( _data );
        auto xmldocument = reinterpret_cast<rapidxml::xml_document<>*>( _document );

        xmlfile->from( _file.c_str() );
        xmldocument->parse<0>( xmlfile->data() );
    }

    KFNode KFXml::RootNode()
    {
        auto xmldocument = reinterpret_cast<rapidxml::xml_document<>*>( _document );

        KFNode kfnode( this );
        kfnode._node = xmldocument->first_node();
        return kfnode;
    }

    KFNode KFXml::FindNode( const char* key )
    {
        auto xmldocument = reinterpret_cast<rapidxml::xml_document<>*>( _document );

        KFNode kfnode( this );
        kfnode._node = xmldocument->first_node( key );
        return kfnode;
    }
}
