
// KFDeployDlg.cpp: 实现文件
//

#include "stdafx.h"
#include "KFDeploy.h"
#include "KFDeployDlg.h"
#include "afxdialogex.h"
#include "KFDeployService.h"
#include "CAddAgentDlg.h"
#include "CAddServerDlg.h"
#include "CVersionDlg.h"
#include "CFileDlg.h"


#define __SERVICE_TIMER_ID__ 666

#ifdef _DEBUG
    #define new DEBUG_NEW
#endif


// CKFDeployDlg 对话框


CKFDeployDlg::CKFDeployDlg( CWnd* pParent /*=nullptr*/ )
    : CDialogEx( IDD_KFDEPLOY_DIALOG, pParent )
{
    m_hIcon = AfxGetApp()->LoadIcon( IDR_MAINFRAME );
}

void CKFDeployDlg::DoDataExchange( CDataExchange* pDX )
{
    CDialogEx::DoDataExchange( pDX );
    DDX_Control( pDX, IDC_LIST1, _list_deploy );
    DDX_Control( pDX, IDC_EDIT1, _edit_deploy_name );
    DDX_Control( pDX, IDC_EDIT2, _edit_deploy_channel );
    DDX_Control( pDX, IDC_EDIT5, _edit_deploy_id );
    DDX_Control( pDX, IDC_EDIT3, _edit_deploy_ip );
    DDX_Control( pDX, IDC_EDIT4, _edit_deploy_port );
    DDX_Control( pDX, IDC_BUTTON1, _button_connect );
    DDX_Control( pDX, IDC_LIST2, _list_log );
    DDX_Control( pDX, IDC_LIST3, _list_agent );
    DDX_Control( pDX, IDC_LIST4, _list_server );
    DDX_Control( pDX, IDC_COMBO2, _combo_command );
    DDX_Control( pDX, IDC_BUTTON2, _button_version );
    DDX_Control( pDX, IDC_BUTTON3, _button_file );
    DDX_Control( pDX, IDC_COMBO1, _combo_log );
    DDX_Control( pDX, IDC_COMBO10, _combo_name );
    DDX_Control( pDX, IDC_COMBO11, _combo_type );
    DDX_Control( pDX, IDC_EDIT6, _edit_param );
    DDX_Control( pDX, IDC_COMBO12, _combo_zone );
    DDX_Control( pDX, IDC_COMBO13, _combo_id );
    DDX_Control( pDX, IDC_DATETIMEPICKER1, _date_time );
}

BEGIN_MESSAGE_MAP( CKFDeployDlg, CDialogEx )
    ON_WM_PAINT()
    ON_WM_QUERYDRAGICON()
    ON_WM_CLOSE()
    ON_WM_TIMER()
    ON_LBN_SELCHANGE( IDC_LIST1, &CKFDeployDlg::OnLbnSelchangeListDeploy )
    ON_BN_CLICKED( IDC_BUTTON1, &CKFDeployDlg::OnBnClickedButtonConnectDeploy )
    ON_NOTIFY( NM_RCLICK, IDC_LIST3, &CKFDeployDlg::OnNMRClickListAgent )
    ON_COMMAND( ID__32771, &CKFDeployDlg::OnMenuDeleteAgent )
    ON_COMMAND( ID_Menu, &CKFDeployDlg::OnMenuAddAgent )
    ON_NOTIFY( NM_RCLICK, IDC_LIST4, &CKFDeployDlg::OnNMRClickListServer )
    ON_COMMAND( ID_32774, &CKFDeployDlg::OnMenuQueryServerList )
    ON_COMMAND( ID_Menu32777, &CKFDeployDlg::OnMenuQueryAgent )
    ON_COMMAND( ID_32776, &CKFDeployDlg::OnMenuDeleteServerList )
    ON_COMMAND( ID_32775, &CKFDeployDlg::OnMenuAddServer )
    ON_BN_CLICKED( IDC_BUTTON4, &CKFDeployDlg::OnBnClickedButtonAddCommand )
    ON_CBN_SELCHANGE( IDC_COMBO2, &CKFDeployDlg::OnCbnSelchangeComboCommand )
    ON_CBN_SELCHANGE( IDC_COMBO1, &CKFDeployDlg::OnCbnSelchangeComboLog )
    ON_CBN_SELCHANGE( IDC_COMBO10, &CKFDeployDlg::OnCbnSelchangeComboName )
    ON_CBN_SELCHANGE( IDC_COMBO11, &CKFDeployDlg::OnCbnSelchangeComboType )
    ON_CBN_SELCHANGE( IDC_COMBO12, &CKFDeployDlg::OnCbnSelchangeComboZone )
    ON_BN_CLICKED( IDC_BUTTON2, &CKFDeployDlg::OnBnClickedButtonQueryVersion )
    ON_BN_CLICKED( IDC_BUTTON3, &CKFDeployDlg::OnBnClickedButtonQueryFile )
END_MESSAGE_MAP()


// CKFDeployDlg 消息处理程序

BOOL CKFDeployDlg::OnInitDialog()
{
    CDialogEx::OnInitDialog();

    // 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
    //  执行此操作
    SetIcon( m_hIcon, TRUE );			// 设置大图标
    SetIcon( m_hIcon, FALSE );		// 设置小图标

    // TODO: 在此添加额外的初始化代码
    SetTimer( __SERVICE_TIMER_ID__, 10, NULL );
    _kf_deploy_service->InitServices();
    _deploy_manage->_dlg_deploy = this;

    InitPlugin();
    RegisterEventMessage();
    LoadDeployData( KFDeploy::_deploy_file );
    LoadAppData( KFDeploy::_deploy_file );
    LoadServiceName( KFDeploy::_deploy_file );
    LoadCommandData( KFDeploy::_deploy_file );

    InitDialogData();
    return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CKFDeployDlg::OnPaint()
{
    if ( IsIconic() )
    {
        CPaintDC dc( this ); // 用于绘制的设备上下文

        SendMessage( WM_ICONERASEBKGND, reinterpret_cast<WPARAM>( dc.GetSafeHdc() ), 0 );

        // 使图标在工作区矩形中居中
        int cxIcon = GetSystemMetrics( SM_CXICON );
        int cyIcon = GetSystemMetrics( SM_CYICON );
        CRect rect;
        GetClientRect( &rect );
        int x = ( rect.Width() - cxIcon + 1 ) / 2;
        int y = ( rect.Height() - cyIcon + 1 ) / 2;

        // 绘制图标
        dc.DrawIcon( x, y, m_hIcon );
    }
    else
    {
        CDialogEx::OnPaint();
    }
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CKFDeployDlg::OnQueryDragIcon()
{
    return static_cast<HCURSOR>( m_hIcon );
}

BOOL CKFDeployDlg::PreTranslateMessage( MSG* pMsg )
{
    //屏蔽ESC关闭窗体/
    if ( pMsg->message == WM_KEYDOWN )
    {
        if ( pMsg->wParam == VK_ESCAPE ||
                pMsg->wParam == VK_RETURN )
        {
            return TRUE;
        }
    }

    return CDialog::PreTranslateMessage( pMsg );
}

void CKFDeployDlg::OnClose()
{
    // TODO: 在此添加消息处理程序代码和/或调用默认值
    KillTimer( __SERVICE_TIMER_ID__ );
    _kf_deploy_service->ShutDown();
    DestoryDialogData();

    CDialogEx::OnClose();
}

void CKFDeployDlg::OnTimer( UINT_PTR nIDEvent )
{
    // TODO: 在此添加消息处理程序代码和/或调用默认值
    switch ( nIDEvent )
    {
    case __SERVICE_TIMER_ID__:
        _kf_deploy_service->Run();
        break;
    default:
        break;
    }

    CDialogEx::OnTimer( nIDEvent );
}

void CKFDeployDlg::InitDialogData()
{
    // agent list
    {
        auto style = _list_agent.GetExtendedStyle();
        _list_agent.SetExtendedStyle( style | LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT );

        RECT rect;
        _list_agent.GetClientRect( &rect );
        auto width = rect.right / 20.f;

        int index = 0;
        _list_agent.InsertColumn( index++, "", LVCFMT_CENTER, width * 2 );
        _list_agent.InsertColumn( index++, "Agent", LVCFMT_CENTER, width * 2 );
        _list_agent.InsertColumn( index++, "Ip", LVCFMT_CENTER, width * 3 );
        _list_agent.InsertColumn( index++, "端口", LVCFMT_CENTER, width * 1.5 );
        _list_agent.InsertColumn( index++, "状态", LVCFMT_CENTER, width * 1 );
        _list_agent.InsertColumn( index++, "执行命令", LVCFMT_CENTER, width * 10 );
        _list_agent.InsertColumn( index++, "类型", LVCFMT_CENTER, width * 2 );
        _list_agent.InsertColumn( index++, "中控服务器", LVCFMT_CENTER, width * 2 );
        _list_agent.InsertColumn( index++, "中控Ip", LVCFMT_CENTER, width * 3 );
        _list_agent.InsertColumn( index++, "中控端口", LVCFMT_CENTER, width * 2 );
        _list_agent.DeleteColumn( 0 );

    }

    // server list
    {
        auto style = _list_server.GetExtendedStyle();
        _list_server.SetExtendedStyle( style | LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT );

        RECT rect;
        _list_server.GetClientRect( &rect );
        auto width = rect.right / 20.f;

        int index = 0;
        _list_server.InsertColumn( index++, "", LVCFMT_CENTER, width * 2 );
        _list_server.InsertColumn( index++, "Id", LVCFMT_CENTER, width * 1.8 );
        _list_server.InsertColumn( index++, "App", LVCFMT_CENTER, width * 2 );
        _list_server.InsertColumn( index++, "类型", LVCFMT_CENTER, width * 1.8 );
        _list_server.InsertColumn( index++, "日志", LVCFMT_CENTER, width * 1.8 );
        _list_server.InsertColumn( index++, "Agent", LVCFMT_CENTER, width * 1.8 );
        _list_server.InsertColumn( index++, "启动", LVCFMT_CENTER, width * 1.1 );
        _list_server.InsertColumn( index++, "模式", LVCFMT_CENTER, width * 1.1 );
        _list_server.InsertColumn( index++, "状态", LVCFMT_CENTER, width * 1.1 );
        _list_server.InsertColumn( index++, "进程", LVCFMT_CENTER, width * 1.1 );
        _list_server.InsertColumn( index++, "版本号", LVCFMT_CENTER, width * 4 );
        _list_server.InsertColumn( index++, "启动时间", LVCFMT_CENTER, width * 2.5 );
        _list_server.InsertColumn( index++, "路径", LVCFMT_CENTER, width * 2 );
        _list_server.InsertColumn( index++, "参数", LVCFMT_CENTER, width * 5 );
        _list_server.DeleteColumn( 0 );
    }

    _date_time.SetFormat( "yyyy-MM-dd HH:mm:ss" );
    _button_version.ShowWindow( FALSE );
    _button_file.ShowWindow( FALSE );
    _combo_log.ShowWindow( FALSE );

    _combo_log.ResetContent();
    for ( auto i = 0u; i < KFDeploy::_log_count; ++i )
    {
        _combo_log.AddString( KFDeploy::_log_name[ i ] );
    }
    _combo_log.SetCurSel( 0 );
}

void CKFDeployDlg::DestoryDialogData()
{
    if ( _add_agent_dlg != nullptr )
    {
        delete _add_agent_dlg;
    }

    if ( _add_server_dlg != nullptr )
    {
        delete _add_server_dlg;
    }

    if ( _version_dlg != nullptr )
    {
        delete _version_dlg;
    }

    if ( _file_dlg != nullptr )
    {
        delete _file_dlg;
    }
}

void CKFDeployDlg::InitPlugin()
{
    auto kfpluginmanage = KFPluginManage::Instance();

    _kf_message = kfpluginmanage->FindModule< KFMessageInterface >();
    _kf_tcp_client = kfpluginmanage->FindModule< KFTcpClientInterface >();
}

void CKFDeployDlg::SendDeployMessage( uint32 msgid, google::protobuf::Message* message )
{
    auto ok = _kf_tcp_client->SendNetMessage( _deploy_manage->_connect_deploy_id, msgid, message );
    if ( !ok )
    {
        AddDeployLog( 0, "发送消息失败, 请检查网络!" );
    }
}

void CKFDeployDlg::LoadDeployData( const char* file )
{
    _deploy_manage->_deploy_data.Clear();

    KFXml kfxml( file );
    auto root = kfxml.FindNode( "Setting" );
    //////////////////////////////////////////////////////////////////////////
    auto deploysnode = root.FindNode( "Deploys" );
    if ( deploysnode.IsValid() )
    {
        auto deploynode = deploysnode.FindNode( "Deploy" );
        while ( deploynode.IsValid() )
        {
            auto strid = deploynode.GetString( "Id" );
            auto deploydata = _deploy_manage->_deploy_data.Create( strid );

            deploydata->_str_id = strid;

            KFAppId appid( strid );
            deploydata->_id = appid.GetId();

            deploydata->_name = KFConvert::ToAscii( deploynode.GetString( "Name" ) );
            deploydata->_channel = KFConvert::ToAscii( deploynode.GetString( "Channel" ) );
            deploydata->_ip = deploynode.GetString( "Ip" );
            deploydata->_port = deploynode.GetUInt32( "Port" );

            deploynode.NextNode();
        }
    }

    ///////////////////////////////////////////////////////////////////////////
    _list_deploy.ResetContent();

    for ( auto& iter : _deploy_manage->_deploy_data._objects )
    {
        auto deploydata = iter.second;

        auto title = __FORMAT__( "{}|{}", deploydata->_str_id, deploydata->_name );
        _list_deploy.AddString( title.c_str() );
    }
}

void CKFDeployDlg::LoadAppData( const char* file )
{
    _deploy_manage->ClearAppData();

    KFXml kfxml( file );
    auto root = kfxml.FindNode( "Setting" );
    //////////////////////////////////////////////////////////////////////////
    auto appsnode = root.FindNode( "Apps" );
    if ( appsnode.IsValid() )
    {
        auto appnode = appsnode.FindNode( "App" );
        while ( appnode.IsValid() )
        {
            auto name = appnode.GetString( "Name" );
            auto type = appnode.GetString( "Type" );
            auto flag = appnode.GetUInt32( "Flag" );
            _deploy_manage->AddAppData( name, type, flag );

            appnode.NextNode();
        }
    }
}

void CKFDeployDlg::LoadServiceName( const char* file )
{
    _deploy_manage->_service_name.clear();

    KFXml kfxml( file );
    auto root = kfxml.FindNode( "Setting" );
    //////////////////////////////////////////////////////////////////////////
    auto servicesnode = root.FindNode( "Services" );
    if ( servicesnode.IsValid() )
    {
        auto servicenode = servicesnode.FindNode( "Service" );
        while ( servicenode.IsValid() )
        {
            auto name = servicenode.GetString( "Name" );
            auto type = servicenode.GetUInt32( "Type" );
            _deploy_manage->_service_name[ type ] = KFConvert::ToAscii( name );

            servicenode.NextNode();
        }
    }
}

void CKFDeployDlg::LoadCommandData( const char* file )
{
    _deploy_manage->ClearCommandData();

    KFXml kfxml( file );
    auto root = kfxml.FindNode( "Setting" );
    //////////////////////////////////////////////////////////////////////////
    auto commandsnode = root.FindNode( "Commands" );
    if ( commandsnode.IsValid() )
    {
        auto commandnode = commandsnode.FindNode( "Command" );
        while ( commandnode.IsValid() )
        {
            auto data = new CommandData();
            data->_command = commandnode.GetString( "Value" );
            data->_name = KFConvert::ToAscii( commandnode.GetString( "Name" ) );
            data->_text = KFConvert::ToAscii( commandnode.GetString( "Text" ) );
            data->_is_server = commandnode.GetBoolen( "Server" );
            _deploy_manage->_command_data.push_back( data );

            commandnode.NextNode();
        }
    }

    _combo_command.ResetContent();
    for ( auto data : _deploy_manage->_command_data )
    {
        _combo_command.AddString( data->_name.c_str() );
    }
    _combo_command.SetCurSel( 0 );
}

void CKFDeployDlg::OnLbnSelchangeListDeploy()
{
    // TODO: 在此添加控件通知处理程序代码
    auto index = _list_deploy.GetCurSel();
    if ( index == -1 )
    {
        return;
    }

    CString strtitle;
    _list_deploy.GetText( index, strtitle );

    std::string title = strtitle.GetBuffer();
    auto id = KFUtility::SplitString( title, DEFAULT_SPLIT_STRING );

    auto deploydata = _deploy_manage->_deploy_data.Find( id );
    if ( deploydata == nullptr )
    {
        return;
    }

    _edit_deploy_name.SetWindowTextA( deploydata->_name.c_str() );
    _edit_deploy_channel.SetWindowTextA( deploydata->_channel.c_str() );
    _edit_deploy_id.SetWindowTextA( deploydata->_str_id.c_str() );
    _edit_deploy_ip.SetWindowTextA( deploydata->_ip.c_str() );
    _edit_deploy_port.SetWindowTextA( __TO_STRING__( deploydata->_port ).c_str() );

    if ( deploydata->_id != _deploy_manage->_connect_deploy_id )
    {
        _button_connect.EnableWindow( TRUE );
    }
    else
    {
        _button_connect.EnableWindow( FALSE );
    }
}

void CKFDeployDlg::RegisterEventMessage()
{
    __REGISTER_CLIENT_CONNECTION_FUNCTION__( &CKFDeployDlg::OnClientConnectServer );
    __REGISTER_CLIENT_FAILED_FUNCTION__( &CKFDeployDlg::OnClientConnectFailed );
    __REGISTER_CLIENT_LOST_FUNCTION__( &CKFDeployDlg::OnClientLostServer );
    /////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////
    __REGISTER_MESSAGE__( KFMsg::S2S_DEPLOY_LOG_TO_TOOL_ACK, &CKFDeployDlg::HandleDeployLogToToolAck );
    __REGISTER_MESSAGE__( KFMsg::S2S_DEPLOY_TOOL_QUERY_MYSQL_ACK, &CKFDeployDlg::HandleDeployQueryMySQLAck );
    __REGISTER_MESSAGE__( KFMsg::S2S_DEPLOY_TOOL_DELETE_MYSQL_ACK, &CKFDeployDlg::HandleDeployDeleteMySQLAck );
    __REGISTER_MESSAGE__( KFMsg::S2S_DEPLOY_TOOL_EXECUTE_MYSQL_ACK, &CKFDeployDlg::HandleDeployExecuteMySQLAck );
}

void CKFDeployDlg::AddDeployLog( uint64 id, std::string content )
{
    _deploy_manage->AddLogData( id, content );
    _list_log.AddString( content.c_str() );

    __LOG_INFO__( "{}", content );
}

void CKFDeployDlg::ClearDeployLog()
{
    _list_log.ResetContent();
    _deploy_manage->ClearLogData();
}

void CKFDeployDlg::OnBnClickedButtonConnectDeploy()
{
    // TODO: 在此添加控件通知处理程序代码
    auto& strid = GetEditText( _edit_deploy_id );
    auto deploydata = _deploy_manage->_deploy_data.Find( strid );
    if ( deploydata == nullptr )
    {
        return;
    }

    _button_connect.EnableWindow( FALSE );

    // 断开老连接
    if ( _deploy_manage->_connect_deploy_id != _invalid_int )
    {
        _kf_tcp_client->CloseClient( _deploy_manage->_connect_deploy_id, __FUNC_LINE__ );
    }

    // 开始新连接
    _deploy_manage->_connect_deploy_strid = strid;
    _deploy_manage->_connect_deploy_id = deploydata->_id;
    _kf_tcp_client->StartClient( __KF_STRING__( deploy ), __KF_STRING__( server ), deploydata->_id, deploydata->_ip, deploydata->_port );

    ClearDeployLog();
    AddDeployLog( 0, __FORMAT__( "{}:{} connect start!", deploydata->_ip, deploydata->_port ) );
}

__KF_NET_EVENT_FUNCTION__( CKFDeployDlg::OnClientConnectServer )
{
    AddDeployLog( 0, __FORMAT__( "{}:{} connect ok!", netdata->_ip, netdata->_port ) );

    // 查询agent列表
    QueryAgentData();
}

__KF_NET_EVENT_FUNCTION__( CKFDeployDlg::OnClientConnectFailed )
{
    AddDeployLog( 0, __FORMAT__( "{}:{} connect failed!", netdata->_ip, netdata->_port ) );
}

__KF_NET_EVENT_FUNCTION__( CKFDeployDlg::OnClientLostServer )
{
    AddDeployLog( 0, __FORMAT__( "{}:{} disconnect!", netdata->_ip, netdata->_port ) );
}

__KF_MESSAGE_FUNCTION__( CKFDeployDlg::HandleDeployLogToToolAck )
{
    __PROTO_PARSE__( KFMsg::S2SDeployLogToToolAck );

    AddDeployLog( kfmsg.agentid(), kfmsg.content() );
    if ( kfmsg.content().find( "finish" ) != std::string::npos )
    {
        QueryAgentData();
    }
}

void CKFDeployDlg::QueryAgentData()
{
    MapString keys;
    keys[ __KF_STRING__( serverid ) ] = _deploy_manage->_connect_deploy_strid;
    QueryTableValues( __KF_STRING__( agent ), keys );
}

void CKFDeployDlg::QueryTableValues( const std::string& table, MapString& keys )
{
    if ( _deploy_manage->_connect_deploy_id == _invalid_int )
    {
        return AddDeployLog( 0, "请连接中控服务器,再请求查询!" );
    }

    KFMsg::S2SDeployToolQueryMySQLReq req;
    req.set_table( table );

    auto& pbkeys = *req.mutable_keys();
    for ( auto& iter : keys )
    {
        pbkeys[ iter.first ] = iter.second;
    }
    SendDeployMessage( KFMsg::S2S_DEPLOY_TOOL_QUERY_MYSQL_REQ, &req );
}

void CKFDeployDlg::DeleteTableValues( const std::string& table, MapString& keys )
{
    KFMsg::S2SDeployToolDeleteMySQLReq req;
    req.set_table( table );

    auto& pbkeys = *req.mutable_keys();
    for ( auto& iter : keys )
    {
        pbkeys[ iter.first ] = iter.second;
    }
    SendDeployMessage( KFMsg::S2S_DEPLOY_TOOL_DELETE_MYSQL_REQ, &req );
}

void CKFDeployDlg::ExecuteTableValues( const std::string& table, MapString& values )
{
    KFMsg::S2SDeployToolExecuteMySQLReq req;
    req.set_table( table );

    auto& pbvalues = *req.mutable_values();
    for ( auto& iter : values )
    {
        pbvalues[ iter.first ] = iter.second;
    }
    SendDeployMessage( KFMsg::S2S_DEPLOY_TOOL_EXECUTE_MYSQL_REQ, &req );
}

__KF_MESSAGE_FUNCTION__( CKFDeployDlg::HandleDeployQueryMySQLAck )
{
    __PROTO_PARSE__( KFMsg::S2SDeployToolQueryMySQLAck );

    if ( !kfmsg.result() )
    {
        return AddDeployLog( 0, "查询数据库失败!" );
    }

    if ( kfmsg.table() == __KF_STRING__( agent ) )
    {
        RefreshAgentData( &kfmsg.datas() );

        // 查询服务器列表
        QueryServerList();
    }
    else if ( kfmsg.table().find( __KF_STRING__( deploy ) ) != std::string::npos )
    {
        RefreshServerList( &kfmsg.datas() );
    }
    else if ( kfmsg.table() == __KF_STRING__( version ) )
    {
        _version_dlg->RefreshVersionList( &kfmsg.datas() );
    }
    else if ( kfmsg.table() == __KF_STRING__( file ) )
    {
        _file_dlg->RefreshFileList( &kfmsg.datas() );
    }
}

__KF_MESSAGE_FUNCTION__( CKFDeployDlg::HandleDeployDeleteMySQLAck )
{
    __PROTO_PARSE__( KFMsg::S2SDeployToolDeleteMySQLAck );

    if ( !kfmsg.result() )
    {
        return AddDeployLog( 0, "删除数据库失败!" );
    }

    if ( kfmsg.table() == __KF_STRING__( agent ) )
    {
        QueryAgentData();
    }
    else if ( kfmsg.table().find( __KF_STRING__( deploy ) ) != std::string::npos )
    {
        QueryServerList();
    }
}

__KF_MESSAGE_FUNCTION__( CKFDeployDlg::HandleDeployExecuteMySQLAck )
{
    __PROTO_PARSE__( KFMsg::S2SDeployToolExecuteMySQLAck );

    if ( !kfmsg.result() )
    {
        return AddDeployLog( 0, "执行数据库失败!" );
    }

    if ( kfmsg.table() == __KF_STRING__( agent ) )
    {
        QueryAgentData();
        _add_agent_dlg->ShowWindow( SW_HIDE );
    }
    else if ( kfmsg.table().find( __KF_STRING__( deploy ) ) != std::string::npos )
    {
        QueryServerList();
    }
}

void CKFDeployDlg::RefreshAgentData( const KFMsg::PBMySQLDatas* pbdatas )
{
    _list_agent.DeleteAllItems();
    _deploy_manage->ClearAgentData();

    for ( auto i = 0; i < pbdatas->data_size(); ++i )
    {
        auto pbdata = &pbdatas->data( i );
        auto pbvalues = pbdata->values();

        auto service = KFUtility::ToValue< uint32>( pbvalues[ __KF_STRING__( service ) ] );
        _deploy_manage->AddAgentData( pbvalues[ __KF_STRING__( strappid ) ], pbvalues[ __KF_STRING__( localip ) ], service );

        // agengid
        auto column = 0;
        _list_agent.InsertItem( i, pbvalues[ __KF_STRING__( strappid ) ].c_str() );

        // ip
        _list_agent.SetItemText( i, ++column, pbvalues[ __KF_STRING__( localip ) ].c_str() );

        // port
        _list_agent.SetItemText( i, ++column, pbvalues[ __KF_STRING__( port ) ].c_str() );

        // 状态
        auto status = KFUtility::ToValue< uint32 >( pbvalues[ __KF_STRING__( status ) ] );
        _list_agent.SetItemText( i, ++column, KFDeploy::_status_name[ status ] );

        // 执行命令
        _list_agent.SetItemText( i, ++column, pbvalues[ __KF_STRING__( command ) ].c_str() );

        // service
        auto strservice = __FORMAT__( "{}|{}", _deploy_manage->_service_name[ service ], service );
        _list_agent.SetItemText( i, ++column, strservice.c_str() );

        // 中控id
        _list_agent.SetItemText( i, ++column, pbvalues[ __KF_STRING__( serverid ) ].c_str() );

        // 中控ip
        _list_agent.SetItemText( i, ++column, pbvalues[ __KF_STRING__( serverip ) ].c_str() );

        // 中控端口
        _list_agent.SetItemText( i, ++column, pbvalues[ __KF_STRING__( serverport ) ].c_str() );
    }
}

void CKFDeployDlg::OnNMRClickListAgent( NMHDR* pNMHDR, LRESULT* pResult )
{
    LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>( pNMHDR );
    // TODO: 在此添加控件通知处理程序代码
    CMenu popMenu;
    popMenu.LoadMenu( IDR_MENU1 );

    CPoint posMouse;
    GetCursorPos( &posMouse );

    CMenu* pSubMenu = NULL;
    pSubMenu = popMenu.GetSubMenu( 0 );
    pSubMenu->TrackPopupMenu( 0, posMouse.x, posMouse.y, this );

    *pResult = 0;
}


void CKFDeployDlg::OnMenuDeleteAgent()
{
    // TODO: 在此添加命令处理程序代码
    auto index = _list_agent.GetSelectionMark();
    if ( index == -1 )
    {
        return;
    }

    CString strtext = _list_agent.GetItemText( index, 0 );
    if ( strtext.IsEmpty() )
    {
        return;
    }

    if ( MessageBox( "确定要删除这条配置吗?", "请求确认", MB_YESNO ) == IDNO )
    {
        return;
    }

    MapString keys;
    keys[ __KF_STRING__( strappid ) ] = strtext.GetBuffer();
    DeleteTableValues( __KF_STRING__( agent ), keys );
}

void CKFDeployDlg::OnMenuAddAgent()
{
    // TODO: 在此添加命令处理程序代码
    if ( _deploy_manage->_connect_deploy_id == _invalid_int )
    {
        return;
    }

    if ( _add_agent_dlg == nullptr )
    {
        _add_agent_dlg = new CAddAgentDlg();
        _add_agent_dlg->Create( IDD_DIALOG1, this );
    }

    _add_agent_dlg->InitDialog();
    _add_agent_dlg->ShowWindow( SW_SHOW );
}

void CKFDeployDlg::QueryServerList()
{
    _list_server.DeleteAllItems();
    _deploy_manage->ClearServerData();

    std::set< std::string > tables;
    for ( auto& data : _deploy_manage->_agent_data )
    {
        auto table = _deploy_manage->GetTableName( data->_id );
        tables.insert( table );
    }

    for ( auto& table : tables )
    {
        MapString keys;
        keys[ __KF_STRING__( deployid ) ] = _deploy_manage->_connect_deploy_strid;
        QueryTableValues( table, keys );
    }
}

void CKFDeployDlg::RefreshServerList( const KFMsg::PBMySQLDatas* pbdatas )
{
    for ( auto i = 0; i < pbdatas->data_size(); ++i )
    {
        auto pbdata = &pbdatas->data( i );
        auto pbvalues = pbdata->values();

        // 判断是否在列表中
        {
            auto name = pbvalues[ __KF_STRING__( appname ) ];
            auto type = pbvalues[ __KF_STRING__( apptype ) ];
            auto id = pbvalues[ __KF_STRING__( appid ) ];
            auto ok = _deploy_manage->AddServerData( name, type, id );
            if ( !ok )
            {
                continue;
            }
        }

        // agengid
        auto column = 0;
        _list_server.InsertItem( i, pbvalues[ __KF_STRING__( appid ) ].c_str() );

        // app
        auto strapp = __FORMAT__( "{}.{}", pbvalues[ __KF_STRING__( appname ) ], pbvalues[ __KF_STRING__( apptype ) ] );
        _list_server.SetItemText( i, ++column, strapp.c_str() );

        // service
        {
            auto strservice = pbvalues[ __KF_STRING__( service ) ];
            auto nettype = KFUtility::SplitValue< uint32 >( strservice, "." );
            auto service = KFUtility::SplitValue< uint32 >( strservice, "." );
            strservice = __FORMAT__( "{}{}|{}.{}",
                                     KFDeploy::_net_name[ nettype ],
                                     _deploy_manage->_service_name[ service ],
                                     nettype, service );
            _list_server.SetItemText( i, ++column, strservice.c_str() );
        }

        // log
        {
            auto strlog = pbvalues[ __KF_STRING__( logtype ) ];
            auto level = KFUtility::SplitValue< uint32 >( strlog, "." );

            auto log = __FORMAT__( "{}|{}.{}", KFDeploy::_log_name[ level ], level, strlog );
            _list_server.SetItemText( i, ++column, log.c_str() );
        }

        // agentid
        _list_server.SetItemText( i, ++column, pbvalues[ __KF_STRING__( agentid ) ].c_str() );

        // startup
        _list_server.SetItemText( i, ++column, pbvalues[ __KF_STRING__( startup ) ].c_str() );

        // debug
        auto debug = KFUtility::ToValue< uint32 >( pbvalues[ __KF_STRING__( debug ) ] );
        _list_server.SetItemText( i, ++column,  KFDeploy::_mode_name[ debug ] );

        // shutdown
        auto shutdown = KFUtility::ToValue< uint32 >( pbvalues[ __KF_STRING__( shutdown ) ] ) == 1 ? 0 : 1;
        _list_server.SetItemText( i, ++column,  KFDeploy::_status_name[shutdown] );

        // 进程
        _list_server.SetItemText( i, ++column, pbvalues[ __KF_STRING__( process ) ].c_str() );

        // 版本
        _list_server.SetItemText( i, ++column, pbvalues[ __KF_STRING__( version ) ].c_str() );

        // 启动时间
        {
            auto time = KFUtility::ToValue( pbvalues[ __KF_STRING__( time ) ] );
            auto strtime = KFDate::GetTimeString( time );
            _list_server.SetItemText( i, ++column, strtime.c_str() );
        }

        // 路径
        _list_server.SetItemText( i, ++column, pbvalues[ __KF_STRING__( deploypath ) ].c_str() );

        // param
        _list_server.SetItemText( i, ++column, pbvalues[ __KF_STRING__( param ) ].c_str() );
    }
}

void CKFDeployDlg::OnNMRClickListServer( NMHDR* pNMHDR, LRESULT* pResult )
{
    LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast< LPNMITEMACTIVATE >( pNMHDR );
    // TODO: 在此添加控件通知处理程序代码
    CMenu popMenu;
    popMenu.LoadMenu( IDR_MENU1 );

    CPoint posMouse;
    GetCursorPos( &posMouse );

    CMenu* pSubMenu = NULL;
    pSubMenu = popMenu.GetSubMenu( 1 );
    pSubMenu->TrackPopupMenu( 0, posMouse.x, posMouse.y, this );

    *pResult = 0;
}


void CKFDeployDlg::OnMenuQueryServerList()
{
    // TODO: 在此添加命令处理程序代码
    QueryServerList();
}


void CKFDeployDlg::OnMenuQueryAgent()
{
    // TODO: 在此添加命令处理程序代码
    QueryAgentData();
}


void CKFDeployDlg::OnMenuDeleteServerList()
{
    // TODO: 在此添加命令处理程序代码
    auto index = _list_server.GetSelectionMark();
    if ( index == -1 )
    {
        return;
    }

    CString strtext = _list_server.GetItemText( index, 0 );
    if ( strtext.IsEmpty() )
    {
        return;
    }

    std::string appid = strtext.GetBuffer();
    if ( MessageBox( "确定要删除这条服务器配置吗?", "请求确认", MB_YESNO ) == IDNO )
    {
        return;
    }

    strtext = _list_server.GetItemText( index, 5 );
    auto stragentid = strtext.GetBuffer();
    auto table = _deploy_manage->GetTableName( stragentid );

    MapString keys;
    keys[ __KF_STRING__( appid ) ] = appid;
    DeleteTableValues( table, keys );
}


void CKFDeployDlg::OnMenuAddServer()
{
    // TODO: 在此添加命令处理程序代码
    if ( _deploy_manage->_connect_deploy_id == _invalid_int )
    {
        return;
    }

    if ( _add_server_dlg == nullptr )
    {
        _add_server_dlg = new CAddServerDlg();
        _add_server_dlg->Create( IDD_DIALOG2, this );
    }

    _add_server_dlg->InitDialog();
    _add_server_dlg->ShowWindow( SW_SHOW );
}

void CKFDeployDlg::OnCbnSelchangeComboCommand()
{
    // TODO: 在此添加控件通知处理程序代码
    CString strtext;
    _combo_command.GetWindowTextA( strtext );
    auto commanddata = _deploy_manage->FindCommandData( strtext.GetBuffer() );
    if ( commanddata == nullptr )
    {
        return;
    }

    if ( commanddata->_text.empty() )
    {
        _edit_param.EnableWindow( FALSE );
    }
    else
    {
        _edit_param.EnableWindow( TRUE );
    }

    _button_version.ShowWindow( FALSE );
    _button_file.ShowWindow( FALSE );
    _combo_log.ShowWindow( FALSE );
    if ( commanddata->_command == __KF_STRING__( version ) )
    {
        _button_version.ShowWindow( TRUE );
    }
    else if ( commanddata->_command == __KF_STRING__( reloadconfig ) ||
              commanddata->_command == __KF_STRING__( reloadscript ) )
    {
        _button_file.ShowWindow( TRUE );
    }
    else if ( commanddata->_command == __KF_STRING__( loglevel ) )
    {
        _combo_log.ShowWindow( TRUE );
    }
    else if ( commanddata->_command == __KF_STRING__( logmemory ) )
    {
        _edit_param.SetWindowTextA( "1" );
    }
    else if ( commanddata->_command == __KF_STRING__( unschedule ) )
    {
        _edit_param.SetWindowTextA( "0" );
    }

    if ( commanddata->_is_server )
    {
        _combo_name.EnableWindow( TRUE );
        _combo_type.EnableWindow( TRUE );
        _combo_zone.EnableWindow( TRUE );
        _combo_id.EnableWindow( TRUE );

        // 初始化所有服务器类型
        std::set< std::string > namelist;
        for ( auto data : _deploy_manage->_server_data )
        {
            namelist.insert( data->_name );
        }

        _combo_name.ResetContent();
        _combo_name.AddString( "*" );
        for ( auto& name : namelist )
        {
            _combo_name.AddString( name.c_str() );
        }
        _combo_name.SetCurSel( 0 );

        _combo_type.ResetContent();
        _combo_type.AddString( "*" );
        _combo_type.SetCurSel( 0 );

        _combo_zone.ResetContent();
        _combo_zone.AddString( "0" );
        _combo_zone.SetCurSel( 0 );

        _combo_id.ResetContent();
        _combo_id.AddString( "*" );
        _combo_id.SetCurSel( 0 );
    }
    else
    {
        _combo_name.EnableWindow( FALSE );
        _combo_type.EnableWindow( FALSE );
        _combo_zone.EnableWindow( FALSE );
        _combo_id.EnableWindow( FALSE );
    }

}

void CKFDeployDlg::OnCbnSelchangeComboLog()
{
    // TODO: 在此添加控件通知处理程序代码
    auto index = _combo_log.GetCurSel();
    _edit_param.SetWindowTextA( __TO_STRING__( index ).c_str() );
}

void CKFDeployDlg::OnCbnSelchangeComboName()
{
    // TODO: 在此添加控件通知处理程序代码
    CString strtext;
    _combo_name.GetWindowTextA( strtext );
    std::string name = strtext.GetBuffer();

    std::set< std::string > typelist;
    std::set< uint32 > zonelist;
    zonelist.insert( 0 );
    std::set< std::string > idlist;

    for ( auto data : _deploy_manage->_server_data )
    {
        if ( data->_name == name || name == _globbing_str )
        {
            typelist.insert( data->_type );
            zonelist.insert( data->_zone_id );
            idlist.insert( data->_id );
        }
    }

    _combo_type.ResetContent();
    _combo_type.AddString( "*" );
    for ( auto& type : typelist )
    {
        _combo_type.AddString( type.c_str() );
    }
    _combo_type.SetCurSel( 0 );

    // 小区
    _combo_zone.ResetContent();
    for ( auto zoneid : zonelist )
    {
        _combo_zone.AddString( __TO_STRING__( zoneid ).c_str() );
    }
    _combo_zone.SetCurSel( 0 );

    if ( name == __KF_STRING__( zone ) )
    {
        _combo_zone.EnableWindow( TRUE );
    }
    else
    {
        _combo_zone.EnableWindow( FALSE );
    }

    _combo_id.ResetContent();
    _combo_id.AddString( "*" );
    for ( auto& id : idlist )
    {
        _combo_id.AddString( id.c_str() );
    }
    _combo_id.SetCurSel( 0 );
}


void CKFDeployDlg::OnCbnSelchangeComboType()
{
    // TODO: 在此添加控件通知处理程序代码

    CString strtext;
    _combo_name.GetWindowTextA( strtext );
    std::string name = strtext.GetBuffer();

    _combo_type.GetWindowTextA( strtext );
    std::string type = strtext.GetBuffer();

    std::set< std::string > idlist;
    for ( auto data : _deploy_manage->_server_data )
    {
        if ( ( data->_name == name || name == _globbing_str ) &&
                ( data->_type == type || type == _globbing_str ) )
        {
            idlist.insert( data->_id );
        }
    }

    _combo_id.ResetContent();
    _combo_id.AddString( "*" );
    for ( auto& id : idlist )
    {
        _combo_id.AddString( id.c_str() );
    }
    _combo_id.SetCurSel( 0 );
}


void CKFDeployDlg::OnCbnSelchangeComboZone()
{
    // TODO: 在此添加控件通知处理程序代码
    CString strtext;
    _combo_name.GetWindowTextA( strtext );
    std::string name = strtext.GetBuffer();

    _combo_type.GetWindowTextA( strtext );
    std::string type = strtext.GetBuffer();

    _combo_zone.GetWindowTextA( strtext );
    auto zoneid = KFUtility::ToValue< uint32 >( strtext.GetBuffer() );

    std::set< std::string > idlist;
    for ( auto data : _deploy_manage->_server_data )
    {
        if ( ( data->_name == name || name == _globbing_str ) &&
                ( data->_type == type || type == _globbing_str ) &&
                ( zoneid == data->_zone_id || zoneid == _invalid_int ) )
        {
            idlist.insert( data->_id );
        }
    }

    _combo_id.ResetContent();
    _combo_id.AddString( "*" );
    for ( auto& id : idlist )
    {
        _combo_id.AddString( id.c_str() );
    }
    _combo_id.SetCurSel( 0 );
}


void CKFDeployDlg::OnBnClickedButtonQueryVersion()
{
    // TODO: 在此添加控件通知处理程序代码
    if ( _version_dlg == nullptr )
    {
        _version_dlg = new CVersionDlg();
        _version_dlg->Create( IDD_DIALOG3, this );
        _version_dlg->InitDialogData();
    }

    _version_dlg->ShowWindow( SW_SHOW );

    // 查询
    MapString keys;
    QueryTableValues( __KF_STRING__( version ), keys );
}


void CKFDeployDlg::OnBnClickedButtonQueryFile()
{
    // TODO: 在此添加控件通知处理程序代码
    if ( _file_dlg == nullptr )
    {
        _file_dlg = new CFileDlg();
        _file_dlg->Create( IDD_DIALOG4, this );
        _file_dlg->InitDialogData();
    }

    _file_dlg->ShowWindow( SW_SHOW );

    // 查询
    MapString keys;
    QueryTableValues( __KF_STRING__( file ), keys );
}


void CKFDeployDlg::OnBnClickedButtonAddCommand()
{
    // TODO: 在此添加控件通知处理程序代码
    CString strtext;
    _combo_command.GetWindowTextA( strtext );
    auto commanddata = _deploy_manage->FindCommandData( strtext.GetBuffer() );
    if ( commanddata == nullptr )
    {
        AfxMessageBox( "请选择正确的命令!" );
        return;
    }

    auto param = GetEditText( _edit_param );
    if ( param.empty() && !commanddata->_text.empty() )
    {
        AfxMessageBox( commanddata->_text.c_str() );
        return;
    }

    _combo_name.GetWindowTextA( strtext );
    std::string name = strtext.GetBuffer();

    _combo_type.GetWindowTextA( strtext );
    std::string type = strtext.GetBuffer();

    _combo_zone.GetWindowTextA( strtext );
    auto zoneid = KFUtility::ToValue< uint32 >( strtext.GetBuffer() );

    _combo_id.GetWindowTextA( strtext );
    std::string appid = strtext.GetBuffer();

    _date_time.GetWindowTextA( strtext );
    auto strtime = strtext.GetBuffer();
    auto scheduletime = KFDate::FromString( strtime );

    std::string message = "";
    if ( scheduletime <= KFGlobal::Instance()->_real_time )
    {
        message = __FORMAT__( "command:{}\nparam:{}\nappname:{}\napptype:{}\nzoneid:{}\nappid:{}\n确认需要发送命令吗?",
                              commanddata->_command, param, name, type, zoneid, appid );
    }
    else
    {
        message = __FORMAT__( "command:{}\nparam:{}\nappname:{}\napptype:{}\nzoneid:{}\nappid:{}\n计划时间:{}\n确认需要发送命令吗?",
                              commanddata->_command, param, name, type, zoneid, appid, strtime );
    }

    if ( MessageBox( message.c_str(), "请求确认", MB_YESNO ) == IDNO )
    {
        return;
    }

    KFMsg::S2SDeployToolCommandReq req;
    auto pbcommand = req.mutable_deploycommand();
    pbcommand->set_command( commanddata->_command );
    pbcommand->set_value( param );
    pbcommand->set_appname( name );
    pbcommand->set_apptype( type );
    pbcommand->set_zoneid( zoneid );
    pbcommand->set_appid( appid );
    pbcommand->set_toolid( KFGlobal::Instance()->_app_id->GetId() );

    req.set_time( 0 );
    SendDeployMessage( KFMsg::S2S_DEPLOY_TOOL_COMMAND_REQ, &req );

    ClearDeployLog();
}
