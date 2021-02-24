#ifndef __KF_COMPONENT_EX_H__
#define __KF_COMPONENT_EX_H__

#include "KFEntity.h"
#include "KFComponent.h"
#include "KFDataFunction.hpp"
#include "KFTimer/KFTimerInterface.h"

namespace KFrame
{
    class KFComponentEx : public KFComponent, public std::enable_shared_from_this<KFComponentEx>
    {
    public:
        KFComponentEx() = default;
        ~KFComponentEx();
        //////////////////////////////////////////////////////////////////////////////////////////////
        void Run();
        void AfterRun();
        //////////////////////////////////////////////////////////////////////////////////////////////
        // 创建实体
        virtual EntityPtr CreateEntity( uint64 key );
        virtual EntityPtr CreateEntity( uint64 key, const KFMsg::PBObject* proto );

        // 添加实体
        virtual void AddEntity( uint64 key, EntityPtr entity );

        // 查找实体
        virtual EntityPtr FindEntity( uint64 key );
        virtual EntityPtr FindEntity( uint64 key, const char* function, uint32 line );

        // 删除实体
        virtual bool RemoveEntity( uint64 key );
        virtual bool RemoveEntity( EntityPtr entity );

        // 实体数量
        virtual uint32 GetEntityCount();

        // 遍历列表
        virtual EntityPtr FirstEntity();
        virtual EntityPtr NextEntity();
        //////////////////////////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////////////////////////
        // 获得属性列表
        virtual StringVector& GetDataList( const std::string& data_name );

        // 获得属性配置
        virtual std::shared_ptr<const KFDataSetting> FindDataSetting();
        virtual std::shared_ptr<const KFDataSetting> FindDataSetting( const std::string& data_name );
        virtual std::shared_ptr<const KFDataSetting> FindDataSetting( const std::string& parent_name, const std::string& data_name );
        //////////////////////////////////////////////////////////////////////////////////////
        // 启动保存定时器
        void StartSaveEntityTimer( EntityPtr entity, DataPtr data );

        // 停止保存定时器
        void DeleteSaveEntity( EntityPtr entity );
        //////////////////////////////////////////////////////////////////////////////////////
        // 属性更新回调函数
        void UpdateDataCallBack( EntityPtr entity, DataPtr data, const std::string& old_value, const std::string& new_value, bool callback );
        void UpdateDataCallBack( EntityPtr entity, uint64 key, DataPtr data, uint64 index, uint32 operate, uint64 value, uint64 old_value, uint64 new_value, bool callback );

        // 属性添加回调函数
        void AddDataCallBack( EntityPtr entity, DataPtr parent_data, uint64 key, DataPtr data, bool callback );

        // 属性删除回调函数
        void RemoveDataCallBack( EntityPtr entity, DataPtr parent_data, uint64 key, DataPtr data, bool callback );

        // 添加更新的对象
        void AddSyncEntity( EntityPtr entity );

        ///////////////////////////////////////////////////////////////////////////////////////////////
        ///////////////////////////////////////////////////////////////////////////////////////////////
        void CallLogElementFunction( EntityPtr entity, const KFElementResult* result );
        bool CallElementResultFunction( EntityPtr entity, const KFElementResult* result );
        ///////////////////////////////////////////////////////////////////////////////////////////////
        ///////////////////////////////////////////////////////////////////////////////////////////////
        virtual void BindCheckAddElementFunction( KFModule* module, const std::string& data_name, KFCheckAddElementFunction& function );
        virtual void UnRegisterCheckAddElementFunction( const std::string& data_name );

        virtual void BindAddElementFunction( KFModule* module, const std::string& data_name, KFAddElementFunction& function );
        virtual void UnRegisterAddElementFunction( const std::string& data_name );

        virtual void BindCheckRemoveElementFunction( KFModule* module, const std::string& data_name, KFCheckRemoveElementFunction& function );
        virtual void UnRegisterCheckRemoveElementFunction( const std::string& data_name );

        virtual void BindRemoveElementFunction( KFModule* module, const std::string& data_name, KFRemoveElementFunction& function );
        virtual void UnRegisterRemoveElementFunction( const std::string& data_name );
        ///////////////////////////////////////////////////////////////////////////////////////////////
        virtual void BindAddDataLogic( KFModule* module, const std::string& data_name, KFAddDataFunction& function );
        virtual void UnBindAddDataLogic( KFModule* module, const std::string& data_name );

        virtual void BindAddDataModule( KFModule* module, KFAddDataFunction& function );
        virtual void UnBindAddDataModule( KFModule* module );
        virtual void BindAddDataFunction( KFModule* module, const std::string& data_name, uint64 key, KFAddDataFunction& function );
        virtual void UnBindAddDataFunction( KFModule* module, const std::string& data_name, uint64 key );

        virtual void BindRemoveDataLogic( KFModule* module, const std::string& data_name, KFRemoveDataFunction& function );
        virtual void UnBindRemoveDataLogic( KFModule* module, const std::string& data_name );

        virtual void BindRemoveDataModule( KFModule* module, KFRemoveDataFunction& function );
        virtual void UnBindRemoveDataModule( KFModule* module );
        virtual void BindRemoveDataFunction( KFModule* module, const std::string& data_name, uint64 key, KFRemoveDataFunction& function );
        virtual void UnBindRemoveDataFunction( KFModule* module, const std::string& data_name, uint64 key );

        virtual void BindUpdateDataModule( KFModule* module, KFUpdateDataFunction& function );
        virtual void UnBindUpdateDataModule( KFModule* module );
        virtual void BindUpdateDataFunction( KFModule* module, const std::string& parent_name, const std::string& data_name, KFUpdateDataFunction& function );
        virtual void UnBindUpdateDataFunction( KFModule* module, const std::string& parent_name, const std::string& data_name );

        virtual void BindUpdateStringModule( KFModule* module, KFUpdateStringFunction& function );
        virtual void UnBindUpdateStringModule( KFModule* module );
        virtual void BindUpdateStringFunction( KFModule* module, const std::string& parent_name, const std::string& data_name, KFUpdateStringFunction& function );
        virtual void UnBindUpdateStringFunction( KFModule* module, const std::string& parent_name, const std::string& data_name );
        /////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual void BindEntityInitializeFunction( KFModule* module, KFEntityFunction& function );
        virtual void UnRegisterEntityInitializeFunction();

        virtual void BindEntityRemoveFunction( KFModule* module, KFEntityFunction& function );
        virtual void UnRegisterEntityRemoveFunction();

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
        void OnInitEntity( EntityPtr entity );

        // 卸载
        void OnRemoveEntity( EntityPtr entity );

        // 保存
        void SaveEntity( EntityPtr entity, uint32 flag, const char* function, uint32 line );

        // 删除
        void DeleteEntity( EntityPtr entity );

        // 格式化数据Key
        const DataKeyType& FormatDataKey( DataPtr data );
        const DataKeyType& FormatDataKey( const std::string& parent_name, const std::string& data_name );
    public:
        /////////////////////////////////////////////////////////////////////////////////////////////
        // 判断是否能添加
        KFMapModuleFunction<std::string, KFCheckAddElementFunction> _check_add_element_function;

        // 添加元数据函数
        KFMapModuleFunction<std::string, KFAddElementFunction> _add_element_function;

        // 判断元数据满足
        KFMapModuleFunction<std::string, KFCheckRemoveElementFunction> _check_remove_element_function;

        // 删除元数据函数
        KFMapModuleFunction<std::string, KFRemoveElementFunction> _remove_element_function;
        /////////////////////////////////////////////////////////////////////////////////////////////
        // 属性更新回调列表
        KFMapModuleFunction<KFModule*, KFAddDataFunction> _add_data_module;
        KFMapModuleFunction<KFModule*, KFRemoveDataFunction> _remove_data_module;
        KFMapModuleFunction<KFModule*, KFUpdateDataFunction> _update_data_module;
        KFMapModuleFunction<KFModule*, KFUpdateStringFunction> _update_string_module;

        // 添加属性逻辑
        KFMap<std::string, KFDataFunction<KFAddDataFunction>> _add_logic_function;

        // 添加属性回调
        KFMap<RecordKeyType, KFDataFunction<KFAddDataFunction>> _add_data_function;

        // 删除属性逻辑
        KFMap<std::string, KFDataFunction<KFRemoveDataFunction>> _remove_logic_function;

        // 删除数据的回调函数
        KFMap<RecordKeyType, KFDataFunction<KFRemoveDataFunction>> _remove_data_function;

        // 更新数据的回调函数
        KFMap<DataKeyType, KFDataFunction<KFUpdateDataFunction>> _update_data_function;

        // 更新数据的回调函数
        KFMap<DataKeyType, KFDataFunction<KFUpdateStringFunction>> _update_string_function;
        /////////////////////////////////////////////////////////////////////////////////////////////
        KFModuleFunction<KFEntityFunction> _entity_init_function;
        KFModuleFunction<KFEntityFunction> entity_remove_function;
        KFModuleFunction<KFEntityFunction> _entity_run_function;
        KFModuleFunction<KFEntityFunction> _entity_after_run_function;
        KFModuleFunction<KFEntityFunction> _entity_delete_function;
        KFModuleFunction<KFSaveEntityFunction> _entity_save_function;
        /////////////////////////////////////////////////////////////////////////////////////////////
        /////////////////////////////////////////////////////////////////////////////////////////////
        // 对象同步更新函数
        KFModuleFunction<KFSyncFunction> _entity_sync_update_function;

        // 对象同步添加函数
        KFModuleFunction<KFSyncFunction> _entity_sync_add_function;

        // 对象同步删除函数
        KFModuleFunction<KFSyncFunction> _entity_sync_remove_function;

        // 显示奖励函数
        KFModuleFunction<KFShowElementFunction> _show_element_function;
        /////////////////////////////////////////////////////////////////////////////////////////////
        // 获得属性逻辑
        KFMapModuleFunction<std::string, KFGetConfigValueFunction> _get_config_value_function;

        // 日志函数
        KFMapModuleFunction<std::string, KFLogElementFunction> _log_element_function;

        // 操作结果
        KFMapModuleFunction<std::string, KFElementResultFunction> _element_result_function;

    public:
        // 组件名称
        std::string _name;

        // 属性配置
        std::shared_ptr<const KFDataSetting> _data_setting = nullptr;

        // 对象实例列表
        KFHashMap<uint64, KFEntity> _entity_list;
    protected:
        // 需要发送消息的对象
        std::set<EntityPtr> _sync_entity_list;
    };
}

#endif