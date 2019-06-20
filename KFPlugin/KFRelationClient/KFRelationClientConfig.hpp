#ifndef __KF_RELATION_CLIENT_CONFIG_H__
#define __KF_RELATION_CLIENT_CONFIG_H__

#include "KFConfig/KFConfigInterface.h"

namespace KFrame
{
    class KFRelationSetting : public KFStrSetting
    {
    public:
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
    class KFRelationClientConfig : public KFStrConfigT< KFRelationSetting >, public KFSingleton< KFRelationClientConfig >
    {
    public:
        KFRelationClientConfig( const std::string& file, bool isclear )
            : KFStrConfigT< KFRelationSetting >( file, isclear )
        {
        }

        // 查找关系配置
        const KFRelationSetting* FindRelationSettingByInviteName( const std::string& name );

    protected:
        // 读取配置
        void ReadSetting( KFNode& xmlnode, KFRelationSetting* kfsetting );
    };

    //////////////////////////////////////////////////////////////////////////////////////////////////
    static auto _kf_relation_client_config = KFRelationClientConfig::Instance( "relation.xml", true );
    //////////////////////////////////////////////////////////////////////////////////////////////////
}

#endif