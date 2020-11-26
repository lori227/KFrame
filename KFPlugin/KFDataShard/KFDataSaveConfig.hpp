#ifndef __KF_DATA_SAVE_CONFIG_H__
#define __KF_DATA_SAVE_CONFIG_H__

#include "KFConfig.h"

namespace KFrame
{
    class KFDataSaveSetting : public KFIntSetting
    {
    public:
        // 类型
        uint32 _save_flag = 0;

        // 优先级
        uint32 _sort = 0u;

        // 是否打开
        bool _is_open = false;

        // 缓存时间
        uint32 _cache_time = 2592000;

        // 压缩类型
        uint32 _compress_type = 0u;

        // 压缩等级
        uint32 _compress_level = 0u;
    };

    /////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////
    class KFDataSaveConfig : public KFConfigT<KFDataSaveSetting>, public KFInstance< KFDataSaveConfig >
    {
    public:
        KFDataSaveConfig()
        {
            _key_name = "type";
            _file_name = "datashard";
        }

    public:
        virtual void ReadSetting( KFXmlNode& xmlnode, KFDataSaveSetting* kfsetting )
        {
            kfsetting->_is_open = xmlnode.ReadUInt32( "open" );
            kfsetting->_sort = xmlnode.ReadUInt32( "sort" );
            kfsetting->_save_flag = xmlnode.ReadUInt32( "saveflag" );
            kfsetting->_cache_time = xmlnode.ReadUInt32( "cachetime" );
            kfsetting->_compress_type = xmlnode.ReadUInt32( "compresstype" );
            kfsetting->_compress_level = xmlnode.ReadUInt32( "compresslevel" );
        }
    };
}

#endif