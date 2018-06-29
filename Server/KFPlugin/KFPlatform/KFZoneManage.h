#ifndef __KF_ZONE_MANAGE_H__
#define __KF_ZONE_MANAGE_H__

#include "KFrame.h"
#include "KFThread/KFMutex.h"
#include "KFTimer/KFTimerInterface.h"

namespace KFrame
{
    enum KFZoneStatusEnum
    {
        Shutoff = 0,
        Running = 1,
    };

    class KFAddress
    {
    public:
        KFAddress()
        {
            _appid = 0;
            _port = 0;
            _is_online = false;
            _offline_time = 0;
        }

    public:
        // appid
        uint32 _appid;

        // ip地址
        std::string _address;

        // 端口
        uint32 _port;

        // 是否在线
        bool _is_online;

        // 平台id
        uint32 _platform_id;

        // 下一次刷新时间
        uint64 _offline_time;
    };

    class KFZoneData
    {
    public:
        KFZoneData();

        void SaveToJson( KFJson& kfjson );

        // 添加ip地址
        bool AddAddress( uint32 appid, const std::string& address, uint32 port, bool isonline, uint32 platformid );

        // 删除ip地址
        void RemoveAddress( uint32 appid );

        // 选择ip地址
        KFAddress* SelectAddress();

        void SaveTo( std::map<std::string, std::string>& values, bool database );
        void CopyFrom( std::map<std::string, std::string>& values );

        // 判断在线
        void CheckOnline( KFAddress* kfaddress );
    public:
        // 服务器id
        uint32 _id;

        // 服务器类型
        uint32 _type;

        // 服务器名字
        std::string _name;

        // 服务器状态
        uint32 _status;

        // 服务器状态描述
        std::string _describe;

        // 服务器Http服务器地址
        std::string _zone_http;

        // 索引
        uint32 _address_index;

        // 服务器登录地址
        std::vector< KFAddress > _address_list;
    };

    /////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////
    class KFZoneManage : public KFSingleton< KFZoneManage >
    {
    public:
        KFZoneManage();
        ~KFZoneManage();

        void Run();
        void BeforeRun();
        void BeforeShut();

        // 添加服务器
        bool AddZoneData( KFJson& kfjson );

        // 删除大厅
        void RemoveZoneData( uint32 id );

        // 删除地址
        void RemoveZoneAddress( uint32 zoneid, uint32 appid );

        // 更新状态
        void UpdateZoneStatus( uint32 id, uint32 status, const std::string& describe );

        // 查询状态
        void QueryZoneStatus( uint32 id, uint32& status, std::string& describe );

        // 更新大厅http服务器
        void UpdateZoneHttp( uint32 zoneid, const std::string& url );

        // 更新服务器信息
        void GetZoneJson( KFJson& kfjson );

        // 随机一个服务器
        void RandZoneJson( uint32 zoneid, std::string& ip, uint32& port );

        // 获得http地址
        std::string GetZoneUrl( uint32 zoneid );

        // 查找大厅数据
        KFZoneData* FindZoneData( uint32 id );

        // 保存大厅数据
        void SaveZoneDataToDatabase( KFZoneData* zonedata );

        // 删除大厅
        void RemoveZoneDataToDatabase( uint32 zoneid );

    protected:
        // 加载大厅数据
        __KF_TIMER_FUNCTION__( OnTimerLoadZoneDataFormDatabase );

    public:
        // 服务器列表
        KFMap< uint32, uint32, KFZoneData > _kf_zone_data;

        // 线程锁
        KFMutex _kf_mutex;

        // 服务器列表版本
        std::string _zone_list_version;
    };

    ////////////////////////////////////////////////////////////////////////
    static auto _kf_zone_manage = KFZoneManage::Instance();
    ////////////////////////////////////////////////////////////////////////
}
#endif