#ifndef __KF_XML_NODE_H__
#define __KF_XML_NODE_H__

#include "KFInclude.h"

namespace KFrame
{
    /////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////
    class KFXml;
    class KFNode
    {
    public:
        KFNode( KFXml* kfxml );
        ~KFNode();

        // 是否有效
        bool IsValid();

        // 兄弟节点
        void NextNode( const char* key = nullptr );

        // 查找子节点
        KFNode FindNode( const char* key );
        /////////////////////////////////////////////////////////////////////////////////////
        /////////////////////////////////////////////////////////////////////////////////////
        template< class T >
        T ReadT( const char* key, bool optional = false )
        {
            return GetUInt32( key, optional );
        }

        // 读取bool
        bool GetBoolen( const char* key, bool optional = false, bool defaultvale = false );

        // 读取int32
        int32 GetInt32( const char* key, bool optional = false, int32 defaultvalue = 0 );

        // 读取uint32
        uint32 GetUInt32( const char* key, bool optional = false, uint32 defaultvalue = 0 );

        // 读取uint64
        uint64 GetUInt64( const char* key, bool optional = false, uint64 defaultvalue = 0 );

        // 读取string
        std::string GetString( const char* key, bool optional = false, std::string defaultvalue = "" );

        // 读取double
        double GetDouble( const char* key, bool optional = false, double defaultvalue = 0.0f );

        // 读取uint32 vector
        UInt32Vector& GetUInt32Vector( const char* key, bool optional = false );

        // 读取uint32 map
        UInt32Map& GetUInt32Map( const char* key, bool optional = false );

        // 读取uint32 set
        UInt32Set& GetUInt32Set( const char* key, bool optional = false );

        // 读取string set
        StringSet& GetStringSet( const char* key, bool optional = false );

        // 读取<string,uint64>
        StringUInt64& GetStringUInt64( const char* key, bool optional = false );

        // 读取<uint64,string>
        UInt64String& GetUInt64String( const char* key, bool optional = false );
        /////////////////////////////////////////////////////////////////////////////////////
        /////////////////////////////////////////////////////////////////////////////////////
        void GetKeyList( StringList& outlist );

        // 是否有子属性
        bool HaveChild( const char* key );

    private:
        friend class KFXml;
        KFXml* _kf_xml;
        // 节点
        void* _node;
    };

    template<>
    inline bool KFNode::ReadT( const char* key, bool optional /* = false */ )
    {
        return GetBoolen( key, optional );
    }

    template<>
    inline int32 KFNode::ReadT( const char* key, bool optional /* = false */ )
    {
        return GetInt32( key, optional );
    }

    template<>
    inline uint32 KFNode::ReadT( const char* key, bool optional /* = false */ )
    {
        return GetUInt32( key, optional );
    }

    template<>
    inline uint64 KFNode::ReadT( const char* key, bool optional /* = false */ )
    {
        return GetUInt64( key, optional );
    }

    template<>
    inline std::string KFNode::ReadT( const char* key, bool optional /* = false */ )
    {
        return GetString( key, optional );
    }

    template<>
    inline double KFNode::ReadT( const char* key, bool optional /* = false */ )
    {
        return GetDouble( key, optional );
    }
}

#endif
