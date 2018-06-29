#ifndef __KF_MATCH_CONFIG_H__
#define __KF_MATCH_CONFIG_H__

#include "KFrame.h"
#include "KFConfig/KFConfigInterface.h"

namespace KFrame
{
    class KFMatchSetting
    {
    public:
        KFMatchSetting()
        {
            _match_id = 1;
            _min_group_player_count = 1;
            _max_group_player_count = 3;
        }

    public:
        // 匹配模式id
        uint32 _match_id;

        // 队伍的最小人数
        uint32 _min_group_player_count;

        // 队伍的最大人数
        uint32 _max_group_player_count;
    };

    /////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////
    class KFMatchClientConfig : public KFConfig, public KFSingleton< KFMatchClientConfig >
    {
    public:
        KFMatchClientConfig();
        ~KFMatchClientConfig();

        // 加载配置
        bool LoadConfig( const char* file );

        // 查找匹配设置
        const KFMatchSetting* FindMatchSetting( uint32 matchid ) const;

    public:
        // 匹配配置列表
        KFMap< uint32, uint32, KFMatchSetting > _kf_match_setting;
    };

    //////////////////////////////////////////////////////////////////////////////////////////////////
    static auto _kf_match_config = KFMatchClientConfig::Instance();
    //////////////////////////////////////////////////////////////////////////////////////////////////
}

#endif