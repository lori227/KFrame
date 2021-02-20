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
        void AddData( const std::string& name, uint64 projecttime, uint32 timebits, uint32 zonebits, uint32 workerbits, uint32 seqbits );
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
        uint64 STMake( const std::string& name, uint32 zoneid, uint32 workerid, uint64 nowtime );
        uint64 MTMake( const std::string& name, uint32 zoneid, uint32 workerid, uint64 nowtime );

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
