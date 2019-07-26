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

        // 读取bool
        bool GetBoolen( const char* key, bool optional = false, bool defaultvale = false );

        // 读取int32
        int32 GetInt32( const char* key, bool optional = false, int32 defaultvalue = 0 );

        // 读取uint32
        uint32 GetUInt32( const char* key, bool optional = false, uint32 defaultvalue = 0 );

        // 读取uint64
        uint64 GetUInt64( const char* key, bool optional = false, uint64 defaultvalue = 0 );

        // 读取string
        std::string GetString( const char* key, bool optional = false );

        // 读取double
        double GetDouble( const char* key, bool optional = false, double defaultvalue = 0.0f );

        /////////////////////////////////////////////////////////////////////////////////////
        /////////////////////////////////////////////////////////////////////////////////////
        void GetKeyList( std::list< std::string >& outlist );

    private:
        friend class KFXml;
        KFXml* _kf_xml;
        // 节点
        void* _node;
    };
}

#endif
