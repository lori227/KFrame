#ifndef __KF_PLAYER_CONFIG_H__
#define __KF_PLAYER_CONFIG_H__

#include "KFCore/KFElement.h"
#include "KFZConfig/KFConfig.h"

namespace KFrame
{
    class KFPlayerSetting : public KFIntSetting
    {
    public:
        ~KFPlayerSetting()
        {
            for ( auto kflements : _new_player_elements )
            {
                __KF_DELETE__( KFElements, kflements );
            }
            _new_player_elements.clear();
        }
    public:
        std::vector< KFElements* > _new_player_elements;
    };
    ////////////////////////////////////////////////////////////////////////////////////
    class KFPlayerConfig : public KFIntConfigT< KFIntSetting >, public KFInstance< KFPlayerConfig >
    {
    public:
        KFPlayerConfig()
        {
            _file_name = "player";
        }

        ~KFPlayerConfig()
        {
            ClearElements();
        }

    protected:
        // 清空数据
        void ClearElements();

        // 清空配置
        virtual void ClearSetting();

        // 创建配置
        KFIntSetting* CreateSetting( KFNode& xmlnode );

        // 读取配置
        virtual void ReadSetting( KFNode& xmlnode, KFIntSetting* kfsetting );

    public:
        // 新角色添加的数据
        std::vector< KFElements* > _new_player_elements;
    };
}

#endif