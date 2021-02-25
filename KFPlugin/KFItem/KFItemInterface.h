#ifndef __KF_ITEM_INTERFACE_H__
#define __KF_ITEM_INTERFACE_H__

#include "KFrame.h"

namespace KFrame
{
    class KFItemSetting;
    typedef std::function< void( EntityPtr, DataPtr, std::shared_ptr<const KFItemSetting> ) > KFItemInitFunction;

    class KFItemInterface : public KFModule
    {
    public:
        // 背包格子最大数量
        virtual uint32 GetItemBagMaxCount( EntityPtr player, DataPtr item_record ) = 0;

        // 创建一个道具( 可传入一个存在的item_data, 直接返回 )
        virtual DataPtr CreateItem( EntityPtr player, uint32 item_id, DataPtr item_data, const char* function, uint32 line ) = 0;

        // 获取能添加物品数量
        virtual uint32 GetCanAddItemCount( EntityPtr player, uint32 item_id, uint32 count ) = 0;

        // 查找道具  返回<背包指针, 道具指针>
        virtual std::tuple<DataPtr, DataPtr> FindItem( EntityPtr player, uint64 item_uuid ) = 0;

        // 获得背包空格子数量
        virtual uint32 GetItemEmptyCount( EntityPtr player, DataPtr item_record ) = 0;

        // 判断包裹是否满了
        virtual bool CheckItemBagFull( EntityPtr player, DataPtr item_record ) = 0;

        // 获得包裹内道具数量( max_count 最多查找数量, 优化用 )
        virtual uint32 GetItemCount( EntityPtr player, DataPtr item_record, uint32 item_id, uint32 max_count = __MAX_UINT32__ ) = 0;

        // 删除包裹内道具
        virtual void RemoveItem( EntityPtr player, DataPtr item_record, uint32 item_id, uint32 item_count ) = 0;
        virtual uint32 RemoveItemCount( EntityPtr player, DataPtr item_data, uint32 item_count ) = 0;

        // 获得背包属性
        virtual DataPtr FindItemBag( EntityPtr player, uint32 item_id ) = 0;
        virtual DataPtr FindItemBag( EntityPtr player, DataPtr item_data ) = 0;

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // 注册道具初始化函数
        template<class T>
        void RegisterItemInitFunction( uint32 item_type, T* module, void ( T::*handle )( EntityPtr, DataPtr, std::shared_ptr<const KFItemSetting> ) )
        {
            KFItemInitFunction function = std::bind( handle, module, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3 );
            BindItemInitFunction( item_type, module, function );
        }

        template<class T>
        void UnRegisterItemInitFunction( T* module, uint32 item_type )
        {
            UnBindItemInitFunction( item_type, module );
        }

    protected:
        virtual void BindItemInitFunction( uint32 item_type, KFModule* module, KFItemInitFunction& function ) = 0;
        virtual void UnBindItemInitFunction( uint32 item_type, KFModule* module ) = 0;
    };
    __KF_INTERFACE__( _kf_item, KFItemInterface );

#define __KF_ITEM_INIT_FUNCTION__( function )\
    void function( EntityPtr player, DataPtr item_data, std::shared_ptr<const KFItemSetting> setting )
#define __REGISTER_ITEM_INIT__( item_type, function )\
    _kf_item->RegisterItemInitFunction( item_type, this, function )
#define __UN_ITEM_INIT__( item_type )\
    _kf_item->UnRegisterItemInitFunction( this, item_type )


}

#endif