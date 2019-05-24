#ifndef __KF_RELATION_CLIENT_CONFIG_H__
#define __KF_RELATION_CLIENT_CONFIG_H__

#include "KFConfig/KFConfigInterface.h"

namespace KFrame
{
    class KFRelationSetting
    {
    public:
        // 关系属性名
        std::string _data_name;

        // 最大数量
        uint32 _max_count = 0u;

        // 是否需要更新
        bool _need_update = false;

        // 是否是双向的
        bool _both_way = false;

        // 是否上线加载
        bool _online_load = false;

        // 邀请列表名字
        std::string _invite_data_name;

        // 邀请列表数量
        uint32 _invite_data_count = 0u;

        // 拒绝申请名字
        std::string _refuse_name;
    };

    /////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////
    class KFRelationClientConfig : public KFConfig, public KFSingleton< KFRelationClientConfig >
    {
    public:
        KFRelationClientConfig()
        {
            _file = "relation.config";
        }

        // 加载配置
        bool LoadConfig( const std::string& file );

        // 查找配置选项
        const KFRelationSetting* FindRelationSetting( const std::string& name );
        const KFRelationSetting* FindRelationSettingByInviteName( const std::string& name );

    public:
        // 关系配置列表
        KFMap< std::string, const std::string&, KFRelationSetting > _relation_list;
    };

    //////////////////////////////////////////////////////////////////////////////////////////////////
    static auto _kf_relation_client_config = KFRelationClientConfig::Instance();
    //////////////////////////////////////////////////////////////////////////////////////////////////
}

#endif