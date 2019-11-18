// CAddServerDlg.cpp: 实现文件
//

#include "stdafx.h"
#include "KFDeploy.h"
#include "CAddServerDlg.h"
#include "afxdialogex.h"
#include "KFDeployDlg.h"


// CAddServerDlg 对话框

IMPLEMENT_DYNAMIC( CAddServerDlg, CDialogEx )

CAddServerDlg::CAddServerDlg( CWnd* pParent /*=nullptr*/ )
    : CDialogEx( IDD_DIALOG2, pParent )
{

}

CAddServerDlg::~CAddServerDlg()
{
}

void CAddServerDlg::DoDataExchange( CDataExchange* pDX )
{
    CDialogEx::DoDataExchange( pDX );
    DDX_Control( pDX, IDC_COMBO1, _combo_name );
    DDX_Control( pDX, IDC_COMBO3, _combo_type );
    DDX_Control( pDX, IDC_COMBO4, _combo_net );
    DDX_Control( pDX, IDC_COMBO5, _combo_mode );
    DDX_Control( pDX, IDC_COMBO7, _combo_agent );
    DDX_Control( pDX, IDC_COMBO9, _combo_startup );
    DDX_Control( pDX, IDC_COMBO6, _combo_log );
    DDX_Control( pDX, IDC_COMBO8, _combo_service );
    DDX_Control( pDX, IDC_EDIT1, _edit_zone );
    DDX_Control( pDX, IDC_EDIT2, _edit_id );
    DDX_Control( pDX, IDC_EDIT3, _edit_param );
    DDX_Control( pDX, IDC_EDIT4, _edit_path );
}


BEGIN_MESSAGE_MAP( CAddServerDlg, CDialogEx )
    ON_BN_CLICKED( IDC_BUTTON1, &CAddServerDlg::OnBnClickedButtonAddServer )
    ON_CBN_SELCHANGE( IDC_COMBO1, &CAddServerDlg::OnCbnSelchangeComboName )
    ON_CBN_SELCHANGE( IDC_COMBO3, &CAddServerDlg::OnCbnSelchangeComboType )
END_MESSAGE_MAP()


// CAddServerDlg 消息处理程序
BOOL CAddServerDlg::PreTranslateMessage( MSG* pMsg )
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

void CAddServerDlg::InitDialog()
{
    _edit_path.SetWindowTextA( "/data" );
    _edit_id.SetWindowTextA( "0" );
    _edit_zone.SetWindowTextA( "0" );
    _edit_param.SetWindowTextA( "" );
    ////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////
    _combo_name.ResetContent();
    for ( auto data : _deploy_manage->_app_data )
    {
        _combo_name.AddString( data->_name.c_str() );
    }
    _combo_name.SetCurSel( 0 );

    _combo_type.ResetContent();

    _combo_net.ResetContent();
    for ( auto i = 0u; i < KFDeploy::_net_count; ++i )
    {
        _combo_net.AddString( KFDeploy::_net_name[ i ] );
    }
    _combo_net.SetCurSel( 1 );

    _combo_service.ResetContent();
    for ( auto& iter : _deploy_manage->_service_name )
    {
        _combo_service.AddString( iter.second.c_str() );
    }
    _combo_service.SetCurSel( 0 );

    _combo_agent.ResetContent();
    for ( auto data : _deploy_manage->_agent_data )
    {
        _combo_agent.AddString( data->_id.c_str() );
    }
    _combo_agent.SetCurSel( 0 );

    _combo_startup.ResetContent();
    for ( auto i = 0u; i < KFDeploy::_status_count; ++i )
    {
        _combo_startup.AddString( KFDeploy::_status_name[i] );
    }
    _combo_startup.SetCurSel( 1 );

    _combo_log.ResetContent();
    for ( auto i = 0u; i < KFDeploy::_log_count; ++i )
    {
        _combo_log.AddString( KFDeploy::_log_name[ i ] );
    }
    _combo_log.SetCurSel( 0 );

    _combo_mode.ResetContent();
    for ( auto i = 0u; i < KFDeploy::_mode_count; ++i )
    {
        _combo_mode.AddString( KFDeploy::_mode_name[ i ] );
    }
    _combo_mode.SetCurSel( 0 );
}

void CAddServerDlg::OnBnClickedButtonAddServer()
{
    // TODO: 在此添加控件通知处理程序代码
    MapString values;

    CString strtext;
    _combo_name.GetWindowTextA( strtext );
    std::string name = strtext.GetBuffer();

    _combo_type.GetWindowTextA( strtext );
    std::string type = strtext.GetBuffer();
    if ( name.empty() || type.empty() )
    {
        AfxMessageBox( "请选择名字和类型!" );
        return;
    }

    auto zoneid = KFUtility::ToValue< uint32 >( GetEditText( _edit_zone ) );
    if ( name == __STRING__( zone ) )
    {
        if ( zoneid == 0 )
        {
            AfxMessageBox( "请输入正确的小区id!" );
            return;
        }
    }

    auto workid = KFUtility::ToValue< uint32 >( GetEditText( _edit_id ) );
    if ( workid == 0 )
    {
        AfxMessageBox( "请输入服务器id!" );
        return;
    }

    auto maxworkid = _deploy_manage->FindMaxServerId( name, type );
    if ( workid < maxworkid )
    {
        if ( MessageBox( "输入的服务器id可能已经存在, 确定需要添加吗?", "请求确认", MB_YESNO ) == IDNO )
        {
            return;
        }
    }

    auto apptype = 0;
    auto appdata = _deploy_manage->FindAppData( name );
    if ( appdata != nullptr )
    {
        auto iter = appdata->_types.find( type );
        if ( iter != appdata->_types.end() )
        {
            apptype = iter->second;
        }
    }

    KFAppId appid( _deploy_manage->_connect_deploy_id );
    appid.SetType( apptype );
    appid.SetZoneId( zoneid );
    appid.SetWorkId( workid );

    values[ __STRING__( appname ) ] = name;
    values[ __STRING__( apptype ) ] = type;
    values[ __STRING__( appid ) ] = appid.ToString();

    auto net = _combo_net.GetCurSel();
    if ( net == 0 )
    {
        net = KFServerEnum::Local;
    }
    values[ __STRING__( net ) ] = __TO_STRING__( net );

    uint32 service = KFServerEnum::Debug;
    _combo_service.GetWindowTextA( strtext );
    std::string strservice = strtext.GetBuffer();
    for ( auto& iter : _deploy_manage->_service_name )
    {
        if ( iter.second == strservice )
        {
            service = iter.first;
        }
    }
    values[ __STRING__( service ) ] = __FORMAT__( "{}.{}", _deploy_manage->_channel, service );

    auto startup = _combo_startup.GetCurSel();
    values[ __STRING__( startup ) ] = __TO_STRING__( startup );

    values[ __STRING__( param ) ] = GetEditText( _edit_param );

    auto path = GetEditText( _edit_path );
    if ( path.empty() )
    {
        AfxMessageBox( "请输入服务器路径!" );
        return;
    }
    values[ __STRING__( deploypath ) ] = path;

    auto log = _combo_log.GetCurSel();
    values[ __STRING__( logtype ) ] = __FORMAT__( "{}.{}", log, 0 );

    auto mode = _combo_mode.GetCurSel();
    values[ __STRING__( debug ) ] = __TO_STRING__( mode );

    _combo_agent.GetWindowTextA( strtext );
    std::string stragent = strtext.GetBuffer();
    if ( stragent.empty() )
    {
        AfxMessageBox( "请选择agent!" );
        return;
    }
    values[ __STRING__( agentid ) ] = stragent;

    values[ __STRING__( deployid ) ] = _deploy_manage->_connect_deploy_strid;

    auto table = _deploy_manage->GetTableName( stragent );
    _deploy_manage->_dlg_deploy->ExecuteTableValues( table, values );
}


void CAddServerDlg::OnCbnSelchangeComboName()
{
    // TODO: 在此添加控件通知处理程序代码
    CString strtext;
    _combo_name.GetWindowTextA( strtext );

    std::string name = strtext.GetBuffer();
    auto appdata = _deploy_manage->FindAppData( name );
    if ( appdata == nullptr )
    {
        return;
    }

    _combo_type.ResetContent();
    for ( auto& iter : appdata->_types )
    {
        _combo_type.AddString( iter.first.c_str() );
    }
    _combo_type.SetCurSel( 0 );

    OnCbnSelchangeComboType();

    if ( name != __STRING__( zone ) )
    {
        _edit_zone.SetWindowTextA( "0" );
    }
}

void CAddServerDlg::OnCbnSelchangeComboType()
{
    // TODO: 在此添加控件通知处理程序代码
    CString strtext;
    _combo_name.GetWindowTextA( strtext );
    std::string name = strtext.GetBuffer();

    _combo_type.GetWindowTextA( strtext );
    std::string type = strtext.GetBuffer();

    auto maxid = _deploy_manage->FindMaxServerId( name, type );
    _edit_id.SetWindowTextA( __TO_STRING__( maxid ).c_str() );
}
