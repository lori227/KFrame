// CResourceDlg.cpp: 实现文件
//

#include "stdafx.h"
#include "KFDeploy.h"
#include "CResourceDlg.h"
#include "afxdialogex.h"
#include "KFDeployDlg.h"

// CResourceDlg 对话框

IMPLEMENT_DYNAMIC( CResourceDlg, CDialogEx )

CResourceDlg::CResourceDlg( CWnd* pParent /*=nullptr*/ )
    : CDialogEx( IDD_DIALOG5, pParent )
{

}

CResourceDlg::~CResourceDlg()
{
}

void CResourceDlg::DoDataExchange( CDataExchange* pDX )
{
    CDialogEx::DoDataExchange( pDX );
    DDX_Control( pDX, IDC_LIST1, _list_resource );
}


BEGIN_MESSAGE_MAP( CResourceDlg, CDialogEx )
    ON_NOTIFY( NM_DBLCLK, IDC_LIST1, &CResourceDlg::OnNMDblclkListResource )
END_MESSAGE_MAP()


// CResourceDlg 消息处理程序
BOOL CResourceDlg::PreTranslateMessage( MSG* pMsg )
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

void CResourceDlg::InitDialogData()
{
    {
        auto style = _list_resource.GetExtendedStyle();
        _list_resource.SetExtendedStyle( style | LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT );

        RECT rect;
        _list_resource.GetClientRect( &rect );
        auto width = rect.right / 20.f;

        int index = 0;
        _list_resource.InsertColumn( index++, "", LVCFMT_CENTER, width * 2 );
        _list_resource.InsertColumn( index++, "资源文件名", LVCFMT_CENTER, width * 8 );
        _list_resource.InsertColumn( index++, "上传时间", LVCFMT_CENTER, width * 6 );
        _list_resource.InsertColumn( index++, "MD5", LVCFMT_CENTER, width * 5 );
        _list_resource.DeleteColumn( 0 );
    }
}

class VersionData
{
public:
    std::string _time;
    std::string _file;
    std::string _md5;
};

void CResourceDlg::RefreshResourceList( const KFMsg::PBMySQLDatas* pbdatas )
{
    _list_resource.DeleteAllItems();

    std::map< std::string, VersionData > versionlist;
    for ( auto i = 0; i < pbdatas->data_size(); ++i )
    {
        if ( i >= 50 )
        {
            break;
        }

        auto pbdata = &pbdatas->data( i );
        auto pbvalues = pbdata->values();

        VersionData data;
        data._time = pbvalues[ "resource_time" ];
        data._file = pbvalues[ "resource_name" ];
        data._md5 = pbvalues[ "resource_md5" ];
        versionlist[ data._time ] = data;
    }

    auto i = 0;
    for ( auto iter = versionlist.rbegin(); iter != versionlist.rend(); ++iter, ++i )
    {
        auto data = &iter->second;

        // time
        auto column = 0;
        _list_resource.InsertItem( i, data->_file.c_str() );

        // name
        _list_resource.SetItemText( i, ++column, data->_time.c_str() );

        // md5
        _list_resource.SetItemText( i, ++column, data->_md5.c_str() );
    }
}

void CResourceDlg::OnNMDblclkListResource( NMHDR* pNMHDR, LRESULT* pResult )
{
    LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast< LPNMITEMACTIVATE >( pNMHDR );
    // TODO: 在此添加控件通知处理程序代码
    *pResult = 0;

    auto index = pNMItemActivate->iItem;
    if ( index == -1 )
    {
        return;
    }

    CString strtext = _list_resource.GetItemText( index, 0 );
    if ( strtext.IsEmpty() )
    {
        return;
    }

    _deploy_manage->_dlg_deploy->_edit_param.SetWindowTextA( strtext );
    ShowWindow( SW_HIDE );
}
