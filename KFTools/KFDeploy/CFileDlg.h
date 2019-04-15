#pragma once

#include "KFProtocol/KFProtocol.h"

// CFileDlg 对话框

class CFileDlg : public CDialogEx
{
    DECLARE_DYNAMIC( CFileDlg )

public:
    CFileDlg( CWnd* pParent = nullptr ); // 标准构造函数
    virtual ~CFileDlg();

    // 对话框数据
#ifdef AFX_DESIGN_TIME
    enum { IDD = IDD_DIALOG4 };
#endif
public:
    // 初始化界面
    void InitDialogData();

    void RefreshFileList( const KFMsg::PBMySQLDatas* pbdatas );

protected:
    virtual void DoDataExchange( CDataExchange* pDX );  // DDX/DDV 支持

    DECLARE_MESSAGE_MAP()
public:
    virtual BOOL PreTranslateMessage( MSG* pMsg );
    CListCtrl _list_file;
    afx_msg void OnNMDblclkListFile( NMHDR* pNMHDR, LRESULT* pResult );
};
