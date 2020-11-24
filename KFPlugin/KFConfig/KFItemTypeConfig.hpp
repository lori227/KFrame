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
	class KFItemTypeConfig : public KFConfigT< KFItemTypeSetting >, public KFInstance< KFItemTypeConfig >
	{
	public:
		KFItemTypeConfig()
		{
			_key_name = "type";
			_file_name = "itemtype";
		}

		~KFItemTypeConfig() = default;

	protected:
		virtual void ReadSetting( KFXmlNode& xmlnode, KFItemTypeSetting* kfsetting )
		{
			kfsetting->_obtain_bag = xmlnode.ReadString( "obtainbag", true );
			kfsetting->_move_bag_list = xmlnode.ReadStringSet( "movebaglist", true );
			kfsetting->_tab_list = xmlnode.ReadStringSet( "tablist", true );
			kfsetting->_use_limit = xmlnode.ReadUInt32( "uselimit", true );
			kfsetting->_sort = xmlnode.ReadUInt32( "sort", true );
		}

	};

	/////////////////////////////////////////////////////////////////////////////////
}
#endif
