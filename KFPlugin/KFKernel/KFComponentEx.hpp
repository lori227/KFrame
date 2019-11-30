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
        void AddFunction( const std::string& name, T& function )
        {
            auto kffunction = _functions.Create( name );
            kffunction->_function = function;
        }

        void RemoveFunction( const std::string& name )
        {
            _functions.Remove( name );
        }

        template< class ... Arg >
        void CallFunction( Arg&& ...params )
        {
            for ( auto& iter : _functions._objects )
            {
                auto kffunction = iter.second;
                kffunction->_function( std::forward<Arg>( params )... );
            }
        }

    public:
        // 函数列表
        KFBind< std::string, const std::string&, T > _functions;
    };
    ///////////////////////////////////////////////////////////////////////////////////////////////////////
    class KFComponentEx : public KFComponent
    {
    public:
        KFComponentEx();
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
        virtual VectorString& GetDataList( const std::string& dataname );

        // 获得类配置
        virtual const KFClassSetting* FindClassSetting();
        virtual const KFClassSetting* FindClassSetting( const std::string& dataname );

        // 获得属性配置
        virtual const KFDataSetting* FindDataSetting();
        virtual const KFDataSetting* FindDataSetting( const std::string& dataname );
        virtual const KFDataSetting* FindDataSetting( const std::string& parentname, const std::string& dataname );
        //////////////////////////////////////////////////////////////////////////////////////
        virtual void SetLogicValue( const std::string& dataname, uint32 value );
        virtual void SetLogicValue( const std::string& parentname, const std::string& dataname, uint32 value );

        virtual void SetMaxValue( const std::string& dataname, uint32 value );
        virtual void SetMaxValue( const std::string& parentname, const std::string& dataname, uint32 value );
        //////////////////////////////////////////////////////////////////////////////////////

        // 启动保存定时器
        void StartSaveEntityTimer( KFEntity* kfentity, KFData* kfdata );

        // 停止保存定时器
        void DeleteSaveEntity( KFEntity* kfentity );
        //////////////////////////////////////////////////////////////////////////////////////
        // 属性更新回调函数
        void UpdateDataCallBack( KFEntity* kfentity, KFData* kfdata, const std::string& value, bool callback );
        void UpdateDataCallBack( KFEntity* kfentity, uint64 key, KFData* kfdata, uint64 index, uint32 operate, uint64 value, uint64 oldvalue, uint64 newvalue, bool callback );

        // 属性添加回调函数
        void AddDataCallBack( KFEntity* kfentity, KFData* kfparent, uint64 key, KFData* kfdata, bool callback );

        // 属性删除回调函数
        void RemoveDataCallBack( KFEntity* kfentity, KFData* kfparent, uint64 key, KFData* kfdata, bool callback );

        // 添加更新的对象
        void AddSyncEntity( KFEntity* entity );

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
        virtual void BindAddDataModule( const std::string& module, KFAddDataFunction& function );
        virtual void UnBindAddDataModule( const std::string& module );
        virtual void BindAddDataFunction( const std::string& module, const std::string& dataname, uint64 key, KFAddDataFunction& function );
        virtual void UnBindAddDataFunction( const std::string& module, const std::string& dataname, uint64 key );

        virtual void BindRemoveDataModule( const std::string& module, KFRemoveDataFunction& function );
        virtual void UnBindRemoveDataModule( const std::string& module );
        virtual void BindRemoveDataFunction( const std::string& module, const std::string& dataname, uint64 key, KFRemoveDataFunction& function );
        virtual void UnBindRemoveDataFunction( const std::string& module, const std::string& dataname, uint64 key );

        virtual void BindUpdateDataModule( const std::string& module, KFUpdateDataFunction& function );
        virtual void UnBindUpdateDataModule( const std::string& module );
        virtual void BindUpdateDataFunction( const std::string& module, const std::string& parentname, const std::string& dataname, KFUpdateDataFunction& function );
        virtual void UnBindUpdateDataFunction( const std::string& module, const std::string& parentname, const std::string& dataname );

        virtual void BindUpdateStringModule( const std::string& module, KFUpdateStringFunction& function );
        virtual void UnBindUpdateStringModule( const std::string& module );
        virtual void BindUpdateStringFunction( const std::string& module, const std::string& parentname, const std::string& dataname, KFUpdateStringFunction& function );
        virtual void UnBindUpdateStringFunction( const std::string& module, const std::string& parentname, const std::string& dataname );
        /////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual void BindEntityInitializeFunction( KFEntityFunction& function );
        virtual void UnRegisterEntityInitializeFunction();

        virtual void BindEntityUninitializeFunction( KFEntityFunction& function );
        virtual void UnRegisterEntityUninitializeFunction();

        virtual void BindEntityRunFunction( KFEntityFunction& function );
        virtual void UnRegisterEntityRunFunction();

        virtual void BindEntityAfterRunFunction( KFEntityFunction& function );
        virtual void UnRegisterEntityAfterRunFunction();

        virtual void BindEntitySaveFunction( KFSaveEntityFunction& function );
        virtual void UnRegisterEntitySaveFunction();

        virtual void BindEntityDeleteFunction( KFEntityFunction& function );
        virtual void UnRegisterEntityDeleteFunction();
        /////////////////////////////////////////////////////////////////////////////////////////////////////
        /////////////////////////////////////////////////////////////////////////////////////////////////////

        virtual void BindSyncUpdateFunction( KFSyncFunction& function );
        virtual void UnRegisterSyncUpdateFunction();

        virtual void BindSyncAddFunction( KFSyncFunction& function );
        virtual void UnRegisterSyncAddFunction();

        virtual void BindSyncRemoveFunction( KFSyncFunction& function );
        virtual void UnRegisterSyncRemoveFunction();
        /////////////////////////////////////////////////////////////////////////////////////////////////////
        /////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual void BindShowElementFunction( KFShowElementFunction& function );
        virtual void UnRegisterShowElementFunction();
        /////////////////////////////////////////////////////////////////////////////////////////////////////
        /////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual void BindGetConfigValueFunction( const std::string& name, KFGetConfigValueFunction& function );
        virtual void UnBindGetConfigValueFunction( const std::string& name );
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
        const DataKeyType& FormatDataKey( const std::string& parentname, const std::string& dataname );
    public:
        // 对象实例列表
        KFHashMap< uint64, uint64, KFEntity > _entitys;
        /////////////////////////////////////////////////////////////////////////////////////////////
        // 判断是否能添加
        KFBind< std::string, const std::string&, KFCheckAddElementFunction > _check_add_element_function;

        // 添加元数据函数
        KFBind< std::string, const std::string&, KFAddElementFunction > _add_element_function;

        // 判断元数据满足
        KFBind< std::string, const std::string&, KFCheckRemoveElementFunction > _check_remove_element_function;

        // 删除元数据函数
        KFBind< std::string, const std::string&, KFRemoveElementFunction > _remove_element_function;
        /////////////////////////////////////////////////////////////////////////////////////////////
        // 属性更新回调列表
        KFBind< std::string, const std::string&, KFAddDataFunction > _add_data_module;
        KFBind< std::string, const std::string&, KFRemoveDataFunction > _remove_data_module;
        KFBind< std::string, const std::string&, KFUpdateDataFunction > _update_data_module;
        KFBind< std::string, const std::string&, KFUpdateStringFunction > _update_string_module;

        // 添加属性回调
        KFMap< RecordKeyType, const RecordKeyType&, KFDataFunction< KFAddDataFunction > > _add_data_function;

        // 删除数据的回调函数
        KFMap< RecordKeyType, const RecordKeyType&, KFDataFunction< KFRemoveDataFunction > > _remove_data_function;

        // 更新数据的回调函数
        KFMap< DataKeyType, const DataKeyType&, KFDataFunction< KFUpdateDataFunction > > _update_data_function;

        // 更新数据的回调函数
        KFMap< DataKeyType, const DataKeyType&, KFDataFunction< KFUpdateStringFunction > > _update_string_function;
        /////////////////////////////////////////////////////////////////////////////////////////////
        KFEntityFunction _entity_initialize_function;
        KFEntityFunction _entity_uninitialize_function;
        KFEntityFunction _entity_run_function;
        KFEntityFunction _entity_after_run_function;
        KFEntityFunction _entity_delete_function;
        KFSaveEntityFunction _entity_save_function;
        /////////////////////////////////////////////////////////////////////////////////////////////
        /////////////////////////////////////////////////////////////////////////////////////////////
        // 对象同步更新函数
        KFSyncFunction _entity_sync_update_function;

        // 对象同步添加函数
        KFSyncFunction _entity_sync_add_function;

        // 对象同步删除函数
        KFSyncFunction _entity_sync_remove_function;

        // 显示奖励函数
        KFShowElementFunction _show_element_function;
        /////////////////////////////////////////////////////////////////////////////////////////////
        // 获得属性逻辑
        KFBind< std::string, const std::string&, KFGetConfigValueFunction > _get_config_value_function;

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