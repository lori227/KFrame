// CAddAgentDlg.cpp: 实现文件
//

#include "stdafx.h"
#include "KFDeploy.h"
#include "CAddAgentDlg.h"
#include "afxdialogex.h"
#include "KFDeployDlg.h"


// CAddAgentDlg 对话框

IMPLEMENT_DYNAMIC( CAddAgentDlg, CDialogEx )

CAddAgentDlg::CAddAgentDlg( CWnd* pParent /*=nullptr*/ )
    : CDialogEx( IDD_DIALOG1, pParent )
{

}

CAddAgentDlg::~CAddAgentDlg()
{
}

void CAddAgentDlg::DoDataExchange( CDataExchange* pDX )
{
    CDialogEx::DoDataExchange( pDX );
    DDX_Control( pDX, IDC_EDIT1, _edit_agent_id );
    DDX_Control( pDX, IDC_IPADDRESS1, _ip_control );
}


BEGIN_MESSAGE_MAP( CAddAgentDlg, CDialogEx )
    ON_BN_CLICKED( IDC_BUTTON2, &CAddAgentDlg::OnBnClickedButtonAddAgent )
END_MESSAGE_MAP()


// CAddAgentDlg 消息处理程序
BOOL CAddAgentDlg::PreTranslateMessage( MSG* pMsg )
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


void CAddAgentDlg::InitDialog()
{
    _ip_control.ClearAddress();
    _edit_agent_id.SetWindowTextA( "" );
}

void CAddAgentDlg::OnBnClickedButtonAddAgent()
{
    // TODO: 在此添加控件通知处理程序代码
    auto agentid = GetEditText( _edit_agent_id );
    if ( agentid.empty() )
    {
        return;
    }

    BYTE nf1, nf2, nf3, nf4;
    _ip_control.GetAddress( nf1, nf2, nf3, nf4 );
    auto agentip = __FORMAT__( "{}.{}.{}.{}", nf1, nf2, nf3, nf4 );
    if ( agentip.empty() )
    {
        return;
    }

    auto deploydata = _deploy_manage->_deploy_data.Find( KFAppId::ToString( _deploy_manage->_connect_deploy_id ) );
    if ( deploydata == nullptr )
    {
        return;
    }

    KFAppId appid( _deploy_manage->_connect_deploy_id );
    appid.SetType( KFServerEnum::DeployAgent );
    appid.SetWorkId( __TO_UINT32__( agentid ) );

    StringMap values;
    values[ __STRING__( strappid ) ] = appid.ToString();
    values[ __STRING__( localip ) ] = agentip;
    values[ __STRING__( interanetip ) ] = agentip;
    values[ __STRING__( serverid ) ] = deploydata->_str_id;
    values[ __STRING__( serverip ) ] = deploydata->_ip;
    values[ __STRING__( serverport ) ] = __TO_STRING__( deploydata->_port );
    _deploy_manage->_dlg_deploy->ExecuteTableValues( __STRING__( agent ), values );
}

