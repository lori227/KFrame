#ifndef __KF_OPTION_MOUDLE_H__
#define __KF_OPTION_MOUDLE_H__

/************************************************************************
//    @Module			:    全局配置模块
//    @Author           :    __凌_痕__
//    @QQ				:    7969936
//    @Mail			    :    lori227@qq.com
//    @Date             :    2017-4-24
************************************************************************/

#include "KFrame.h"
#include "KFOptionInterface.h"
#include "KFConfig/KFConfigInterface.h"

namespace KFrame
{
    class KFOptionModule : public KFOptionInterface
    {
    public:
        KFOptionModule();
        ~KFOptionModule();

        // 初始化
        virtual void InitModule();

        // 关闭
        virtual void BeforeShut();
        ////////////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////////////

    protected:
        // 获得string配置
        virtual const std::string& GetString( const std::string& name, const std::string& key );
    };
}



#endif