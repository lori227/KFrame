#ifndef __KF_KERNEL_MODULE_H__
#define __KF_KERNEL_MODULE_H__

/************************************************************************
//    @Module			:    游戏核心数据
//    @Author           :    __凌_痕__
//    @QQ				:    7969936
//    @Mail			    :    lori227@qq.com
//    @Date             :    2017-1-12
************************************************************************/

#include "KFrame.h"
#include "KFKernelInterface.h"
#include "KFComponentEx.h"
#include "KFTimer/KFTimerInterface.h"
#include "KFConfig/KFConfigInterface.h"
#include "KFOption/KFOptionInterface.h"

namespace KFrame
{
    class KFKernelModule : public KFKernelInterface
    {
    public:
        KFKernelModule() = default;
        ~KFKernelModule() = default;

        virtual void InitModule();
        virtual void AfterLoad();
        virtual void Run();
        virtual void AfterRun();
        virtual void ShutDown();
        ////////////////////////////////////////////////////////////////////////////////////////////

        // 添加配置读取
        virtual const KFClassSetting* FindClassSetting( const std::string& dataname );

        /////////////////////////////////////////////////////////////////////////////////////////////
        /////////////////////////////////////////////////////////////////////////////////////////////
        // 查找组件
        virtual KFComponent* FindComponent( const std::string& dataname );

        // 查找实体
        virtual KFEntity* FindEntity( const std::string& dataname, uint64 key, const char* function, uint32 line );
        /////////////////////////////////////////////////////////////////////////////////////////////
        // 创建数据
        virtual KFData* CreateObject( const std::string& dataname );
        virtual KFData* CreateObject( const KFDataSetting* datasetting );
        virtual KFData* CreateObject( const std::string& classname, const std::string& dataname );

        virtual KFData* CreateObject( const KFDataSetting* datasetting, const KFMsg::PBObject* proto );

        // 释放数据
        virtual void ReleaseObject( KFData* kfdata );
        /////////////////////////////////////////////////////////////////////////////////////////////

        // proto 序列化
        virtual bool ParseFromProto( KFData* kfdata, const KFMsg::PBObject* proto );
        virtual bool ParseFromProto( KFData* kfdata, const std::string& proto );

        virtual void SerializeToClient( KFData* kfdata, KFMsg::PBObject* proto );
        virtual void SerializeToData( KFData* kfdata, KFMsg::PBObject* proto );
        virtual void SerializeToView( KFData* kfdata, KFMsg::PBObject* proto );
        virtual void SerializeToOnline( KFData* kfdata, KFMsg::PBObject* proto );

        // 获得数值
        virtual uint32 CalcAgentValue( KFAgentValue* kfvalue, float multiple );

        // 是否可以倍率
        bool IsMultipleData( const std::string& dataname );

    public:
        // copy from proto
        void CopyFromInt32( KFData* kfdata, const KFMsg::PBInt32* proto );
        void CopyFromUInt32( KFData* kfdata, const KFMsg::PBUInt32* proto );
        void CopyFromInt64( KFData* kfdata, const KFMsg::PBInt64* proto );
        void CopyFromUInt64( KFData* kfdata, const KFMsg::PBUInt64* proto );
        void CopyFromDouble( KFData* kfdata, const KFMsg::PBDouble* proto );
        void CopyFromString( KFData* kfdata, const KFMsg::PBString* proto );
        void CopyFromVector3D( KFData* kfdata, const KFMsg::PBVector3D* proto );
        void CopyFromArray( KFData* kfdata, const KFMsg::PBArray* proto );
        void CopyFromObject( KFData* kfdata, const KFMsg::PBObject* proto );
        void CopyFromRecord( KFData* kfdata, const KFMsg::PBRecord* proto );

        // save to proto
        void SaveToInt32( KFData* kfdata, KFMsg::PBInt32* proto );
        void SaveToUInt32( KFData* kfdata, KFMsg::PBUInt32* proto );
        void SaveToInt64( KFData* kfdata, KFMsg::PBInt64* proto );
        void SaveToUInt64( KFData* kfdata, KFMsg::PBUInt64* proto );
        void SaveToDouble( KFData* kfdata, KFMsg::PBDouble* proto );
        void SaveToString( KFData* kfdata, KFMsg::PBString* proto );
        void SaveToVector3D( KFData* kfdata, KFMsg::PBVector3D* proto );
        void SaveToArray( KFData* kfdata, KFMsg::PBArray* proto );
        void SaveToObject( KFData* kfdata, KFMsg::PBObject* proto, uint32 datamask );
        void SaveToRecord( KFData* kfdata, KFMsg::PBRecord* proto, uint32 datamask );

    private:
        KFMap< std::string, const std::string, KFComponentEx > _kf_component;

        // 允许有倍数的属性
        std::set< std::string > _multiple_data;
    };
}

#endif