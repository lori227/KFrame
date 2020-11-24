#ifndef	__KF_ITEMBAG_CONFIG_H__
#define	__KF_ITEMBAG_CONFIG_H__

#include "KFConfig.h"

namespace KFrame
{
	/////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////
	class KFItemBagSetting : public KFStrSetting
	{
	public:
		// 最大数量
		uint32 _max_count = 0u;

		// 主索引的名字
		std::string _index_name;

		// 页签列表
		StringSet _tab_list;

		// 满包是否弹出额外背包
		std::string _extend_bag;

		// 可同时使用的背包
		StringList _use_child_bag;

		// 来自源背包的移动道具添加需要更新
		StringSet _update_add_from_source;

		// 移动到目标背包的道具需要更新
		StringSet _update_move_to_target;

		// 移动需要客户端飘字的背包
		StringSet _show_move_to_target;

		// 添加道具是否通知客户端飘字
		bool _show_when_add = false;

		// 是否支持直接移动
		bool _can_move = false;

		// 是否支持一键移动所有
		bool _can_move_all = false;

		// 是否支持一键清空
		bool _can_cleanup = false;

	};

	/////////////////////////////////////////////////////////////////////////////////
	class KFItemBagConfig : public KFConfigT< KFItemBagSetting >, public KFInstance< KFItemBagConfig >
	{
	public:
		KFItemBagConfig()
		{
			_key_name = "id";
			_file_name = "itembag";
		}

		~KFItemBagConfig() = default;

	protected:
		virtual void ReadSetting( KFXmlNode& xmlnode, KFItemBagSetting* kfsetting )
		{
			kfsetting->_max_count = xmlnode.ReadUInt32( "maxcount", true );
			kfsetting->_index_name = xmlnode.ReadString( "indexname", true );
			kfsetting->_tab_list = xmlnode.ReadStringSet( "tablist", true );
			kfsetting->_extend_bag = xmlnode.ReadString( "extendbag", true );
			kfsetting->_use_child_bag = xmlnode.ReadStringList( "usechildbag", true );
			kfsetting->_update_add_from_source = xmlnode.ReadStringSet( "updateaddfromsource", true );
			kfsetting->_update_move_to_target = xmlnode.ReadStringSet( "updatemovetotarget", true );
			kfsetting->_show_move_to_target = xmlnode.ReadStringSet( "showmovetotarget", true );
			kfsetting->_show_when_add = xmlnode.ReadBoolen( "showwhenadd", true );
			kfsetting->_can_move = xmlnode.ReadBoolen( "canmove", true );
			kfsetting->_can_move_all = xmlnode.ReadBoolen( "canmoveall", true );
			kfsetting->_can_cleanup = xmlnode.ReadBoolen( "cancleanup", true );
		}

	};

	/////////////////////////////////////////////////////////////////////////////////
}
#endif
