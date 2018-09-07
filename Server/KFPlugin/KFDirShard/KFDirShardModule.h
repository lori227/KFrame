#ifndef __KF_DIR_SHARD_MODULE_H__
#define __KF_DIR_SHARD_MODULE_H__

/************************************************************************
//    @Module			:    目录服务逻辑
//    @Author           :    __凌_痕__
//    @QQ				:    7969936
//    @Mail			    :    lori227@qq.com
//    @Date             :    2017-11-30
************************************************************************/

#include "KFrame.h"
#include "KFDirShardInterface.h"
#include "KFRedis/KFRedisInterface.h"
#include "KFConfig/KFConfigInterface.h"
#include "KFMessage/KFMessageInterface.h"
#include "KFTimer/KFTimerInterface.h"
#include "KFHttpServer/KFHttpServerInterface.h"

namespace KFrame
{
    class KFDirData
    {
    public:
        KFDirData()
        {
            _zone_id = _invalid_int;
            _zone_channel = _invalid_int;
            _zone_port = _invalid_int;
            _app_id = _invalid_int;
            _online_count = std::numeric_limits<uint32>::max();
        }

    public:
        // 小区id
        uint32 _zone_id;

        // 小区类型
        uint32 _zone_channel;

        // 小区名字
        std::string _zone_name;

        // 服务器id
        uint32 _app_id;

        // 小区ip
        std::string _zone_ip;

        // 小区端口
        uint32 _zone_port;

        // 小区在线人数
        uint32 _online_count;
    };

    class KFDirShardModule : public KFDirShardInterface
    {
    public:
        KFDirShardModule() = default;
        ~KFDirShardModule() = default;

        virtual void InitModule();

        // 初始化
        virtual void BeforeRun();

        // 关闭
        virtual void BeforeShut ();
        ////////////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////////////

    protected:
        // 更新在线数量
        __KF_MESSAGE_FUNCTION__( HandleUpdateOnlineToDirReq );

        // 删除在线数量
        __KF_MESSAGE_FUNCTION__( HandleRemoveOnlineToDirReq );

    protected:
        // 请求http列表
        __KF_HTTP_FUNCTION__( HandleQueryDirList );

    protected:
        // 注册url地址
        __KF_TIMER_FUNCTION__( OnTimerRegisterDirUrl );

    protected:
        uint32 BalanceAllocZoneId();
    private:
        // 小区列表
        KFMap< uint32, uint32, KFDirData > _kf_dir_list;

        // 服务器列表类型
        uint32 _dir_list_type;
    };
}

#endif