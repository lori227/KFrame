#ifndef __KF_CONFIG_MODULE_H__
#define __KF_CONFIG_MODULE_H__


/************************************************************************
//    @Module			:    配置管理模块
//    @Author           :    __凌_痕__
//    @QQ				:    7969936
//    @Mail			    :    lori227@qq.com
//    @Date             :    2017-3-19
************************************************************************/

#include "KFrame.h"
#include "KFConfigInterface.h"

namespace KFrame
{
    class KFConfigData
    {
    public:
        KFConfigData()
        {
            _config = nullptr;
            _can_reload = false;
        }

    public:
        KFConfig* _config;
        std::string _file;
        bool _can_reload;
    };
    ////////////////////////////////////////////////////////////////
    class KFConfigModule : public KFConfigInterface
    {
    public:
        // 加载一个配置
        virtual bool AddConfig( KFConfig* config, const std::string& file, bool canreload );

        // 删除配置
        virtual void RemoveConfig( KFConfig* config );

        // 重新加载配置
        virtual void ReloadConfig( const std::string& file );
        ////////////////////////////////////////////////////////////////////////////////
    protected:

        // 加载配置
        void LoadConfig( KFConfig* config, const std::string& file );

    private:
        // 配置列表
        KFHashMap< uint64, uint64, KFConfigData > _kf_config_data;
    };
}



#endif