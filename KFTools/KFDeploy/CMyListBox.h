#pragma once
#include <afxwin.h>
class CMyListBox :
    public CListBox
{
public:
    CMyListBox();
    ~CMyListBox();
    DECLARE_MESSAGE_MAP()
    afx_msg void OnRButtonUp( UINT nFlags, CPoint point );
};

