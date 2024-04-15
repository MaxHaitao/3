#pragma once


// CTreeCtrlEx

class CTreeCtrlEx : public CTreeCtrl
{
	DECLARE_DYNAMIC(CTreeCtrlEx)

public:
	CTreeCtrlEx();
	virtual ~CTreeCtrlEx();

	void SetItemBold(HTREEITEM hItem,BOOL bBold);

	HTREEITEM	FindItem(HTREEITEM , CString );
	HTREEITEM	MyInsertItem(HTREEITEM , CString);
	BOOL		MyDeleteItem(HTREEITEM);
	BOOL		MyDeleteItem(CString);
	CString		MyGetItemText(HTREEITEM);
	void		TreeVisitFree(HTREEITEM);
	void		TreeVisitComput(HTREEITEM);
	void		TreeVisitComput(CString);

protected:
	HTREEITEM _FindItem(HTREEITEM,CString);
	DECLARE_MESSAGE_MAP()
};


