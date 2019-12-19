
// KFDeployDlg.h: 头文件
//

#pragma once
#include "KFDeployData.h"
#include "KFProtocol/KFProtocol.h"
#include "KFMessage/KFMessageInterface.h"
#include "KFTcpClient/KFTcpClientInterface.h"
#include "CMyListBox.h"
using namespace KFrame;

class CAddAgentDlg;
class CAddServerDlg;
class CVersionDlg;
class CFileDlg;
class CResourceDlg;
class CPluginDlg;

// CKFDeployDlg 对话框
class CKFDeployDlg : public CDialogEx
{
    // 构造
public:
    CKFDeployDlg( CWnd* pParent = nullptr );	// 标准构造函数

    // 对话框数据
#ifdef AFX_DESIGN_TIME
    enum { IDD = IDD_KFDEPLOY_DIALOG };
#endif

public:
    //////////////////////////////////////////////////////////////////////////////////////////////
    // 查询数据库
    void QueryTableValues( const std::string& table, StringMap& keys );

    // 删除数据库
    void DeleteTableValues( const std::string& table, StringMap& keys );

    // 执行数据库
    void ExecuteTableValues( const std::string& table, StringMap& values );
    //////////////////////////////////////////////////////////////////////////////////////////////
protected:
    virtual void DoDataExchange( CDataExchange* pDX );	// DDX/DDV 支持

    // 注册消息和时间
    void RegisterEventMessage();

    // 初始化插件
    void InitPlugin();

    // 读取配置
    void LoadDeployData( const char* file );
    void LoadAppData( const char* file );
    void LoadServiceName( const char* file );
    void LoadCommandData( const char* file );

    // 初始化界面控件
    void InitDialogData();
    void DestoryDialogData();

    // 添加日志
    void AddDeployLog( uint64 id, std::string content );

    // 清空日志
    void ClearDeployLog();

    // 发送消息
    void SendDeployMessage( uint32 msgid, google::protobuf::Message* message );

    // 获得局域网ip
    std::string GetLocalIp();
    //////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////
    // 连接成功
    __KF_NET_EVENT_FUNCTION__( OnClientConnectServer );

    // 连接失败
    __KF_NET_EVENT_FUNCTION__( OnClientConnectFailed );

    // 断开连接
    __KF_NET_EVENT_FUNCTION__( OnClientLostServer );
    //////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////
    // 日志消息
    __KF_MESSAGE_FUNCTION__( HandleDeployLogToToolAck );

    // 查询mysql结果
    __KF_MESSAGE_FUNCTION__( HandleDeployQueryMySQLAck );

    //agent list
    void QueryAgentData();
    void RefreshAgentData( const KFMsg::PBMySQLDatas* pbdatas );

    // 删除mysql结果
    __KF_MESSAGE_FUNCTION__( HandleDeployDeleteMySQLAck );

    // 执行mysql结果
    __KF_MESSAGE_FUNCTION__( HandleDeployExecuteMySQLAck );
    void QueryServerList();
    void RefreshServerList( const KFMsg::PBMySQLDatas* pbdatas );

    // 查询id
    __KF_MESSAGE_FUNCTION__( HandleDeployQueryToolIdAck );
    //////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////

protected:
    CAddAgentDlg* _add_agent_dlg = nullptr;
    CAddServerDlg* _add_server_dlg = nullptr;
    CVersionDlg* _version_dlg = nullptr;
    CFileDlg* _file_dlg = nullptr;
    CResourceDlg* _resource_dlg = nullptr;
    CPluginDlg* _plugin_dlg = nullptr;

    // 实现
protected:
    HICON m_hIcon;

    // 生成的消息映射函数
    virtual BOOL OnInitDialog();
    afx_msg void OnPaint();
    afx_msg HCURSOR OnQueryDragIcon();
    DECLARE_MESSAGE_MAP()
public:
    afx_msg void OnClose();
    afx_msg void OnTimer( UINT_PTR nIDEvent );
    virtual BOOL PreTranslateMessage( MSG* pMsg );

    CListBox _list_deploy;
    afx_msg void OnLbnSelchangeListDeploy();
    CEdit _edit_deploy_name;
    CEdit _edit_deploy_channel;
    CEdit _edit_deploy_id;
    CEdit _edit_deploy_ip;
    CEdit _edit_deploy_port;
    CButton _button_connect;
    afx_msg void OnBnClickedButtonConnectDeploy();
    CMyListBox _list_log;
    CListCtrl _list_agent;
    afx_msg void OnNMRClickListAgent( NMHDR* pNMHDR, LRESULT* pResult );
    afx_msg void OnNMRClickListServer( NMHDR* pNMHDR, LRESULT* pResult );
    afx_msg void OnMenuDeleteAgent();
    afx_msg void OnMenuAddAgent();
    CListCtrl _list_server;
    afx_msg void OnMenuQueryServerList();
    afx_msg void OnMenuQueryAgent();
    afx_msg void OnMenuDeleteServerList();
    afx_msg void OnMenuAddServer();
    CComboBox _combo_command;
    CButton _button_version;
    CButton _button_file;
    CComboBox _combo_log;
    CComboBox _combo_name;
    CComboBox _combo_type;
    CEdit _edit_param;
    CComboBox _combo_zone;
    CComboBox _combo_id;
    CDateTimeCtrl _date_time;
    afx_msg void OnBnClickedButtonAddCommand();
    afx_msg void OnCbnSelchangeComboCommand();
    afx_msg void OnCbnSelchangeComboLog();
    afx_msg void OnCbnSelchangeComboName();
    afx_msg void OnCbnSelchangeComboType();
    afx_msg void OnCbnSelchangeComboZone();
    afx_msg void OnBnClickedButtonQueryVersion();
    afx_msg void OnBnClickedButtonQueryFile();
    afx_msg void OnEnumClearLog();
    afx_msg void OnBnClickedButtonQueryResource();
    CButton _button_resource;
    CButton _button_plugin;
    afx_msg void OnBnClickedButtonQueryPlugin();
};
