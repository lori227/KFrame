#ifndef __KF_CONFIG_H__
#define __KF_CONFIG_H__

#include "KFrame.h"
#include "KFSetting.h"

namespace KFrame
{
    ///////////////////////////////////////////////////////////////
    class KFConfig
    {
    public:
        KFConfig() = default;
        virtual ~KFConfig() = default;

        ////////////////////////////////////////////////////////////////////////////
        // 加载配置
        virtual void LoadConfig( const std::string& file, uint32 loadmask ) {};

        // 加载完配置
        virtual void LoadComplete() {}

        // 所有配置加载完
        virtual void LoadAllComplete() {}
    public:
        // 默认配置文件名
        std::string _file_name;

        // 版本号
        std::string _version;
    };
    ///////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////
    template< class T >
    class KFIntConfigT : public KFConfig
    {
    public:
        // 加载配置
        void LoadConfig( const std::string& file, uint32 loadmask )
        {
            KFXml kfxml( file );
            auto config = kfxml.RootNode();
            _version = config.GetString( "version" );

            CheckClearSetting( loadmask );

            auto xmlnode = config.FindNode( "item" );
            while ( xmlnode.IsValid() )
            {
                auto kfsetting = CreateSetting( xmlnode );
                if ( kfsetting != nullptr )
                {
                    ReadSetting( xmlnode, kfsetting );
                }
                xmlnode.NextNode();
            }
        }

        // 获取配置
        const T* FindSetting( uint64 id )
        {
            return _settings.Find( id );
        }

    protected:
        void CheckClearSetting( uint32 loadmask )
        {
            if ( KFUtility::HaveBitMask<uint32>( loadmask, KFConfigEnum::NeedClearData ) )
            {
                ClearSetting();
            }
        }

        virtual void ClearSetting()
        {
            _settings.Clear();
        }

        virtual T* CreateSetting( KFNode& xmlnode )
        {
            auto id = xmlnode.GetUInt64( "Id" );
            auto kfsetting = _settings.Create( id );
            kfsetting->_id = id;

            return kfsetting;
        }

        // 读取配置
        virtual void ReadSetting( KFNode& xmlnode, T* kfsetting ) = 0;
    public:
        // 列表
        KFHashMap< uint64, uint64, T > _settings;
    };

    ///////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////
    template< class T >
    class KFStrConfigT : public KFConfig
    {
    public:
        // 加载配置
        void LoadConfig( const std::string& file, uint32 loadmask )
        {
            KFXml kfxml( file );
            auto config = kfxml.RootNode();
            _version = config.GetString( "version" );

            CheckClearSetting( loadmask );

            auto xmlnode = config.FindNode( "item" );
            while ( xmlnode.IsValid() )
            {
                auto kfsetting = CreateSetting( xmlnode );
                if ( kfsetting != nullptr )
                {
                    ReadSetting( xmlnode, kfsetting );
                }
                xmlnode.NextNode();
            }
        }

        // 获取配置
        const T* FindSetting( const std::string& id )
        {
            return _settings.Find( id );
        }

    protected:
        void CheckClearSetting( uint32 loadmask )
        {
            if ( KFUtility::HaveBitMask<uint32>( loadmask, KFConfigEnum::NeedClearData ) )
            {
                ClearSetting();
            }
        }

        virtual void ClearSetting()
        {
            _settings.Clear();
        }

        virtual T* CreateSetting( KFNode& xmlnode )
        {
            auto id = xmlnode.GetString( "Id" );
            auto kfsetting = _settings.Create( id );
            kfsetting->_id = id;

            return kfsetting;
        }

        // 读取配置
        virtual void ReadSetting( KFNode& xmlnode, T* kfsetting ) = 0;
    public:
        // 列表
        KFHashMap< std::string, const std::string&, T > _settings;
    };
}

#endif