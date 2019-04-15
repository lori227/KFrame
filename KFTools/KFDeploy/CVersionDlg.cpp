// CVersionDlg.cpp: 实现文件
//

#include "stdafx.h"
#include "KFDeploy.h"
#include "CVersionDlg.h"
#include "afxdialogex.h"
#include "KFDeployDlg.h"


// CVersionDlg 对话框

IMPLEMENT_DYNAMIC( CVersionDlg, CDialogEx )

CVersionDlg::CVersionDlg( CWnd* pParent /*=nullptr*/ )
    : CDialogEx( IDD_DIALOG3, pParent )
{

}

CVersionDlg::~CVersionDlg()
{
}

void CVersionDlg::DoDataExchange( CDataExchange* pDX )
{
    CDialogEx::DoDataExchange( pDX );
    DDX_Control( pDX, IDC_LIST1, _list_version );
}


BEGIN_MESSAGE_MAP( CVersionDlg, CDialogEx )
    ON_NOTIFY( NM_DBLCLK, IDC_LIST1, &CVersionDlg::OnNMDblclkListVersion )
END_MESSAGE_MAP()


// CVersionDlg 消息处理程序
BOOL CVersionDlg::PreTranslateMessage( MSG* pMsg )
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

void CVersionDlg::InitDialogData()
{
    {
        auto style = _list_version.GetExtendedStyle();
        _list_version.SetExtendedStyle( style | LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT );

        RECT rect;
        _list_version.GetClientRect( &rect );
        auto width = rect.right / 20.f;

        int index = 0;
        _list_version.InsertColumn( index++, "", LVCFMT_CENTER, width * 2 );
        _list_version.InsertColumn( index++, "版本文件名", LVCFMT_CENTER, width * 8 );
        _list_version.InsertColumn( index++, "上传时间", LVCFMT_CENTER, width * 6 );
        _list_version.InsertColumn( index++, "MD5", LVCFMT_CENTER, width * 5 );
        _list_version.DeleteColumn( 0 );
    }
}

class VersionData
{
public:
    std::string _time;
    std::string _file;
    std::string _md5;
};

void CVersionDlg::RefreshVersionList( const KFMsg::PBMySQLDatas* pbdatas )
{
    _list_version.DeleteAllItems();

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
        data._time = pbvalues[ "version_time" ];
        data._file = pbvalues[ "version_name" ];
        data._md5 = pbvalues[ "version_md5" ];
        versionlist[ data._time ] = data;
    }

    auto i = 0;
    for ( auto iter = versionlist.rbegin(); iter != versionlist.rend(); ++iter, ++i )
    {
        auto data = &iter->second;

        // time
        auto column = 0;
        _list_version.InsertItem( i, data->_file.c_str() );

        // name
        _list_version.SetItemText( i, ++column, data->_time.c_str() );

        // md5
        _list_version.SetItemText( i, ++column, data->_md5.c_str() );
    }
}

void CVersionDlg::OnNMDblclkListVersion( NMHDR* pNMHDR, LRESULT* pResult )
{
    LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>( pNMHDR );
    // TODO: 在此添加控件通知处理程序代码
    *pResult = 0;

    auto index = pNMItemActivate->iItem;
    if ( index == -1 )
    {
        return;
    }

    CString strtext = _list_version.GetItemText( index, 0 );
    if ( strtext.IsEmpty() )
    {
        return;
    }

    _deploy_manage->_dlg_deploy->_edit_param.SetWindowTextA( strtext );
    ShowWindow( SW_HIDE );
}
