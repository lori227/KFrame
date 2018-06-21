#ifndef __KF_COMPONENT_EX_H__
#define __KF_COMPONENT_EX_H__

#include "KFEntity.h"
#include "KFComponent.h"
#include "KFTimer/KFTimerInterface.h"

namespace KFrame
{
	class KFKernelModule;
	class KFComponentEx : public KFComponent
	{
	public:
		KFComponentEx();
		~KFComponentEx();

		virtual void InitEntity( KFEntity* kfentity );
		virtual void UnInitEntity( KFEntity* kfentity );

		virtual void Run();
		virtual void AfterRun();

		virtual void SetName( const std::string& name );
		virtual const std::string& GetName();

		void SetKernelModule( KFKernelModule* module );
		//////////////////////////////////////////////////////////////////////////////////////////////
		// 创建实体
		virtual KFEntity* CreateEntity( uint64 key );
		virtual KFEntity* CreateEntity( uint64 key, const KFMsg::PBObject* proto );

		// 添加实体
		virtual void AddEntity( uint64 key, KFEntity* kfentity );

		// 查找实体
		virtual KFEntity* FindEntity( uint64 key, const char* function, uint32 line );

		// 删除实体
		virtual bool RemoveEntity( uint64 key );
		virtual bool RemoveEntity( KFEntity* kfentity );

		// 实体数量
		virtual uint32 GetEntityCount();

		//////////////////////////////////////////////////////////////////////////////////////
		// 设置保存标记
		virtual void SetEntityDataMask( uint32 mask );

		// 启动保存定时器
		void StartSaveEntityTimer( KFEntity* kfentity );

		// 停止保存定时器
		void DeleteSaveEntity( KFEntity* kfentity );

		// 保存数据到数据库
		__KF_TIMER_FUNCTION__( OnTimerSaveEntity );
		//////////////////////////////////////////////////////////////////////////////////////

		// 遍历列表
		virtual KFEntity* FirstEntity();
		virtual KFEntity* NextEntity();
		///////////////////////////////////////////////////////////////////////////////////////////////
		// 属性更新回调函数
		void UpdateDataCallBack( KFEntity* kfentity, KFData* kfdata, const std::string& value );
		void UpdateDataCallBack( KFEntity* kfentity, KFData* kfdata, uint64 key, uint32 operate, uint64 value, uint64 oldvalue, uint64 newvalue );

		// 属性添加回调函数
		void AddDataCallBack( KFEntity* kfentity, KFData* kfparent, uint64 key, KFData* kfdata );

		// 属性删除回调函数
		void RemoveDataCallBack( KFEntity* kfentity, KFData* kfparent, uint64 key, KFData* kfdata );

		///////////////////////////////////////////////////////////////////////////////////////////////
		///////////////////////////////////////////////////////////////////////////////////////////////
		virtual void BindAddAgentFunction( const std::string& dataname, KFAddAgentFunction& function );
		virtual void UnRegisterAddAgentFunction( const std::string& dataname );
		
		virtual void BindCheckAgentFunction( const std::string& dataname, KFCheckAgentFunction& function );
		virtual void UnRegisterCheckAgentFunction( const std::string& dataname );

		virtual void BindRemoveAgentFunction( const std::string& dataname, KFRemoveAgentFunction& function );
		virtual void UnRegisterRemoveAgentFunction( const std::string& dataname );
		///////////////////////////////////////////////////////////////////////////////////////////////
		virtual void BindAddDataModule( const std::string& module, KFAddDataFunction& function );
		virtual void UnBindAddDataModule( const std::string& module );

		virtual void BindAddDataFunction( const std::string& dataname, KFAddDataFunction& function );
		virtual void UnRegisterAddDataFunction( const std::string& dataname );

		virtual void BindRemoveDataModule( const std::string& module, KFRemoveDataFunction& function );
		virtual void UnBindRemoveDataModule( const std::string& module );

		virtual void BindRemoveDataFunction( const std::string& dataname, KFRemoveDataFunction& function );
		virtual void UnRegisterRemoveDataFunction( const std::string& dataname );

		virtual void BindUpdateDataModule( const std::string& module, KFUpdateDataFunction& function );
		virtual void UnBindUpdateDataModule( const std::string& module );

		virtual void BindUpdateDataFunction( const std::string& parentname, const std::string& dataname, KFUpdateDataFunction& function );
		virtual void UnRegisterUpdateDataFunction( const std::string& parentname, const std::string& dataname );

		virtual void BindUpdateStringModule( const std::string& module, KFUpdateStringFunction& function );
		virtual void UnBindUpdateStringModule( const std::string& module );

		virtual void BindUpdateStringFunction( const std::string& parentname, const std::string& dataname, KFUpdateStringFunction& function );
		virtual void UnRegisterUpdateStringFunction( const std::string& parentname, const std::string& dataname );
		/////////////////////////////////////////////////////////////////////////////////////////////////////
		virtual void BindEntityInitializeFunction( KFEntityFunction& function );
		virtual void UnRegisterEntityInitializeFunction();

		virtual void BindEntityUninitializeFunction( KFEntityFunction& function );
		virtual void UnRegisterEntityUninitializeFunction();

		virtual void BindEntityRunFunction( KFEntityFunction& function );
		virtual void UnRegisterEntityRunFunction();

		virtual void BindEntityAfterRunFunction( KFEntityFunction& function );
		virtual void UnRegisterEntityAfterRunFunction();

		virtual void BindEntitySaveFunction( KFEntityFunction& function );
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
		virtual void BindShowRewardFunction( KFShowRewardFunction& function );
		virtual void UnRegisterShowRewardFunction();

	protected:
		// 保存
		void SaveEntity( KFEntity* kfentity, const char* function, uint32 line );

		// 删除
		void DeleteEntity( KFEntity* kfentity );
	public:
		KFKernelModule* _kf_kernel_module;

		// 对象实例列表
		KFMap< uint64, uint64, KFEntity > _entitys;
		/////////////////////////////////////////////////////////////////////////////////////////////
		
		// 添加元数据函数
		KFBind< std::string, const std::string&, KFAddAgentFunction > _add_agent_function;

		// 判断元数据满足
		KFBind< std::string, const std::string&, KFCheckAgentFunction > _check_agent_function;

		// 删除元数据函数
		KFBind< std::string, const std::string&, KFRemoveAgentFunction > _remove_agent_function;
		/////////////////////////////////////////////////////////////////////////////////////////////
		// 属性更新回调列表
		typedef std::pair< std::string, std::string > DataKeyType;
		KFBind< std::string, const std::string&, KFAddDataFunction > _add_data_module;
		KFBind< std::string, const std::string&, KFAddDataFunction > _add_data_function;

		// 删除数据的回调函数
		KFBind< std::string, const std::string&, KFRemoveDataFunction > _remove_data_module;
		KFBind< std::string, const std::string&, KFRemoveDataFunction > _remove_data_function;

		// 更新数据的回调函数
		KFBind< std::string, const std::string&, KFUpdateDataFunction > _update_data_module;
		KFBind< DataKeyType, const DataKeyType&, KFUpdateDataFunction > _update_data_function;

		// 更新数据的回调函数
		KFBind< std::string, const std::string&, KFUpdateStringFunction > _update_string_module;
		KFBind< DataKeyType, const DataKeyType&, KFUpdateStringFunction > _update_string_function;
		/////////////////////////////////////////////////////////////////////////////////////////////
		KFEntityFunction _entity_initialize_function;
		KFEntityFunction _entity_uninitialize_function;
		KFEntityFunction _entity_run_function;
		KFEntityFunction _entity_after_run_function;
		KFEntityFunction _entity_save_function;
		KFEntityFunction _entity_delete_function;
		/////////////////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////////////////
		// 对象同步更新函数
		KFSyncFunction _entity_sync_update_function;

		// 对象同步添加函数
		KFSyncFunction _entity_sync_add_function;

		// 对象同步删除函数
		KFSyncFunction _entity_sync_remove_function;

		// 显示奖励函数
		KFShowRewardFunction _show_reward_function;

	private:
		// 组件名称
		std::string _component_name;

		// 是否需要保存
		uint32 _entity_data_mask;
	};
}

#endif