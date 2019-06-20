#ifndef __KF_CONFIG_H__
#define __KF_CONFIG_H__

#include "KFMap.h"
#include "KFUtility/KFXml.h"

namespace KFrame
{
    ///////////////////////////////////////////////////////////////
    template< class T >
    class KFSetting
    {
    public:
        // 类型
        typedef T Type;

        // 配置id
        T _id;
    };
    ///////////////////////////////////////////////////////////////
    class KFIntSetting : public KFSetting< uint32 >
    {
    public:
        typedef uint32 ParamType;
    };
    ///////////////////////////////////////////////////////////////
    class KFStrSetting : public KFSetting< std::string >
    {
    public:
        typedef const std::string& ParamType;
    };
    ///////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////
    class KFConfig
    {
    public:
        KFConfig( const std::string& file )
            : _file( file )
        {
        }
        virtual ~KFConfig() = default;

        // 加载配置
        virtual bool LoadConfig( const std::string& file ) = 0;

        // 加载完配置
        virtual void LoadComplete() {}
    public:
        // 配置文件
        std::string _file;
    };
    ///////////////////////////////////////////////////////////////
    template< class T >
    class KFConfigT : public KFConfig
    {
    public:
        KFConfigT( const std::string& file, bool isclear )
            : KFConfig( file ), _is_clear( isclear )
        {
        }

        // 加载配置
        bool LoadConfig( const std::string& file )
        {
            ClearSetting();

            KFXml kfxml( file );
            auto config = kfxml.RootNode();
            _version = config.GetString( "version" );

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

            return true;
        }

        // 获取配置
        const T* FindSetting( typename T::ParamType id )
        {
            return _settings.Find( id );
        }

    protected:
        // 清空配置
        virtual void ClearSetting()
        {
            if ( _is_clear )
            {
                _settings.Clear();
            }
        }

        // 创建配置
        virtual T* CreateSetting( KFNode& xmlnode ) = 0;

        // 读取配置
        virtual void ReadSetting( KFNode& xmlnode, T* kfsetting ) = 0;
    public:
        // 版本号
        std::string _version;

        // 列表
        KFHashMap< typename T::Type, typename T::ParamType, T > _settings;

    protected:
        // 是否是要清除原来的数据
        bool _is_clear = true;
    };
    ///////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////
    template< class T >
    class KFIntConfigT : public KFConfigT< T >
    {
    public:
        KFIntConfigT( const std::string& file, bool isclear )
            : KFConfigT< T >( file, isclear )
        {
        }

    protected:
        virtual T* CreateSetting( KFNode& xmlnode )
        {
            auto id = xmlnode.GetUInt32( "Id" );
            auto kfsetting = _settings.Create( id );
            kfsetting->_id = id;

            return kfsetting;
        }
    };

    ///////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////
    template< class T >
    class KFStrConfigT : public KFConfigT< T >
    {
    public:
        KFStrConfigT( const std::string& file, bool isclear )
            : KFConfigT( file, isclear )
        {
        }

    protected:
        virtual T* CreateSetting( KFNode& xmlnode )
        {
            auto id = xmlnode.GetString( "Id" );
            auto kfsetting = _settings.Create( id );
            kfsetting->_id = id;

            return kfsetting;
        }
    };
}

#endif