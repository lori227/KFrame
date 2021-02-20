#ifndef __KF_UUIDS_H__
#define __KF_UUIDS_H__

#include "KFDefine.h"

namespace KFrame
{
    class KFUuidData;
    class KFUuidGenerate;
    class KFUuid
    {
    public:
        KFUuid();
        ~KFUuid();
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // 初始化配置
        void AddData( const std::string& name, uint64 project_time, uint32 time_bits, uint32 zone_bits, uint32 worker_bits, uint32 seq_bits );
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
        uint64 STMake( const std::string& name, uint32 zone_id, uint32 worker_id, uint64 now_time );
        uint64 MTMake( const std::string& name, uint32 zone_id, uint32 worker_id, uint64 now_time );

        // 获得zoneid
        uint32 STZoneId( const std::string& name, uint64 uuid );
        uint32 MTZoneId( const std::string& name, uint64 uuid );
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // 解析guid( time, zone, worker, seq )
        std::tuple<uint64, uint32, uint32, uint32> STParse( const std::string& name, uint64 uuid );
        std::tuple<uint64, uint32, uint32, uint32> MTParse( const std::string& name, uint64 uuid );
    protected:
        // 查找uuidsetting
        const KFUuidData* FindUuidData( const std::string& name );

        // 创建uuiddata
        KFUuidGenerate* CreateUuidGenerate( const std::string& name );

    private:
        // 多线程互斥量
        KFMutex* _kf_mutex = nullptr;

        // uuid生成列表
        std::unordered_map<std::string, KFUuidGenerate*> _uuid_data_list;

        // 配置列表
        std::unordered_map<std::string, KFUuidData*> _uuid_setting_list;
    };
}

#endif
