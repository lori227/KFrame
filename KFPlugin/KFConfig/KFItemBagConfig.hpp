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
		virtual void ReadSetting( KFXmlNode& xml_node, std::shared_ptr<KFItemBagSetting> setting )
		{
			setting->_max_count = xml_node.ReadUInt32( "maxcount", true );
			setting->_index_name = xml_node.ReadString( "indexname", true );
			setting->_tab_list = xml_node.ReadStringSet( "tablist", true );
			setting->_extend_bag = xml_node.ReadString( "extendbag", true );
			setting->_use_child_bag = xml_node.ReadStringList( "usechildbag", true );
			setting->_update_add_from_source = xml_node.ReadStringSet( "updateaddfromsource", true );
			setting->_update_move_to_target = xml_node.ReadStringSet( "updatemovetotarget", true );
			setting->_show_move_to_target = xml_node.ReadStringSet( "showmovetotarget", true );
			setting->_show_when_add = xml_node.ReadBool( "showwhenadd", true );
			setting->_can_move = xml_node.ReadBool( "canmove", true );
			setting->_can_move_all = xml_node.ReadBool( "canmoveall", true );
			setting->_can_cleanup = xml_node.ReadBool( "cancleanup", true );
		}

	};

	/////////////////////////////////////////////////////////////////////////////////
}
#endif
