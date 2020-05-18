#ifndef __KF_ZONE_CONFIG_H__
#define __KF_ZONE_CONFIG_H__

#include "KFZConfig/KFConfig.h"

namespace KFrame
{
    // 分区设置
    class KFZoneSetting : public KFIntSetting
    {
    public:
        // 游戏服名字
        std::string _name;

        // 登录指定id
        uint32 _login_id = 0u;

        // 数据指定id
        uint32 _data_id = 0u;

        // 小区标识
        std::string _flag;

        // 是否推荐
        uint32 _recommend = 0u;
    };

    /////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////
    class KFZoneConfig : public KFConfig, public KFInstance< KFZoneConfig >
    {
    public:
        KFZoneConfig()
        {
            _file_name = "zone";
        }

        // 加载配置
        bool LoadConfig( const std::string& filename, const std::string& filepath, uint32 loadmask );

        // 查找小区信息
        const KFZoneSetting* ZoneSetting();
        const KFZoneSetting* FindSetting( uint32 zoneid );

        // 是否开启推荐
        bool IsOpenRecommend() const
        {
            return _is_open_recommend;
        }

    protected:
        // 设置小区信息
        void SetZoneData( KFZoneSetting* kfsetting, uint32 zoneid );

    private:
        // 本分区属性
        KFZoneSetting _zone;

        // 小区模板
        KFZoneSetting _zone_template;

        // 小区列表
        KFHashMap< uint32, uint32, KFZoneSetting > _zone_list;

        // 推荐的小区
        bool _is_open_recommend = false;
    };
}

#endif