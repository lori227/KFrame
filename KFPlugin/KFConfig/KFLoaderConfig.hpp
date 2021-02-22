#ifndef	__KF_LOADER_CONFIG_H__
#define	__KF_LOADER_CONFIG_H__

#include "KFConfig.h"

namespace KFrame
{
	/////////////////////////////////////////////////////////////////////////////////
	class KFConfigData
	{
	public:
		// ������
		std::string _name;

		// �����ļ�·��
		std::string _path;

		// ���ļ�������(���ļ����¼��غ���Ҫ���¼���)
		std::string _parent_name;

		// �Ƿ�������¼���
		bool _can_reload = false;

		// �Ƿ���Ҫ�������
		uint32 _clear_type = 0u;

	};

	/////////////////////////////////////////////////////////////////////////////////
	class KFLoaderSetting : public KFStrSetting
	{
	public:
		// �����ļ��б�
		std::vector<KFConfigData> _config_data;
	};

	/////////////////////////////////////////////////////////////////////////////////
	class KFLoaderConfig : public KFConfigT< KFLoaderSetting >, public KFInstance< KFLoaderConfig >
	{
	public:
		KFLoaderConfig()
		{
			_key_name = "id";
			_file_name = "loader";
		}

		~KFLoaderConfig() = default;

	protected:
		virtual void ReadSetting( KFXmlNode& xml_node, std::shared_ptr<KFLoaderSetting> setting )
		{
			KFConfigData config_data;
            config_data._name = xml_node.ReadString( "name", true );
            config_data._path = xml_node.ReadString( "path", true );
            config_data._parent_name = xml_node.ReadString( "parent_name", true );
            config_data._can_reload = xml_node.ReadBool( "canreload", true );
            config_data._clear_type = xml_node.ReadUInt32( "cleartype", true );
            setting->_config_data.push_back( config_data );
		}

	};

	/////////////////////////////////////////////////////////////////////////////////
}
#endif
