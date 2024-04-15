// TreeCtrlEx.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "2.h"
#include "TreeCtrlEx.h"


// CTreeCtrlEx

IMPLEMENT_DYNAMIC(CTreeCtrlEx, CTreeCtrl)

CTreeCtrlEx::CTreeCtrlEx()
{
}

CTreeCtrlEx::~CTreeCtrlEx()
{
}


BEGIN_MESSAGE_MAP(CTreeCtrlEx, CTreeCtrl)
END_MESSAGE_MAP()

void CTreeCtrlEx::SetItemBold(HTREEITEM hItem,BOOL bBold)
{
	SetItemState( hItem, bBold?TVIS_BOLD:0, TVIS_BOLD );
}

// CTreeCtrlEx ��Ϣ�������

/***************************************************************
*
* ���ƣ�FindItem
*
* ���ܣ����������и������������Ʋ���������
*
* ������item1 - Ҫ��ѯ�����������������
*
*		 strText - Ҫ��ѯ������������
*
* ����ֵ��NULL - û�ҵ�
* 
* (�ǿյ�HTREEITEM ) - �ҵ� 
*
***************************************************************/
HTREEITEM CTreeCtrlEx::FindItem(const HTREEITEM item1, CString strText)
{
	HTREEITEM item=item1;

	if(item == TVI_ROOT)
		item=GetRootItem();

	if(MyGetItemText(item) == strText)
		return item;
	else
	{
		if(ItemHasChildren(item))
		{
			return _FindItem(GetChildItem(item),strText);
		}
	}

	return NULL;
}

HTREEITEM CTreeCtrlEx::_FindItem(const HTREEITEM item1, CString strText)
{
	HTREEITEM item=item1,hFind=NULL;

	if(item != NULL)
	{
		if(MyGetItemText(item) == strText)
			return item;

		if(ItemHasChildren(item))
		{
			hFind=_FindItem(GetChildItem(item),strText);
			if(hFind)
				return hFind;
		}
		item=GetNextSiblingItem(item);
		if(item)
		{
			hFind=_FindItem(item,strText);
			if(hFind)
				return hFind;
		}
	}

	return NULL;
}

/***************************************************************
*
* ���ƣ�MyInsertItem
*
* ���ܣ������ؼ��в�������
*
* ������par- ����������ĸ���
*
*		text - ���������������
*
* ����ֵ��NULL - ����ʧ��
* 
*		  (�ǿյ�HTREEITEM ) - ����ɹ�
*
***************************************************************/
HTREEITEM CTreeCtrlEx::MyInsertItem(HTREEITEM par, CString text)
{
	HTREEITEM t3,t2,t1=GetChildItem(par);
	DWORD_PTR data=0;
	while(t1)
	{
		if( MyGetItemText(t1) == text)	//���ڴ˸��ڵ�������ͬ���Ľڵ㣬���ټ���
			return NULL;
		else
			t1=GetNextItem(t1,TVGN_NEXT);
	}

	t2=FindItem(TVI_ROOT,text);			//���������в��Ҵ�����Ľڵ㣬���ҵ�����˵���������ӽṹ���ֱ䶯
	if(t2)								//�ýڵ�ĸ������ϲ�ڵ㣬��ɾ��ԭ����λ�ã����뵽��λ��
	{
		data=GetItemData(t2);			//����ԭ���洢�Ľڵ���Ϣ
		DeleteItem(t2);
	}

	if(text == _T("0"))
		text=_T("Э����")+text;
	else
		text=_T("�ڵ�")+text;
	t3=InsertItem(text,par,TVI_LAST);
	if(t3)
		SetItemData(t3,data);

	return t3;
}

//ɾ���ڵ�ʱ����õ��洢�ڵ���Ϣʱ������ڴ��ַ�������ͷ��ڴ�
//�������Ϊ�ڵ��ַ(CString)
//���������ȷɾ�����򷵻صõ��ĵ�ַ�����򷵻�0
BOOL CTreeCtrlEx::MyDeleteItem(CString nodestr)
{
	HTREEITEM nodeitem;
	nodeitem=FindItem(TVI_ROOT,nodestr);
	return MyDeleteItem(nodeitem);
}

BOOL CTreeCtrlEx::MyDeleteItem(HTREEITEM nodeitem)
{
	if(nodeitem)
	{
		CString text=GetItemText(nodeitem);
		TreeVisitFree(nodeitem);
		if(DeleteItem(nodeitem))
			WriteLog(_T("ɾ��%s"),text);
	}

	return FALSE;
}

CString CTreeCtrlEx::MyGetItemText(HTREEITEM t)
{
	CString S1=GetItemText(t);
	if(-1 != S1.Find(_T("�ڵ�")))
		S1=S1.Right(S1.GetLength()-2);
	else
		if(-1 != S1.Find(_T("Э����")))
			S1=S1.Right(S1.GetLength()-3);
	return S1;
}

//�ͷ�������������½ڵ�Ϊ����ڵ���Ϣʱ��������ڴ�
void CTreeCtrlEx::TreeVisitFree(HTREEITEM hItem)  
{     
	if(hItem)
	{
		DWORD_PTR dataaddr;
		dataaddr=GetItemData(hItem);
		free((void*)dataaddr);
		WriteLog(_T("�ͷ�Ϊ%s������ڴ�"),GetItemText(hItem));
		if(hItem && ItemHasChildren(hItem))   
		{
			HTREEITEM hChildItem = GetChildItem(hItem);   
			while(hChildItem!=NULL)
			{
				TreeVisitFree(hChildItem);
				hChildItem = GetNextItem(hChildItem, TVGN_NEXT);   
			}   
		}  
	}
} 

void CTreeCtrlEx::TreeVisitComput(CString itemname)  
{     
	HTREEITEM hItem=FindItem(TVI_ROOT,itemname);
	TreeVisitComput(hItem);
} 

void CTreeCtrlEx::TreeVisitComput(HTREEITEM hItem)  
{     
	if(hItem)
	{
		if(MyGetItemText(hItem) != _T("0"))
		{
			DWORD nowtime=GetTickCount();
			pNodeExtInfo pnodeinfo=(pNodeExtInfo)GetItemData(hItem);
			if(pnodeinfo)
			{
				if(nowtime - pnodeinfo->lifetime > NODEPARSONOVER)
					MyDeleteItem(hItem);
			}
		}
		if(hItem)
		{
			if(ItemHasChildren(hItem))   
			{
				HTREEITEM hChildItem = GetChildItem(hItem);   
				while(hChildItem!=NULL)
				{
					TreeVisitComput(hChildItem);
					hChildItem = GetNextItem(hChildItem, TVGN_NEXT);   
				}   
			}
		}
	}
} 

