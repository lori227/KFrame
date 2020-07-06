#ifndef __KF_DIR_SHARD_MODULE_H__
#define __KF_DIR_SHARD_MODULE_H__

/************************************************************************
//    @Module			:    目录模块
//    @Author           :    __凌_痕__
//    @QQ				:    7969936
//    @Mail			    :    lori227@qq.com
//    @Date             :    2017-8-23
************************************************************************/

#include "KFDirShardInterface.h"
#include "KFHttpServer/KFHttpServerInterface.h"
#include "KFDirAttribute/KFDirAttributeInterface.h"

namespace KFrame
{
    class KFDirShardModule : public KFDirShardInterface
    {
    public:
        KFDirShardModule() = default;
        ~KFDirShardModule() = default;

        // 初始化
        virtual void BeforeRun();

        // 关闭
        virtual void BeforeShut();
        ////////////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////////////
    protected:
        // 小区信息注册
        __KF_HTTP_FUNCTION__( HandleZoneRegister );

        // 小区信息更新
        __KF_HTTP_FUNCTION__( HandleZoneUpdate );

        // 更新小区状态
        __KF_HTTP_FUNCTION__( HandleZoneStatus );

        // 查询小区列表
        __KF_HTTP_FUNCTION__( HandleQueryZoneList );

        // 查询小区ip
        __KF_HTTP_FUNCTION__( HandleQueryZoneIp );

        // 小区人数更新
        __KF_HTTP_FUNCTION__( HandleZoneBalance );

        // 推荐小区
        __KF_HTTP_FUNCTION__( HandleZoneRecommend );

        // world的http地址
        __KF_HTTP_FUNCTION__( HandleWorldRegister );

        // 更新masterip
        __KF_HTTP_FUNCTION__( HandleUpdateMasterIp );

        // 查询masterip
        __KF_HTTP_FUNCTION__( HandleQueryMasterIp );

        // 查询masterlist
        __KF_HTTP_FUNCTION__( HandleQueryMasterList );
    };
}



#endif