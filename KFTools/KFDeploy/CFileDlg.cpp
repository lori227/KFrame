// CFileDlg.cpp: 实现文件
//

#include "stdafx.h"
#include "KFDeploy.h"
#include "CFileDlg.h"
#include "afxdialogex.h"
#include "KFDeployDlg.h"

// CFileDlg 对话框

IMPLEMENT_DYNAMIC( CFileDlg, CDialogEx )

CFileDlg::CFileDlg( CWnd* pParent /*=nullptr*/ )
    : CDialogEx( IDD_DIALOG4, pParent )
{

}

CFileDlg::~CFileDlg()
{
}

void CFileDlg::DoDataExchange( CDataExchange* pDX )
{
    CDialogEx::DoDataExchange( pDX );
    DDX_Control( pDX, IDC_LIST1, _list_file );
}


BEGIN_MESSAGE_MAP( CFileDlg, CDialogEx )
    ON_NOTIFY( NM_DBLCLK, IDC_LIST1, &CFileDlg::OnNMDblclkListFile )
END_MESSAGE_MAP()


// CFileDlg 消息处理程序
BOOL CFileDlg::PreTranslateMessage( MSG* pMsg )
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

void CFileDlg::InitDialogData()
{
    {
        auto style = _list_file.GetExtendedStyle();
        _list_file.SetExtendedStyle( style | LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT );

        RECT rect;
        _list_file.GetClientRect( &rect );
        auto width = rect.right / 20.f;

        int index = 0;
        _list_file.InsertColumn( index++, "", LVCFMT_CENTER, width * 2 );
        _list_file.InsertColumn( index++, "文件名", LVCFMT_CENTER, width * 5 );
        _list_file.InsertColumn( index++, "路径", LVCFMT_CENTER, width * 5 );
        _list_file.InsertColumn( index++, "上传时间", LVCFMT_CENTER, width * 5 );
        _list_file.InsertColumn( index++, "MD5", LVCFMT_CENTER, width * 4 );
        _list_file.DeleteColumn( 0 );
    }
}

class FileData
{
public:
    std::string _time;
    std::string _path;
    std::string _file;
    std::string _md5;
};

void CFileDlg::RefreshFileList( const KFMsg::PBMySQLDatas* pbdatas )
{
    _list_file.DeleteAllItems();

    std::map< std::string, FileData > filelist;
    for ( auto i = 0; i < pbdatas->data_size(); ++i )
    {
        if ( i >= 50 )
        {
            break;
        }

        auto pbdata = &pbdatas->data( i );
        auto pbvalues = pbdata->values();

        FileData data;
        data._time = pbvalues[ "file_time" ];
        data._file = pbvalues[ "file_name" ];
        data._path = pbvalues[ "file_path" ];
        data._md5 = pbvalues[ "file_md5" ];
        filelist[ data._time ] = data;
    }

    auto i = 0;
    for ( auto iter = filelist.rbegin(); iter != filelist.rend(); ++iter, ++i )
    {
        auto data = &iter->second;

        // name
        auto column = 0;
        _list_file.InsertItem( i, data->_file.c_str() );

        // path
        _list_file.SetItemText( i, ++column, data->_path.c_str() );

        // time
        _list_file.SetItemText( i, ++column, data->_time.c_str() );

        // md5
        _list_file.SetItemText( i, ++column, data->_md5.c_str() );
    }
}

void CFileDlg::OnNMDblclkListFile( NMHDR* pNMHDR, LRESULT* pResult )
{
    LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>( pNMHDR );
    // TODO: 在此添加控件通知处理程序代码
    *pResult = 0;

    auto index = pNMLV->iItem;
    if ( index == -1 )
    {
        return;
    }

    CString strtext = _list_file.GetItemText( index, 0 );
    if ( strtext.IsEmpty() )
    {
        return;
    }

    _deploy_manage->_dlg_deploy->_edit_param.SetWindowTextA( strtext );
    ShowWindow( SW_HIDE );
}
