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

        // 设置版本号
        void SetVersion( const std::string& file, const std::string& version )
        {
            _versions[ file ] = version;
        }

        // 获得版本
        const std::string& GetVersion()
        {
            if ( _versions.empty() )
            {
                return _invalid_string;
            }

            return _versions.begin()->second;
        }

    public:
        // 默认配置文件名
        std::string _file_name;

    protected:
        // 版本列表
        StringMap _versions;

        // 配置文件名
        std::string _setting_file_anme;
    };
    ///////////////////////////////////////////////////////////////
    template< class T >
    class KFConfigT : public KFConfig
    {
        typedef typename T::Type KeyType;
    public:
        KFConfigT()
        {
            _key_name = "Id";
            _row_name = "item";
        }

        // 加载配置
        bool LoadConfig( const std::string& filename, const std::string& filepath, uint32 loadmask )
        {
            KFXml kfxml( filepath );
            auto config = kfxml.RootNode();

            _setting_file_anme = filename;
            CheckClearSetting( loadmask );

            auto xmlnode = config.FindNode( _row_name.c_str() );
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

            return true;
        }

        // 获取配置
        const T* FindSetting( const KeyType& id ) const
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
            auto id = xmlnode.ReadT< KeyType >( _key_name.c_str() );
            auto kfsetting = _settings.Create( id );
            kfsetting->_id = id;

            return kfsetting;
        }

        // 读取配置
        virtual void ReadSetting( KFNode& xmlnode, T* kfsetting ) = 0;

    protected:
        // 配置表主键字段名
        std::string _key_name;

        // 每一行字段名
        std::string _row_name;

        // 版本号字段名
        std::string _version_name;
    public:
        // 列表
        KFHashMap< KeyType, const KeyType&, T > _settings;
    };

    ///////////////////////////////////////////////////////////////
}

#endif