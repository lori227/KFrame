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
        virtual bool LoadConfig( const std::string& filename, const std::string& filepath, uint32 loadmask )
        {
            return false;
        }

        // 加载完配置
        virtual void LoadComplete() {}

        // 所有配置加载完
        virtual void LoadAllComplete() {}

        // 获得版本
        const std::string& GetVersion()
        {
            if ( _versions.empty() )
            {
                return _invalid_string;
            }

            return _versions.begin()->second;
        }

        // 判断版本
        bool CheckVersion( const std::string& file, const std::string& version )
        {
            if ( version.empty() )
            {
                return false;
            }

            auto iter = _versions.find( file );
            if ( iter == _versions.end() )
            {
                return false;
            }

            return iter->second == version;
        }

    public:
        // 默认配置文件名
        std::string _file_name;

    protected:
        // 配置文件名
        std::string _setting_file_anme;

        // 版本列表
        std::unordered_map< std::string, std::string > _versions;

        // 更新版本
        void UpdateVersion( const std::string& file, const std::string& version )
        {
            _versions[ file ] = version;
        }
    };
    ///////////////////////////////////////////////////////////////
    template< class T >
    class KFConfigT : public KFConfig
    {
        typedef typename T::Type KeyType;
    public:
        // 加载配置
        bool LoadConfig( const std::string& filename, const std::string& filepath, uint32 loadmask )
        {
            KFXml kfxml( filepath );
            auto config = kfxml.RootNode();
            auto version = config.GetString( "version" );

            _setting_file_anme = filename;
            CheckClearSetting( loadmask );

            auto xmlnode = config.FindNode( "item" );
            while ( xmlnode.IsValid() )
            {
                auto kfsetting = CreateSetting( xmlnode );
                if ( kfsetting != nullptr )
                {
                    kfsetting->_file_name = _setting_file_anme;
                    ReadSetting( xmlnode, kfsetting );
                }
                xmlnode.NextNode();
            }

            UpdateVersion( filename, version );
            return true;
        }

        // 获取配置
        const T* FindSetting( const KeyType& id )
        {
            return _settings.Find( id );
        }

    protected:
        void CheckClearSetting( uint32 loadmask )
        {
            if ( !KFUtility::HaveBitMask<uint32>( loadmask, KFConfigEnum::NeedClearData ) )
            {
                return;
            }

            if ( KFUtility::HaveBitMask<uint32>( loadmask, KFConfigEnum::ClearFileData ) )
            {
                ClearFileSetting();
            }
            else
            {
                ClearSetting();
                _versions.clear();
            }
        }

        virtual void ClearSetting()
        {
            _settings.Clear();
        }

        virtual void ClearFileSetting()
        {
            std::list< KeyType > removes;
            for ( auto& iter : _settings._objects )
            {
                auto kfsetting = iter.second;
                if ( kfsetting->_file_name == _setting_file_anme )
                {
                    removes.push_back( kfsetting->_id );
                }
            }

            for ( auto id : removes )
            {
                _settings.Remove( id );
            }
        }

        virtual T* CreateSetting( KFNode& xmlnode )
        {
            auto id = xmlnode.ReadT< KeyType >( "Id" );
            auto kfsetting = _settings.Create( id );
            kfsetting->_id = id;

            return kfsetting;
        }

        // 读取配置
        virtual void ReadSetting( KFNode& xmlnode, T* kfsetting ) = 0;
    public:
        // 列表
        KFHashMap< KeyType, const KeyType&, T > _settings;
    };

    ///////////////////////////////////////////////////////////////
}

#endif