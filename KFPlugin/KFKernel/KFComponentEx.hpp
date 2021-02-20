#ifndef __KF_COMPONENT_EX_H__
#define __KF_COMPONENT_EX_H__

#include "KFEntity.h"
#include "KFComponent.h"
#include "KFTimer/KFTimerInterface.h"

namespace KFrame
{
    typedef std::pair< std::string, std::string > DataKeyType;
    typedef std::pair< std::string, uint64 > RecordKeyType;
    ///////////////////////////////////////////////////////////////////////////////////////////////////////
    template< class T >
    class KFDataFunction
    {
    public:
        void AddFunction( KFModule* module, T& function )
        {
            auto kffunction = _functions.Create( module );
            kffunction->SetFunction( module, function );
        }

        void RemoveFunction( KFModule* module )
        {
            _functions.Remove( module );
        }

        template< class ... Arg >
        void CallFunction( Arg&& ...params )
        {
            for ( auto& iter : _functions._objects )
            {
                auto kffunction = iter.second;
                kffunction->Call( std::forward<Arg>( params )... );
            }
        }

    public:
        // 函数列表
        KFMapFunction< KFModule*, T > _functions;
    };
    ///////////////////////////////////////////////////////////////////////////////////////////////////////
    class KFComponentEx : public KFComponent
    {
    public:
        KFComponentEx() = default;
        ~KFComponentEx();
        //////////////////////////////////////////////////////////////////////////////////////////////
        void Run();
        void AfterRun();
        //////////////////////////////////////////////////////////////////////////////////////////////
        // 创建实体
        virtual KFEntity* CreateEntity( uint64 key );
        virtual KFEntity* CreateEntity( uint64 key, const KFMsg::PBObject* proto );

        // 添加实体
        virtual void AddEntity( uint64 key, KFEntity* kfentity );

        // 查找实体
        virtual KFEntity* FindEntity( uint64 key );
        virtual KFEntity* FindEntity( uint64 key, const char* function, uint32 line );

        // 删除实体
        virtual bool RemoveEntity( uint64 key );
        virtual bool RemoveEntity( KFEntity* kfentity );

        // 实体数量
        virtual uint32 GetEntityCount();

        // 遍历列表
        virtual KFEntity* FirstEntity();
        virtual KFEntity* NextEntity();

        // 获得属性列表
        virtual StringVector& GetDataList( const std::string& dataname );

        // 获得类配置
        virtual const KFClassSetting* FindClassSetting();
        virtual const KFClassSetting* FindClassSetting( const std::string& dataname );

        // 获得属性配置
        virtual const KFDataSetting* FindDataSetting();
        virtual const KFDataSetting* FindDataSetting( const std::string& dataname );
        virtual const KFDataSetting* FindDataSetting( const std::string& parentname, const std::string& dataname );
        //////////////////////////////////////////////////////////////////////////////////////
        virtual void SetMaxValue( const std::string& dataname, uint32 value );
        virtual void SetMaxValue( const std::string& parentname, const std::string& dataname, uint32 value );
        //////////////////////////////////////////////////////////////////////////////////////

        // 启动保存定时器
        void StartSaveEntityTimer( KFEntity* kfentity, KFData* kfdata );

        // 停止保存定时器
        void DeleteSaveEntity( KFEntity* kfentity );
        //////////////////////////////////////////////////////////////////////////////////////
        // 属性更新回调函数
        void UpdateDataCallBack( KFEntity* kfentity, KFData* kfdata, const std::string& oldvalue, const std::string& newvalue, bool callback );
        void UpdateDataCallBack( KFEntity* kfentity, uint64 key, KFData* kfdata, uint64 index, uint32 operate, uint64 value, uint64 oldvalue, uint64 newvalue, bool callback );

        // 属性添加回调函数
        void AddDataCallBack( KFEntity* kfentity, KFData* kfparent, uint64 key, KFData* kfdata, bool callback );

        // 属性删除回调函数
        void RemoveDataCallBack( KFEntity* kfentity, KFData* kfparent, uint64 key, KFData* kfdata, bool callback );

        // 添加更新的对象
        void AddSyncEntity( KFEntity* entity );

        ///////////////////////////////////////////////////////////////////////////////////////////////
        ///////////////////////////////////////////////////////////////////////////////////////////////
        void CallLogElementFunction( KFEntity* kfentity, const KFElementResult* kfresult );
        bool CallElementResultFunction( KFEntity* kfentity, const KFElementResult* kfresult );
        ///////////////////////////////////////////////////////////////////////////////////////////////
        ///////////////////////////////////////////////////////////////////////////////////////////////
        virtual void BindCheckAddElementFunction( const std::string& dataname, KFCheckAddElementFunction& function );
        virtual void UnRegisterCheckAddElementFunction( const std::string& dataname );

        virtual void BindAddElementFunction( const std::string& dataname, KFAddElementFunction& function );
        virtual void UnRegisterAddElementFunction( const std::string& dataname );

        virtual void BindCheckRemoveElementFunction( const std::string& dataname, KFCheckRemoveElementFunction& function );
        virtual void UnRegisterCheckRemoveElementFunction( const std::string& dataname );

        virtual void BindRemoveElementFunction( const std::string& dataname, KFRemoveElementFunction& function );
        virtual void UnRegisterRemoveElementFunction( const std::string& dataname );
        ///////////////////////////////////////////////////////////////////////////////////////////////
        virtual void BindAddDataLogic( KFModule* module, const std::string& dataname, KFAddDataFunction& function );
        virtual void UnBindAddDataLogic( KFModule* module, const std::string& dataname );

        virtual void BindAddDataModule( KFModule* module, KFAddDataFunction& function );
        virtual void UnBindAddDataModule( KFModule* module );
        virtual void BindAddDataFunction( KFModule* module, const std::string& dataname, uint64 key, KFAddDataFunction& function );
        virtual void UnBindAddDataFunction( KFModule* module, const std::string& dataname, uint64 key );

        virtual void BindRemoveDataLogic( KFModule* module, const std::string& dataname, KFRemoveDataFunction& function );
        virtual void UnBindRemoveDataLogic( KFModule* module, const std::string& dataname );

        virtual void BindRemoveDataModule( KFModule* module, KFRemoveDataFunction& function );
        virtual void UnBindRemoveDataModule( KFModule* module );
        virtual void BindRemoveDataFunction( KFModule* module, const std::string& dataname, uint64 key, KFRemoveDataFunction& function );
        virtual void UnBindRemoveDataFunction( KFModule* module, const std::string& dataname, uint64 key );

        virtual void BindUpdateDataModule( KFModule* module, KFUpdateDataFunction& function );
        virtual void UnBindUpdateDataModule( KFModule* module );
        virtual void BindUpdateDataFunction( KFModule* module, const std::string& parentname, const std::string& dataname, KFUpdateDataFunction& function );
        virtual void UnBindUpdateDataFunction( KFModule* module, const std::string& parentname, const std::string& dataname );

        virtual void BindUpdateStringModule( KFModule* module, KFUpdateStringFunction& function );
        virtual void UnBindUpdateStringModule( KFModule* module );
        virtual void BindUpdateStringFunction( KFModule* module, const std::string& parentname, const std::string& dataname, KFUpdateStringFunction& function );
        virtual void UnBindUpdateStringFunction( KFModule* module, const std::string& parentname, const std::string& dataname );
        /////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual void BindEntityInitializeFunction( KFModule* module, KFEntityFunction& function );
        virtual void UnRegisterEntityInitializeFunction();

        virtual void BindEntityUninitializeFunction( KFModule* module, KFEntityFunction& function );
        virtual void UnRegisterEntityUninitializeFunction();

        virtual void BindEntityRunFunction( KFModule* module, KFEntityFunction& function );
        virtual void UnRegisterEntityRunFunction();

        virtual void BindEntityAfterRunFunction( KFModule* module, KFEntityFunction& function );
        virtual void UnRegisterEntityAfterRunFunction();

        virtual void BindEntitySaveFunction( KFModule* module, KFSaveEntityFunction& function );
        virtual void UnRegisterEntitySaveFunction();

        virtual void BindEntityDeleteFunction( KFModule* module, KFEntityFunction& function );
        virtual void UnRegisterEntityDeleteFunction();
        /////////////////////////////////////////////////////////////////////////////////////////////////////
        /////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual void BindSyncUpdateFunction( KFModule* module, KFSyncFunction& function );
        virtual void UnRegisterSyncUpdateFunction();

        virtual void BindSyncAddFunction( KFModule* module, KFSyncFunction& function );
        virtual void UnRegisterSyncAddFunction();

        virtual void BindSyncRemoveFunction( KFModule* module, KFSyncFunction& function );
        virtual void UnRegisterSyncRemoveFunction();
        /////////////////////////////////////////////////////////////////////////////////////////////////////
        /////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual void BindShowElementFunction( KFModule* module, KFShowElementFunction& function );
        virtual void UnRegisterShowElementFunction();
        /////////////////////////////////////////////////////////////////////////////////////////////////////
        /////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual void BindGetConfigValueFunction( KFModule* module, const std::string& name, KFGetConfigValueFunction& function );
        virtual void UnBindGetConfigValueFunction( const std::string& name );
        /////////////////////////////////////////////////////////////////////////////////////////////////////
        /////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual void BindLogElementFunction( KFModule* module, const std::string& name, KFLogElementFunction& function );
        virtual void UnBindLogElementFunction( const std::string& name );

        virtual void BindElementResultFunction( KFModule* module, const std::string& name, KFElementResultFunction& function );
        virtual void UnBindElementResultFunction( const std::string& name );
    protected:
        // 保存数据到数据库
        __KF_TIMER_FUNCTION__( OnTimerSaveEntity );

        // 初始化
        void InitEntity( KFEntity* kfentity );

        // 卸载
        void UnInitEntity( KFEntity* kfentity );

        // 保存
        void SaveEntity( KFEntity* kfentity, uint32 flag, const char* function, uint32 line );

        // 删除
        void DeleteEntity( KFEntity* kfentity );

        // 格式化数据Key
        const DataKeyType& FormatDataKey( KFData* kfdata );
        const DataKeyType& FormatDataKey( const std::string& parentname, const std::string& dataname );
    public:
        // 对象实例列表
        KFHashMap< uint64, KFEntity > _entitys;
        /////////////////////////////////////////////////////////////////////////////////////////////
        // 判断是否能添加
        KFMapFunction< std::string, KFCheckAddElementFunction > _check_add_element_function;

        // 添加元数据函数
        KFMapFunction< std::string, KFAddElementFunction > _add_element_function;

        // 判断元数据满足
        KFMapFunction< std::string, KFCheckRemoveElementFunction > _check_remove_element_function;

        // 删除元数据函数
        KFMapFunction< std::string, KFRemoveElementFunction > _remove_element_function;
        /////////////////////////////////////////////////////////////////////////////////////////////
        // 属性更新回调列表
        KFMapFunction< KFModule*, KFAddDataFunction > _add_data_module;
        KFMapFunction< KFModule*, KFRemoveDataFunction > _remove_data_module;
        KFMapFunction< KFModule*, KFUpdateDataFunction > _update_data_module;
        KFMapFunction< KFModule*, KFUpdateStringFunction > _update_string_module;

        // 添加属性逻辑
        KFMap< std::string, KFDataFunction< KFAddDataFunction > > _add_logic_function;

        // 添加属性回调
        KFMap< RecordKeyType, KFDataFunction< KFAddDataFunction > > _add_data_function;

        // 删除属性逻辑
        KFMap< std::string, KFDataFunction< KFRemoveDataFunction > > _remove_logic_function;

        // 删除数据的回调函数
        KFMap< RecordKeyType, KFDataFunction< KFRemoveDataFunction > > _remove_data_function;

        // 更新数据的回调函数
        KFMap< DataKeyType, KFDataFunction< KFUpdateDataFunction > > _update_data_function;

        // 更新数据的回调函数
        KFMap< DataKeyType, KFDataFunction< KFUpdateStringFunction > > _update_string_function;
        /////////////////////////////////////////////////////////////////////////////////////////////
        KFFunction< KFEntityFunction > _entity_initialize_function;
        KFFunction< KFEntityFunction > _entity_uninitialize_function;
        KFFunction< KFEntityFunction > _entity_run_function;
        KFFunction< KFEntityFunction > _entity_after_run_function;
        KFFunction< KFEntityFunction > _entity_delete_function;
        KFFunction< KFSaveEntityFunction > _entity_save_function;
        /////////////////////////////////////////////////////////////////////////////////////////////
        /////////////////////////////////////////////////////////////////////////////////////////////
        // 对象同步更新函数
        KFFunction< KFSyncFunction > _entity_sync_update_function;

        // 对象同步添加函数
        KFFunction< KFSyncFunction > _entity_sync_add_function;

        // 对象同步删除函数
        KFFunction< KFSyncFunction > _entity_sync_remove_function;

        // 显示奖励函数
        KFFunction< KFShowElementFunction > _show_element_function;
        /////////////////////////////////////////////////////////////////////////////////////////////
        // 获得属性逻辑
        KFMapFunction< std::string, KFGetConfigValueFunction > _get_config_value_function;

        // 日志函数
        KFMapFunction< std::string, KFLogElementFunction > _log_element_function;

        // 操作结果
        KFMapFunction< std::string, KFElementResultFunction > _element_result_function;
    protected:
        // 需要发送消息的对象
        std::set< KFEntity* > _sync_entitys;
    public:
        // 组件名称
        std::string _component_name;

        // 属性配置
        const KFDataSetting* _data_setting = nullptr;
    };
}

#endif