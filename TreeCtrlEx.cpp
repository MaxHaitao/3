// TreeCtrlEx.cpp : 实现文件
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

// CTreeCtrlEx 消息处理程序

/***************************************************************
*
* 名称：FindItem
*
* 功能：在树控制中根据数据项名称查找数据项
*
* 参数：item1 - 要查询的数据项及其子数据项
*
*		 strText - 要查询的数据项名称
*
* 返回值：NULL - 没找到
* 
* (非空的HTREEITEM ) - 找到 
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
* 名称：MyInsertItem
*
* 功能：在树控件中插入子项
*
* 参数：par- 待插入子项的父项
*
*		text - 待插入子项的数据
*
* 返回值：NULL - 插入失败
* 
*		  (非空的HTREEITEM ) - 插入成功
*
***************************************************************/
HTREEITEM CTreeCtrlEx::MyInsertItem(HTREEITEM par, CString text)
{
	HTREEITEM t3,t2,t1=GetChildItem(par);
	DWORD_PTR data=0;
	while(t1)
	{
		if( MyGetItemText(t1) == text)	//若在此父节点下已有同名的节点，则不再加入
			return NULL;
		else
			t1=GetNextItem(t1,TVGN_NEXT);
	}

	t2=FindItem(TVI_ROOT,text);			//在整个树中查找待插入的节点，若找到，则说明网络拓朴结构出现变动
	if(t2)								//该节点的更换了上层节点，则删除原来的位置，插入到新位置
	{
		data=GetItemData(t2);			//保存原来存储的节点信息
		DeleteItem(t2);
	}

	if(text == _T("0"))
		text=_T("协调器")+text;
	else
		text=_T("节点")+text;
	t3=InsertItem(text,par,TVI_LAST);
	if(t3)
		SetItemData(t3,data);

	return t3;
}

//删除节点时，需得到存储节点信息时分配的内存地址，用以释放内存
//输入参数为节点地址(CString)
//输出：若正确删除，则返回得到的地址，否则返回0
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
			WriteLog(_T("删除%s"),text);
	}

	return FALSE;
}

CString CTreeCtrlEx::MyGetItemText(HTREEITEM t)
{
	CString S1=GetItemText(t);
	if(-1 != S1.Find(_T("节点")))
		S1=S1.Right(S1.GetLength()-2);
	else
		if(-1 != S1.Find(_T("协调器")))
			S1=S1.Right(S1.GetLength()-3);
	return S1;
}

//释放输入参数及其下节点为保存节点信息时所分配的内存
void CTreeCtrlEx::TreeVisitFree(HTREEITEM hItem)  
{     
	if(hItem)
	{
		DWORD_PTR dataaddr;
		dataaddr=GetItemData(hItem);
		free((void*)dataaddr);
		WriteLog(_T("释放为%s分配的内存"),GetItemText(hItem));
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

