#ifndef	__KF_ITEMTYPE_CONFIG_H__
#define	__KF_ITEMTYPE_CONFIG_H__

#include "KFConfig.h"

namespace KFrame
{
	/////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////
	class KFItemTypeSetting : public KFIntSetting
	{
	public:
		// 获得时放入的背包名字
		std::string _obtain_bag;

		// 可移动的背包/仓库名
		StringSet _move_bag_list;

		// 可放入子页签名字
		StringSet _tab_list;

		// 使用限制
		uint32 _use_limit = 0u;

		// 排序顺序
		uint32 _sort = 0u;

	};

	/////////////////////////////////////////////////////////////////////////////////
	class KFItemTypeConfig : public KFConfigT<KFItemTypeSetting>, public KFInstance<KFItemTypeConfig>
	{
	public:
		KFItemTypeConfig()
		{
			_key_name = "type";
			_file_name = "itemtype";
		}

		~KFItemTypeConfig() = default;

	protected:
		virtual void ReadSetting( KFXmlNode& xml_node, std::shared_ptr<KFItemTypeSetting> setting )
		{
			setting->_obtain_bag = xml_node.ReadString( "obtainbag", true );
			setting->_move_bag_list = xml_node.ReadStringSet( "movebaglist", true );
			setting->_tab_list = xml_node.ReadStringSet( "tablist", true );
			setting->_use_limit = xml_node.ReadUInt32( "uselimit", true );
			setting->_sort = xml_node.ReadUInt32( "sort", true );
		}

	};

	/////////////////////////////////////////////////////////////////////////////////
}
#endif
