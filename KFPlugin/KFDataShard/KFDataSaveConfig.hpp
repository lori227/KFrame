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
    class KFDataSaveConfig : public KFConfigT<KFDataSaveSetting>, public KFInstance<KFDataSaveConfig>
    {
    public:
        KFDataSaveConfig()
        {
            _key_name = "type";
            _file_name = "datashard";
        }

    public:
        virtual void ReadSetting( KFXmlNode& xml_node, std::shared_ptr<KFDataSaveSetting> setting )
        {
            setting->_is_open = xml_node.ReadUInt32( "open" );
            setting->_sort = xml_node.ReadUInt32( "sort" );
            setting->_save_flag = xml_node.ReadUInt32( "saveflag" );
            setting->_cache_time = xml_node.ReadUInt32( "cachetime" );
            setting->_compress_type = xml_node.ReadUInt32( "compresstype" );
            setting->_compress_level = xml_node.ReadUInt32( "compresslevel" );
        }
    };
}

#endif