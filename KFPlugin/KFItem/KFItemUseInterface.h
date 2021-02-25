#ifndef __KF_ITEM_USE_INTERFACE_H__
#define __KF_ITEM_USE_INTERFACE_H__

#include "KFrame.h"

namespace KFrame
{
    class KFItemSetting;
    class KFItemTypeSetting;
    typedef std::function<bool( EntityPtr, DataPtr, std::shared_ptr<const KFItemSetting>, std::shared_ptr<const KFItemTypeSetting> )> KFItemUseFunction;
    //////////////////////////////////////////////////////////////////////////
    class KFItemUseInterface : public KFModule
    {
    public:
        // 注册检查道具是否可以使用函数
        template<class T>
        void RegisterCheckItemUseFunction( uint32 item_type, T* module, bool ( T::*handle )( EntityPtr, DataPtr, std::shared_ptr<const KFItemSetting>, std::shared_ptr<const KFItemTypeSetting> ) )
        {
            KFItemUseFunction function = std::bind( handle, module, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4 );
            BindCheckItemUseFunction( item_type, module, function );
        }

        template<class T>
        void UnRegisterCheckItemUseFunction( T* module, uint32 item_type )
        {
            UnBindCheckItemUseFunction( item_type, module );
        }
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // 注册道具使用函数
        template<class T>
        void RegisterItemUseFunction( uint32 item_type, T* module, bool ( T::*handle )( EntityPtr, DataPtr, std::shared_ptr<const KFItemSetting>, std::shared_ptr<const KFItemTypeSetting> ) )
        {
            KFItemUseFunction function = std::bind( handle, module, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4 );
            BindItemUseFunction( item_type, module, function );
        }

        template<class T>
        void UnRegisterItemUseFunction( T* module, uint32 item_type )
        {
            UnBindItemUseFunction( item_type, module );
        }

    protected:
        virtual void BindCheckItemUseFunction( uint32 item_type, KFModule* module, KFItemUseFunction& function ) = 0;
        virtual void UnBindCheckItemUseFunction( uint32 item_type, KFModule* module ) = 0;

        virtual void BindItemUseFunction( uint32 item_type, KFModule* module, KFItemUseFunction& function ) = 0;
        virtual void UnBindItemUseFunction( uint32 item_type, KFModule* module ) = 0;
    };
    __KF_INTERFACE__( _kf_item_use, KFItemUseInterface );

    ////////////////////////////////////////////////////////////////////////////////////////////
#define __KF_ITEM_USE_FUNCTION__( function )\
    void function( EntityPtr player, DataPtr item_data, std::shared_ptr<const KFItemSetting> item_setting, std::shared_ptr<const KFItemTypeSetting> item_type_setting )
#define __REGISTER_ITEM_USE__( item_type, function )\
    _kf_item->RegisterItemUseFunction( item_type, this, function )
#define __UN_ITEM_USE__( item_type )\
    _kf_item->UnRegisterItemUseFunction( this, item_type )

#define __KF_CHECK_ITEM_USE_FUNCTION__( function )\
    void function( EntityPtr player, DataPtr item_data, std::shared_ptr<const KFItemSetting> item_setting, std::shared_ptr<const KFItemTypeSetting> item_type_setting )
#define __REGISTER_CHECK_ITEM_USE__( item_type, function )\
    _kf_item->RegisterItemUseFunction( item_type, this, function )
#define __UN_CHECK_ITEM_USE__( item_type )\
    _kf_item->UnRegisterItemUseFunction( this, item_type )
}

#endif