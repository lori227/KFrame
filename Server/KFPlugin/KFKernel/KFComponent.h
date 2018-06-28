#ifndef __KF_COMPONENT_H__
#define __KF_COMPONENT_H__

#include "KFrame.h"
#include "KFEntity.h"
#include "KFCore/KFAgent.h"
#include "KFCore/KFData.h"

namespace KFrame
{
#define __NEED_TO_SAVE__	  1		// 需要保存
#define	__DELETE_AND_REMOVE__ 2		// 删除数据库
#define __DELETE_AND_SAVE__   4		// 删除保存数据库


	typedef std::function< void( KFEntity*, KFAgent*, float, const char*, uint32 ) > KFAddAgentFunction;
	typedef std::function< bool( KFEntity*, KFAgent*, const char*, uint32 ) > KFCheckAgentFunction;
	typedef std::function< void( KFEntity*, KFAgent*, const char*, uint32 ) > KFRemoveAgentFunction;

	typedef std::function<void( KFEntity*, KFData* kfparent, uint64 key, KFData* kfdata )> KFAddDataFunction;
	typedef std::function<void( KFEntity*, KFData* kfparent, uint64 key, KFData* kfdata )> KFRemoveDataFunction;
	typedef std::function<void( KFEntity*, uint64 key, KFData* kfdata, uint32 operate, uint64 value, uint64 oldvalue, uint64 newvalue )> KFUpdateDataFunction;
	typedef std::function<void( KFEntity*, KFData* kfdata, const std::string& value )> KFUpdateStringFunction;


	typedef std::function< void( KFEntity* ) > KFEntityFunction;
	typedef std::function< void( KFEntity*, const KFMsg::PBObject&  ) > KFSyncFunction;
	typedef std::function< void( KFEntity*, const std::string&, bool, const char*, uint32 ) > KFShowRewardFunction;

	////////////////////////////////////////////////////////////////////////////////////////////

	// 游戏中的组件, 负责属性回调时间
	__ST_CLASS__( KFComponent )
	{
	public:
		
		virtual void InitEntity( KFEntity* kfentity ) = 0;
		virtual void UnInitEntity( KFEntity* kfentity ) = 0;

		virtual void Run() = 0;
		virtual void AfterRun() = 0;

		virtual void SetName( const std::string& name ) = 0;
		virtual const std::string& GetName() = 0;
		/////////////////////////////////////////////////////////////////
		// 创建实体
		virtual KFEntity* CreateEntity( uint64 key ) = 0;
		virtual KFEntity* CreateEntity( uint64 key, const KFMsg::PBObject* data ) = 0;

		// 添加实体
		virtual void AddEntity( uint64 key, KFEntity* kfentity ) = 0;

		// 查找实体
		virtual KFEntity* FindEntity( uint64 key, const char* function, uint32 line ) = 0;

		// 删除实体, 保存数据库
		virtual bool RemoveEntity( uint64 key ) = 0;
		virtual bool RemoveEntity( KFEntity* kfentity ) = 0;

		// 实体数量
		virtual uint32 GetEntityCount() = 0;

		// 遍历列表
		virtual KFEntity* FirstEntity() = 0;
		virtual KFEntity* NextEntity() = 0;

		// 设置保存标记
		virtual void SetEntityDataMask( uint32 mask ) = 0;
		//////////////////////////////////////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////////////////////////////////////
		// 注册添加属性函数
		template< class T >
		void RegisterAddAgentFunction( const std::string& dataname, T* object, void (T::*handle)(KFEntity*, KFAgent*, float, const char*, uint32 ) )
		{
			KFAddAgentFunction function = std::bind( handle, object, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5 );
			BindAddAgentFunction( dataname, function );
		}

		virtual void BindAddAgentFunction( const std::string& dataname, KFAddAgentFunction& function ) = 0;
		virtual void UnRegisterAddAgentFunction( const std::string& dataname ) = 0;

		// 注册判断属性函数
		template< class T >
		void RegisterCheckAgentFunction( const std::string& dataname, T* object, bool (T::*handle)(KFEntity* kfentity, KFAgent* kfagent, const char* function, uint32 line ) )
		{
			KFCheckAgentFunction function = std::bind( handle, object, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4 );
			BindCheckAgentFunction( dataname, function );
		}

		virtual void BindCheckAgentFunction( const std::string& dataname, KFCheckAgentFunction& function ) = 0;
		virtual void UnRegisterCheckAgentFunction( const std::string& dataname ) = 0;

		// 注册删除属性函数
		template< class T >
		void RegisterRemoveAgentFunction( const std::string& dataname, T* object, void (T::*handle)(KFEntity* kfentity, KFAgent* kfagent, const char* function, uint32 line ) )
		{
			KFRemoveAgentFunction function = std::bind( handle, object, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4 );
			BindRemoveAgentFunction( dataname, function );
		}

		virtual void BindRemoveAgentFunction( const std::string& dataname, KFRemoveAgentFunction& function ) = 0;
		virtual void UnRegisterRemoveAgentFunction( const std::string& dataname ) = 0;
		////////////////////////////////////////////////////////////////////////////////////////////////////////
		////////////////////////////////////////////////////////////////////////////////////////////////////////
		// 注册添加属性回调函数
		template< class T >
		void RegisterAddDataModule( T* object, void (T::*handle)(KFEntity* kfentity, KFData* kfparent, uint64 key, KFData* kfdata) )
		{
			KFAddDataFunction function = std::bind( handle, object, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4 );
			BindAddDataModule( typeid( T ).name(), function );
		}

		template< class T >
		void UnRegisterAddDataModule( T* object )
		{
			UnBindAddDataModule( typeid( T ).name() );
		}

		virtual void BindAddDataModule( const std::string& module, KFAddDataFunction& function ) = 0;
		virtual void UnBindAddDataModule( const std::string& module ) = 0;
	
		template< class T >
		void RegisterAddDataFunction( const std::string& dataname, T* object, void (T::*handle)(KFEntity* kfentity, KFData* kfparent, uint64 key, KFData* kfdata) )
		{
			KFAddDataFunction function = std::bind( handle, object, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4 );
			BindAddDataFunction( dataname, function );
		}
		virtual void BindAddDataFunction( const std::string& dataname, KFAddDataFunction& function ) = 0;
		virtual void UnRegisterAddDataFunction( const std::string& dataname ) = 0;

		// 注册删除属性回调函数
		template< class T >
		void RegisterRemoveDataModule( T* object, void (T::*handle)(KFEntity* kfentity, KFData* kfparent, uint64 key, KFData* kfdata) )
		{
			KFRemoveDataFunction function = std::bind( handle, object, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4 );
			BindRemoveDataModule( typeid( T ).name(), function );
		}

		template< class T >
		void UnRegisterRemoveDataModule( T* object )
		{
			UnBindRemoveDataModule( typeid( T ).name() );
		}
		
		virtual void BindRemoveDataModule( const std::string& module, KFRemoveDataFunction& function ) = 0;
		virtual void UnBindRemoveDataModule( const std::string& module ) = 0;

		template< class T >
		void RegisterRemoveDataFunction( const std::string& dataname, T* object, void (T::*handle)(KFEntity* kfentity, KFData* kfparent, uint64 key, KFData* kfdata) )
		{
			KFRemoveDataFunction function = std::bind( handle, object, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4 );
			BindRemoveDataFunction( dataname, function );
		}
		virtual void BindRemoveDataFunction( const std::string& dataname, KFRemoveDataFunction& function ) = 0;
		virtual void UnRegisterRemoveDataFunction( const std::string& dataname ) = 0;

		// 注册更新属性回调函数
		template< class T >
		void RegisterUpdateDataModule( T* object, void (T::*handle)(KFEntity* kfentity, uint64 key, KFData* kfdata, uint32 operate, uint64 value, uint64 oldvalue, uint64 newvalue) )
		{
			KFUpdateDataFunction function = std::bind( handle, object,
				std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4,
				std::placeholders::_5, std::placeholders::_6, std::placeholders::_7 );
			BindUpdateDataModule( typeid( T ).name(), function );
		}

		template< class T >
		void UnRegisterUpdateDataModule( T* object )
		{
			UnBindUpdateDataModule( typeid( T ).name() );
		}

		virtual void BindUpdateDataModule( const std::string& module, KFUpdateDataFunction& function ) = 0;
		virtual void UnBindUpdateDataModule( const std::string& module ) = 0;

		template< class T >
		void RegisterUpdateDataFunction( const std::string& parentname, const std::string& dataname, T* object, void (T::*handle)(KFEntity* kfentity, uint64 key, KFData* kfdata, uint32 operate, uint64 value, uint64 oldvalue, uint64 newvalue) )
		{
			KFUpdateDataFunction function = std::bind( handle, object,
				std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4,
				std::placeholders::_5, std::placeholders::_6, std::placeholders::_7 );

			BindUpdateDataFunction( parentname, dataname, function );
		}
		virtual void BindUpdateDataFunction( const std::string& parentname, const std::string& dataname, KFUpdateDataFunction& function ) = 0;
		virtual void UnRegisterUpdateDataFunction( const std::string& parentname, const std::string& dataname ) = 0;
		
		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		// 注册更新属性回调函数
		template< class T >
		void RegisterUpdateStringModule( T* object, void ( T::*handle )( KFEntity* kfentity, KFData* kfdata, const std::string& value ) )
		{
			KFUpdateStringFunction function = std::bind( handle, object,
				std::placeholders::_1, std::placeholders::_2, std::placeholders::_3 );
			BindUpdateStringModule( typeid( T ).name(), function );
		}
		
		template< class T >
		void UnRegisterUpdateStringModule( T* object )
		{
			UnBindUpdateStringModule( typeid( T ).name() );
		}

		virtual void BindUpdateStringModule( const std::string& module, KFUpdateStringFunction& function ) = 0;
		virtual void UnBindUpdateStringModule( const std::string& module ) = 0;

		template< class T >
		void RegisterUpdateStringFunction( const std::string& parentname, const std::string& dataname, T* object, void ( T::*handle )( KFEntity* kfentity, KFData* kfdata, const std::string& value ) )
		{
			KFUpdateStringFunction function = std::bind( handle, object,
				std::placeholders::_1, std::placeholders::_2, std::placeholders::_3 );

			BindUpdateStringFunction( parentname, dataname, function );
		}
		virtual void BindUpdateStringFunction( const std::string& parentname, const std::string& dataname, KFUpdateStringFunction& function ) = 0;
		virtual void UnRegisterUpdateStringFunction( const std::string& parentname, const std::string& dataname ) = 0;

		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		template< class T >
		void RegisterEntityInitializeFunction( T* object, void (T::*handle)(KFEntity* kfentity) )
		{
			KFEntityFunction function = std::bind( handle, object, std::placeholders::_1 );
			BindEntityInitializeFunction( function );
		}
		virtual void BindEntityInitializeFunction( KFEntityFunction& function ) = 0;
		virtual void UnRegisterEntityInitializeFunction() = 0;

		template< class T >
		void RegisterEntityUninitializeFunction( T* object, void (T::*handle)(KFEntity* kfentity) )
		{
			KFEntityFunction function = std::bind( handle, object, std::placeholders::_1 );
			BindEntityUninitializeFunction( function );
		}
		virtual void BindEntityUninitializeFunction( KFEntityFunction& function ) = 0;
		virtual void UnRegisterEntityUninitializeFunction() = 0;
		
		template< class T >
		void RegisterEntityRunFunction( T* object, void (T::*handle)(KFEntity* kfentity) )
		{
			KFEntityFunction function = std::bind( handle, object, std::placeholders::_1 );
			BindEntityRunFunction( function );
		}
		virtual void BindEntityRunFunction( KFEntityFunction& function ) = 0;
		virtual void UnRegisterEntityRunFunction() = 0;

		template< class T >
		void RegisterEntityAfterRunFunction( T* object, void (T::*handle)(KFEntity* kfentity) )
		{
			KFEntityFunction function = std::bind( handle, object, std::placeholders::_1 );
			BindEntityAfterRunFunction( function );
		}
		virtual void BindEntityAfterRunFunction( KFEntityFunction& function ) = 0;
		virtual void UnRegisterEntityAfterRunFunction() = 0;
		
		template< class T >
		void RegisterEntitySaveFunction( T* object, void ( T::*handle )( KFEntity* kfentity ) )
		{
			KFEntityFunction function = std::bind( handle, object, std::placeholders::_1 );
			BindEntitySaveFunction( function );
		}
		virtual void BindEntitySaveFunction( KFEntityFunction& function ) = 0;
		virtual void UnRegisterEntitySaveFunction() = 0;

		template< class T >
		void RegisterEntityDeleteFunction( T* object, void ( T::*handle )( KFEntity* kfentity ) )
		{
			KFEntityFunction function = std::bind( handle, object, std::placeholders::_1 );
			BindEntityDeleteFunction( function );
		}
		virtual void BindEntityDeleteFunction( KFEntityFunction& function ) = 0;
		virtual void UnRegisterEntityDeleteFunction() = 0;
		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		template< class T >
		void RegisterSyncUpdateFunction( T* object, void (T::*handle)(KFEntity* kfentity, const KFMsg::PBObject& pbobject) )
		{
			KFSyncFunction function = std::bind( handle, object, std::placeholders::_1, std::placeholders::_2 );
			BindSyncUpdateFunction( function );
		}
		virtual void BindSyncUpdateFunction( KFSyncFunction& function ) = 0;
		virtual void UnRegisterSyncUpdateFunction() = 0;

		template< class T >
		void RegisterSyncAddFunction( T* object, void (T::*handle)(KFEntity* kfentity, const KFMsg::PBObject& pbobject) )
		{
			KFSyncFunction function = std::bind( handle, object, std::placeholders::_1, std::placeholders::_2 );
			BindSyncAddFunction( function );
		}
		virtual void BindSyncAddFunction( KFSyncFunction& function ) = 0;
		virtual void UnRegisterSyncAddFunction() = 0;

		template< class T >
		void RegisterSyncRemoveFunction( T* object, void (T::*handle)(KFEntity* kfentity, const KFMsg::PBObject& pbobject) )
		{
			KFSyncFunction function = std::bind( handle, object, std::placeholders::_1, std::placeholders::_2 );
			BindSyncRemoveFunction( function );
		}
		virtual void BindSyncRemoveFunction( KFSyncFunction& function ) = 0;
		virtual void UnRegisterSyncRemoveFunction() = 0;
		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		template< class T >
		void RegisterShowRewardFunction( T* object, void ( T::*handle )( KFEntity*, const std::string&, bool, const char*, uint32 ) )
		{
			KFShowRewardFunction function = std::bind( handle, object,
				std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5 );
			BindShowRewardFunction( function );
		}
		virtual void BindShowRewardFunction( KFShowRewardFunction& function ) = 0;
		virtual void UnRegisterShowRewardFunction() = 0;

	};
}

#endif