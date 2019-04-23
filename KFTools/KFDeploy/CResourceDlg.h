#pragma once

#include "KFProtocol/KFProtocol.h"

// CResourceDlg 对话框

class CResourceDlg : public CDialogEx
{
    DECLARE_DYNAMIC( CResourceDlg )

public:
    CResourceDlg( CWnd* pParent = nullptr ); // 标准构造函数
    virtual ~CResourceDlg();

    // 对话框数据
#ifdef AFX_DESIGN_TIME
    enum { IDD = IDD_DIALOG5 };
#endif

public:
    // 初始化界面
    void InitDialogData();

    void RefreshResourceList( const KFMsg::PBMySQLDatas* pbdatas );

protected:
    virtual void DoDataExchange( CDataExchange* pDX );  // DDX/DDV 支持

    DECLARE_MESSAGE_MAP()
public:
    CListCtrl _list_resource;
    virtual BOOL PreTranslateMessage( MSG* pMsg );
    afx_msg void OnNMDblclkListResource( NMHDR* pNMHDR, LRESULT* pResult );
};
