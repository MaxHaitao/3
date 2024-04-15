// 2Dlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "2.h"
#include "2Dlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


//��ֵ��СӦ�õ�������Ϊ������Ҷ�Ӧ��ģ������ʱ���˶��ֲ��ҷ�
const WORD g_IDHEX[]={	0x1000,0x1001,0x1010,0x1011,0x1018,0x1019,0x1020,0x1030,0x1031,0x1032,
						0x1033,0x1040,0x1050,0x2000,0x2001,0x2010,0x2020,0x2100,0x2101,0x2102,
						0x2110,0x3000,0x3010,0x3020,0x3100
					 };
//˳����g_IDHEX��Ӧ,��NULL����
const TCHAR *g_IDSTR[100]={	_T("��ѹ�ɼ�ģ��(��ͨ��)"),			_T("��ѹ�ɼ�ģ��(˫ͨ��)"),
							_T("�����ɼ�ģ��(��ͨ��)"),			_T("�����ɼ�ģ��(˫ͨ��)"),
							_T("΢����������(��ͨ��)"),			_T("΢����������(˫ͨ��)"),
							_T("����������(��ͨ��)"),				_T("ѹ��������ģ��"),
							_T("�ƾ�������ģ��"),					_T("ѹ��������ģ��+�ƾ�������ģ��"),
							_T("ѹǿ������"),						_T("�Ŵ�����"),
							_T("λ�ƴ�����"),						_T("��ʪ�ȴ�����"),
							_T("��ʪ�ȴ�����+����������(ADIN0)"),	_T("��ѹ���������"),
							_T("�̵������������+GPIN������"),	_T("����Ŵ�����"),
							_T("�������������"),					_T("�������������+����Ŵ�����"),
							_T("������������"),					_T("RS232�ӿڿ���(���߽ڵ�)"),
							_T("RDID�ӿڿ�����"),					_T("���´�����"),
							_T("RS232�ӿڿ���(Э����)"),			NULL
};

// CMy2Dlg �Ի���

CMy2Dlg::CMy2Dlg(CWnd* pParent /*=NULL*/)
	: CDialog(CMy2Dlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

CMy2Dlg::~CMy2Dlg()
{
	if(m_timesocket.m_hSocket != INVALID_SOCKET)
	{
		WriteLog(_T("�ر�����\r\n"));
		m_timesocket.Close();
	}
}

void CMy2Dlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CMy2Dlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDOK, &CMy2Dlg::OnBnClickedOk)
	ON_BN_CLICKED(IDC_BTN_CONNECT, &CMy2Dlg::OnBnClickedBtnConnect)
	ON_WM_TIMER()
	ON_NOTIFY(NM_DBLCLK, IDC_TREE_NODE, &CMy2Dlg::OnNMDblclkTreeNode)
	ON_BN_CLICKED(IDC_RADIO1, &CMy2Dlg::OnBnClickedRadio1)
	ON_BN_CLICKED(IDC_RADIO2, &CMy2Dlg::OnBnClickedRadio2)
	ON_BN_CLICKED(IDC_RADIO3, &CMy2Dlg::OnBnClickedRadio3)
	ON_BN_CLICKED(IDC_BTN_SAMPLE, &CMy2Dlg::OnBnClickedBtnSample)
END_MESSAGE_MAP()


// CMy2Dlg ��Ϣ�������

BOOL CMy2Dlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������
	CWnd *pWnd;
	CRect rect;
	pWnd = GetDlgItem(IDC_STATIC_GRAPH);
	pWnd->GetClientRect(rect);
	m_Graph.Create(WS_VISIBLE, rect, pWnd);

	m_ptreeview=(CTreeCtrlEx*)GetDlgItem(IDC_TREE_NODE);
	m_plotchannel=0;
	receivelen=0;
	leftlen=0;

	InitUI();

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

void CMy2Dlg::InitUI()
{
	m_Graph.SetXRange(0, 100, 0);
	m_Graph.SetRange(0, 200, 0) ;

	m_Graph.SetYUnits(_T("")) ;
	m_Graph.SetXUnits(_T("Time")) ;
	m_Graph.SetBackgroundColor(RGB(0, 0, 64)) ;
	m_Graph.SetGridColor(RGB(192, 192, 255)) ;
	m_Graph.SetPlotColor(RGB(0, 255, 0)) ;

}

void CMy2Dlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	CDialog::OnSysCommand(nID, lParam);
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CMy2Dlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ��������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù����ʾ��
//
HCURSOR CMy2Dlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CMy2Dlg::OnOK()
{
	// TODO: �ڴ����ר�ô����/����û���

	//CDialog::OnOK();
}

void CMy2Dlg::OnBnClickedOk()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	//OnOK();

	CDialog::OnOK();
}

void CMy2Dlg::OnBnClickedBtnConnect()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CString temp;
	GetDlgItem(IDC_BTN_CONNECT)->GetWindowText(temp);
	if(temp == _T("����"))	//����
	{
		INT_PTR nResponse = m_connectdlg.DoModal();
		if(nResponse == IDOK)
		{
			m_connectaddr=m_connectdlg.m_connectaddr;
			m_connectportstr=m_connectdlg.m_connectport;
			m_connectport=_tcstoul(m_connectportstr,NULL,10);
			if(BeginConnectToSer())
				//GetDlgItem(IDC_BTN_CONNECT)->EnableWindow(FALSE);
				GetDlgItem(IDC_BTN_CONNECT)->SetWindowText(_T("�Ͽ�����"));
		}
	}
	else					//�Ͽ�����
	{
		m_ptreeview->MyDeleteItem(_T("0"));	
		KillTimer(HEARTBEATTOSERID);
		KillTimer(NODEPARSONINFOID);
		if(m_timesocket.m_hSocket != INVALID_SOCKET)
		{
			WriteLog(_T("�ر�����\r\n"));
			m_timesocket.Close();
		}
		GetDlgItem(IDC_BTN_CONNECT)->SetWindowText(_T("����"));
		InitUI();
	}
}

BOOL CMy2Dlg::BeginConnectToSer()
{
	m_bheartbeatfromserover=FALSE;
	m_timesocket.GetDlg(this);
	if(m_timesocket.m_hSocket == INVALID_SOCKET)
		m_timesocket.Create();
	BOOL r;
	int i;
	for(i=0;i<RETRYTIME;i++)
	{
		r=m_timesocket.MyConnect(5000,m_connectaddr,m_connectport);
		if(!r && i<RETRYTIME-1)
			WriteLog(_T("�������ӵ����������Զ�����"));
		else
			break;
	}
	if(r)
	{
		WriteLog(_T("�ɹ����ӵ�������"));
		AppIdentMsg appidentmsg;
		appidentmsg.head.header='@';
		appidentmsg.head.len=9;
		appidentmsg.head.id=0;
		appidentmsg.head.type=APPINENTMSGTYPE;
		appidentmsg.apptype=1;
		appidentmsg.serheadbeatover=HEARTBEATINTERBALTOSER*3;
		appidentmsg.serheadbeatinte=HEARTBEATINTERBALFROMSER;
		appidentmsg.sum=MyGetSum((BYTE*)&appidentmsg,appidentmsg.head.len);

		Sleep(1000);

		CommACKMsg *pcommack;
		DWORD starttime=GetTickCount();
		MsgHeader *pmsgheader;
		char *pchar;
		int left;
		r=-1;
		while( r == -1 && (GetTickCount()-starttime) < 10000)	//Ҫ����������յ�Ӧ��ʶ��Ӧ��������10��
		{
			m_timesocket.MySend(5000,&appidentmsg,appidentmsg.head.len);
			left=m_timesocket.MyReceive(5000,m_sbuffer,SOCKETBUFFERSIZE);
			while(left >=sizeof(MsgHeader))				//һ���п����յ�����������п��ܲ��������Ӧ�������Ȼ���տ�ʼ���ӣ�Ӧ�ò����ڣ������Ǵ���һ��
			{
				pmsgheader=(MsgHeader*)m_sbuffer;
				pchar=m_sbuffer;
				if(pmsgheader->type==APPINENTMSGTYPE)
				{
					pcommack=(CommACKMsg*)pchar;
					r=1;
					break;
				}
				else
				{
					left-=pmsgheader->len;
					pchar+=pmsgheader->len;
				}
			}
		}
		if(r == 1)
		{
			if(pcommack->ack == 0)		//Ӧ��ʶ��Ӧ����룬00��ȷ����
			{
				WriteLog(_T("�������������ɲ��յ���������'Ӧ��ʶ����Ϣ'�ĳɹ�Ӧ��"));
				MessageBox(_T("��������ɹ�����ͨѸ"));
				SetTimer(HEARTBEATTOSERID,HEARTBEATINTERBALTOSER*1000,NULL);
				receivelen=0;
				leftlen=0;
				m_heartbeatfromser=GetTickCount();
				return TRUE;
			}
			else						//Ӧ��ʶ��Ӧ����룬δ��ȷ���գ�����...
			{
				WriteLog(_T("�յ���������'Ӧ��ʶ����Ϣ'�Ĵ���Ӧ��"));
				MessageBox(_T("�յ���������'Ӧ��ʶ����Ϣ'�Ĵ���Ӧ��"));
			}
		}
		else
		{
			MessageBox(_T("�����յ�������Ӧ��"));
			WriteLog(_T("���Ӻ���ղ�����������'Ӧ��ʶ����Ϣ'��Ӧ��"));
		}
	}
	else
	{
		MessageBox(_T("�������ӵ����������ر�����"));
		WriteLog(_T("�������ӵ����������ر�����"));
		m_timesocket.Close();
	}

	return FALSE;
}

void CMy2Dlg::MyOnReceive()
{
	CString addr;		//�ڵ��ַ
	CString paraddr;	//���ڵ��ַ
	CString temp;		//��ʱ��������������
	DevConnectMsg *devconmsg;
	DevDisConnectMsg *devdisconmsg;
	WORD *ids;
	int idnums,i;
	CommACKMsg ack;			//һ��Ӧ��
	NodeCommMsg nodecom;	//�ڵ�һ��Ӧ��
	pNodeExtInfo nodedata=0;

	nodecom.head.header='@';
	nodecom.head.len=8;
	ack.head.header='@';
	ack.head.len=7;
	ack.head.id=0;

	m_pcstemp=m_pcstemp2=m_sbuffer;		//m_pcstemp2Ϊ��ǰ������Ļ�ַ��m_pcstempΪ��ǰ�������ݵ�ָ��
	MsgHeader *pmsgheader;
	MoveMemory(m_sbuffer,m_sbuffer+receivelen-leftlen,leftlen);
	receivelen=m_timesocket.Receive(m_sbuffer+leftlen,SOCKETBUFFERSIZE-leftlen);
	leftlen+=receivelen;
	receivelen=leftlen;
	pmsgheader=(MsgHeader*)m_pcstemp;
	while(leftlen >= sizeof(MsgHeader) && leftlen >= pmsgheader->len)	//һ�ο����յ���������ж��Ƿ���
	{
		if('@' != pmsgheader->header)
		{
			//����һ���Ϸ��İ�
		}
		else
		{
			if(MyGetSum((BYTE*)m_pcstemp,pmsgheader->len) != (BYTE)*(m_pcstemp+pmsgheader->len-1))
			{
				//У��Ͳ���ȷ,����...
			}
			switch(pmsgheader->type)
			{
			case HEARTBEATMSGTYPE:		//������Ϣ
				CommMsg *commmsg;
				commmsg=(CommMsg*)m_pcstemp;

				if( GetTickCount()-m_heartbeatfromser > HEARTBEATOVERFROMSER)
					m_bheartbeatfromserover=TRUE;		//��ʱ
				else
					m_bheartbeatfromserover=FALSE;

				m_heartbeatfromser=GetTickCount();
				break;

			case DEVCONNECTMSGTYPE:		//�豸�������������Ϣ
				devconmsg=(DevConnectMsg*)m_pcstemp;
				ids=(WORD*)(((BYTE*)devconmsg)+5);
				idnums=(devconmsg->head.len-6)/2;
				for(i=0;i<idnums;i++)
				{
					temp.Format(_T("%d"),i);
					m_ptreeview->MyInsertItem(TVI_ROOT,temp);		//ֻ��һ��Э����ͨ�ţ����ûỰID��Э������ַΪ0
					temp.Empty();
					temp.Format(_T("�豸�����������,idΪ%d"),devconmsg->devsesionid);
					m_selectedid=devconmsg->devsesionid;			//����ỰID
					WriteLog((LPTSTR)(LPCTSTR)temp);
					temp.Empty();
					m_ptreeview->Invalidate();
					nodecom.head.id=devconmsg->devsesionid;
					nodecom.head.type=GETNODEINFOMSGTYPE;
					nodecom.addr=i;
					nodecom.sum=MyGetSum((BYTE*)&nodecom,nodecom.head.len);
					m_timesocket.MySend(5000,&nodecom,nodecom.head.len);
				}

				ack.head.type=DEVCONNECTMSGTYPE;
				ack.ack=ACKOK;
				ack.sum=MyGetSum((BYTE*)&ack,ack.head.len);
				m_timesocket.MySend(5000,&ack,ack.head.len);

				SetTimer(NODEPARSONINFOID,1000,NULL);

				break;
		
			case DEVDISCONNECTMSGTYPE:	//�豸��������Ͽ�������Ϣ
				devdisconmsg=(DevDisConnectMsg*)m_pcstemp;
				m_ptreeview->MyDeleteItem(_T("0"));	//����ֻ��һ��Э����ͨ�ţ��Ͽ�ʱֱ�������
				m_ptreeview->Invalidate();
				temp.Format(_T("�豸��������Ͽ�,idΪ%d"),devconmsg->devsesionid);
				m_selectedid=0;
				WriteLog((LPTSTR)(LPCTSTR)temp);
				temp.Empty();
				break;

			case UNODEPARSONMSGTYPE:		//�ڵ㸸�ӹ�ϵ��Ϣ
				NodeParsonMsg *nodeparsonmsg;
				nodeparsonmsg=(NodeParsonMsg*)m_pcstemp;
				addr.Format(_T("%d"),nodeparsonmsg->NodeAddr);
				paraddr.Format(_T("%d"),nodeparsonmsg->ParNodeAddr);

				//ֻ��һ��Э����ͨ�ţ�ֱ���ڸ��ڵ��¼��ӽڵ�
				HTREEITEM par,cor,newh;
				cor=m_ptreeview->FindItem(TVI_ROOT,_T("0"));	
				if(cor != NULL)
				{
					par=m_ptreeview->FindItem(cor,paraddr);
// 					if(!par)			//���Ҳ������ڵ㣬����ӵ�Э������
// 						par=cor;
					if(par != NULL)		//ֻ�ڸ��ӹ�ϵ�еĸ��ڵ������
					{
						newh=m_ptreeview->MyInsertItem(par,addr);
						if(newh)
						{
							temp.Format(_T("�ڽڵ�%s�����ӽڵ�%s"),paraddr,addr);
							WriteLog((LPTSTR)(LPCTSTR)temp);
							temp.Empty();
							Invalidate();
							nodecom.head.id=nodeparsonmsg->head.id;
							nodecom.head.type=GETNODEINFOMSGTYPE;
							nodecom.addr=nodeparsonmsg->NodeAddr;
							nodecom.sum=MyGetSum((BYTE*)&nodecom,nodecom.head.len);
							m_timesocket.MySend(5000,&nodecom,nodecom.head.len);
						}
						else		//ԭ�нڵ㣬���ָ��ӹ�ϵ
						{
							newh=m_ptreeview->FindItem(cor,addr);
							if(newh)
							{
								nodedata=(pNodeExtInfo)(m_ptreeview->GetItemData(newh));
								if(nodedata)
									nodedata->lifetime=GetTickCount();
							}
						}
					}
				}
				break;
	
			case UNODEINFOMSGTYPE:		//�ڵ���Ϣ��Ϣ
				DoNodeInfoData(m_pcstemp);
				break;

			case USENSORFIXMSGTYPE:		//����������У׼��Ϣ
				DoSensorFixData(m_pcstemp);
				break;

			case UEXTENDRESMSGTYPE:		//�ϴ���չģ����Դ������Ϣ
				DoExternData(m_pcstemp);
				break;

			default:
				;
			}
			leftlen-=pmsgheader->len;
			m_pcstemp+=pmsgheader->len;
		}
		pmsgheader=(MsgHeader*)m_pcstemp;
	}
}

void CMy2Dlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ

	if(HEARTBEATTOSERID == nIDEvent)
	{
		CommMsg heartbeat;
		heartbeat.head.header='@';
		heartbeat.head.len=sizeof(heartbeat);
		heartbeat.head.id=0;
		heartbeat.head.type=HEARTBEATMSGTYPE;
		heartbeat.sum=MyGetSum((BYTE*)&heartbeat,heartbeat.head.len);
		m_timesocket.Send(&heartbeat,sizeof(heartbeat));
	}

	//���ӽڵ��ϵ�Ƿ�ʱ
	if(NODEPARSONINFOID == nIDEvent)
	{
		m_ptreeview->TreeVisitComput(_T("0"));
	}

	if(m_bheartbeatfromserover || GetTickCount()-m_heartbeatfromser > HEARTBEATOVERFROMSER)
	{
		m_bheartbeatfromserover=FALSE;
		WriteLog(_T("���շ�����������ʱ���ر�����\r\n"));
		m_timesocket.Close();
		KillTimer(HEARTBEATTOSERID);
		KillTimer(NODEPARSONINFOID);
		MessageBox(_T("���շ�����������ʱ���Զ��رյ�ǰ���Ӳ���������"));
		WriteLog(_T("��������\r\n"));
		if(BeginConnectToSer())
			GetDlgItem(IDC_BTN_CONNECT)->EnableWindow(FALSE);
		else
			MessageBox(_T("�������ӵ���������������"));
	}

	CDialog::OnTimer(nIDEvent);
}


//�ڵ���Ϣ��Ϣ����
void CMy2Dlg::DoNodeInfoData(const char* p)
{
	NodeInfoMsg *t=(NodeInfoMsg*)p;
	CString id,addr,mboard,ext,tmp;
	id.Format(_T("%d"),t->head.id);
	addr.Format(_T("%d"),t->addr);

	pNodeExtInfo nodeextinfo;

	HTREEITEM tt;
	tt=m_ptreeview->FindItem(TVI_ROOT,addr);
	DWORD_PTR olddata=0;
	if(tt)
	{
		olddata=m_ptreeview->GetItemData(tt);
		if(olddata)	//��ԭ���ѷ����ڴ棬��ֱ����ԭ��������ڴ��ַ�������·����ڴ�
		{
			nodeextinfo=(NodeExtInfo*)olddata;
		}
		else
		{
			nodeextinfo=(NodeExtInfo*)malloc(sizeof(NodeExtInfo));	//�ڶ��з���һ���ڴ汣��ڵ��IEEE,LQI,RSSI,PANID
			ZeroMemory(nodeextinfo,sizeof(NodeExtInfo));
		}	
		nodeextinfo->IEEE[0]=t->IEEE[0];
		nodeextinfo->IEEE[1]=t->IEEE[1];
		nodeextinfo->LQI=t->LQI;
		nodeextinfo->PANID=t->PANID;
		nodeextinfo->RSSI=t->RSSI;
		nodeextinfo->ADDR=t->addr;
		nodeextinfo->mboardtype=t->mboardtype;
		nodeextinfo->extboardtype=t->extboardtype;
		nodeextinfo->lifetime=GetTickCount();
		m_ptreeview->SetItemData(tt,(DWORD_PTR)nodeextinfo);
	}
}

//�����ϴ�����չģ����Դ������Ϣ
void CMy2Dlg::DoExternData(const char* p)
{
	ExternResData *t=(ExternResData*)p;
	if(m_selectednode == t->addr)		//ֻ�з��ϵ�id��node�����ݲŴ���
	{
		UINT datanums=(t->head.len-10)/2,i=0;		//ʵ�����ݵĸ���
		WORD *data=(WORD*)(((BYTE*)t)+9);			//ָ��ʵ�ʵ�����
		BYTE *tdata;
		WORD dt1,dt2,dt3;
		float ft1,ft2,ft3;
		int light;
		CString datastr;
		//...������չģ����Դ
		switch(t->extcode)
		{
		case 0X1000:		//��ͨ����ѹ�ɼ�
			for(i=0;i<datanums;i++)
			{
				dt1=*(data+i);
				ft1=ComputeVoltage(dt1,m_nodeextinfo->fixdata[0].maxdata,
					m_nodeextinfo->fixdata[0].zerodata,m_nodeextinfo->fixdata[0].mindata);
				m_Graph.AppendPoint(ft1);
				datastr.Format(_T("��ѹ�ɼ����� %3.2fV"),ft1);
				GetDlgItem(IDC_DATA)->SetWindowText(datastr);
			}
			break;

		case 0x1001:		//˫ͨ����ѹ�ɼ�
			for(i=0;i<datanums;i+=2)
			{
				dt1=*(data+i);
				dt2=*(data+i+1);
				ft1=ComputeVoltage(dt1,m_nodeextinfo->fixdata[0].maxdata,
					m_nodeextinfo->fixdata[0].zerodata,m_nodeextinfo->fixdata[0].mindata);
				ft2=ComputeVoltage(dt2,m_nodeextinfo->fixdata[0].maxdata,
					m_nodeextinfo->fixdata[0].zerodata,m_nodeextinfo->fixdata[0].mindata);
				if(m_plotchannel == 1)
					m_Graph.AppendPoint(ft1);
				else
					if(m_plotchannel == 2)
						m_Graph.AppendPoint(ft2);
				datastr.Format(_T("��ѹ 1ͨ��:%3.2fV 2ͨ��:%3.2fV"),ft1,ft2);
				GetDlgItem(IDC_DATA)->SetWindowText(datastr);
			}
			break;

		case 0X1010:		//��ͨ�������ɼ�
			for(i=0;i<datanums;i++)
			{
				dt1=*(data+i);
				ft1=ComputeCurrent(dt1,m_nodeextinfo->fixdata[0].maxdata,
					m_nodeextinfo->fixdata[0].zerodata);
				m_Graph.AppendPoint(ft1);
				datastr.Format(_T("���� %3.2fmA"),ft1);
				GetDlgItem(IDC_DATA)->SetWindowText(datastr);
			}
			break;

		case 0x1011:		//˫ͨ�������ɼ�
			for(i=0;i<datanums;i+=2)
			{
				dt1=*(data+i);
				dt2=*(data+i+1);
				ft1=ComputeCurrent(dt1,m_nodeextinfo->fixdata[0].maxdata,
					m_nodeextinfo->fixdata[0].zerodata);
				ft2=ComputeCurrent(dt2,m_nodeextinfo->fixdata[0].maxdata,
					m_nodeextinfo->fixdata[0].zerodata);
				if(m_plotchannel == 1)
					m_Graph.AppendPoint(ft1);
				else
					if(m_plotchannel == 2)
						m_Graph.AppendPoint(ft2);
				datastr.Format(_T("���� 1ͨ��:%3.2fmA 2ͨ��:%3.2fmA"),ft1,ft2);
				GetDlgItem(IDC_DATA)->SetWindowText(datastr);
			}
			break;

		case 0X1020:		//����������
			for(i=0;i<datanums;i++)
			{
				dt1=*(data+i);
				light=ComputeLight(dt1);
				m_Graph.AppendPoint(light);
				datastr.Format(_T("���� %dLX"),light);
				GetDlgItem(IDC_DATA)->SetWindowText(datastr);
			}
			break;

		case 0X1030:		//ѹ��������
			for(i=0;i<datanums;i++)
			{
				dt1=*(data+i);
				m_Graph.AppendPoint(dt1);
				datastr.Format(_T("ѹ�� %dg"),dt1);
				GetDlgItem(IDC_DATA)->SetWindowText(datastr);
			}
			break;

		case 0X1031:		//�ƾ�������
			for(i=0;i<datanums;i++)
			{
				dt1=*(data+i);
				m_Graph.AppendPoint(dt1);
				datastr.Format(_T("�ƾ� %d"),dt1);
				GetDlgItem(IDC_DATA)->SetWindowText(datastr);
			}
			break;

		case 0X1032:		//ѹ���ƾ�������ģ��
			for(i=0;i<datanums;i+=2)
			{
				dt1=*(data+i);
				dt2=*(data+i+1);
				if(m_plotchannel == 1)
					m_Graph.AppendPoint(dt1);
				else
					if(m_plotchannel == 2)
						m_Graph.AppendPoint(dt2);
				datastr.Format(_T("ѹ��:%dg �ƾ�:%d"),dt1,dt2);
				GetDlgItem(IDC_DATA)->SetWindowText(datastr);
			}
			break;

		case 0X2000:		//��ʪ�ȴ�����
			for(i=0;i<datanums;i+=2)
			{
				dt1=*(data+i);
				dt2=*(data+i+1);
				if(m_plotchannel == 1)
					m_Graph.AppendPoint(dt1);
				else
					if(m_plotchannel == 2)
						m_Graph.AppendPoint(dt2);
				datastr.Format(_T("�¶�:%d�� ʪ��:%d%%"),dt1,dt2);
				GetDlgItem(IDC_DATA)->SetWindowText(datastr);
			}
			break;

		case 0X2001:		//��ʪ�ȴ�����,����
			for(i=0;i<datanums;i+=3)
			{
				dt1=*(data+i);
				dt2=*(data+i+1);
				dt3=*(data+i+2);
				light=ComputeLight(dt3);
				if(m_plotchannel == 1)
					m_Graph.AppendPoint(dt1);
				else
				{
					if(m_plotchannel == 2)
						m_Graph.AppendPoint(dt2);
					else
						if(m_plotchannel == 3)
							m_Graph.AppendPoint(light);
				}
				datastr.Format(_T("�¶�:%d�� ʪ��:%d%% ����:%dLX"),dt1,dt2,light);
				GetDlgItem(IDC_DATA)->SetWindowText(datastr);
			}
			break;

		case 0x2010:		//��ѹ���������
			GetDlgItem(IDC_DATA)->SetWindowText(_T(""));
			break;

		case 0x2020:		//�̵������������
			GetDlgItem(IDC_DATA)->SetWindowText(_T(""));
			tdata=(BYTE*)data;
			if((*tdata)&0x01)
				((CButton*)GetDlgItem(IDC_RELAY5))->SetCheck(BST_UNCHECKED);
			else
				((CButton*)GetDlgItem(IDC_RELAY5))->SetCheck(BST_CHECKED);
			if((*tdata)&0x02)
				((CButton*)GetDlgItem(IDC_RELAY6))->SetCheck(BST_UNCHECKED);
			else
				((CButton*)GetDlgItem(IDC_RELAY6))->SetCheck(BST_CHECKED);
			if((*tdata)&0x04)
				((CButton*)GetDlgItem(IDC_RELAY7))->SetCheck(BST_UNCHECKED);
			else
				((CButton*)GetDlgItem(IDC_RELAY7))->SetCheck(BST_CHECKED);
			if((*tdata)&0x08)
				((CButton*)GetDlgItem(IDC_RELAY8))->SetCheck(BST_UNCHECKED);
			else
				((CButton*)GetDlgItem(IDC_RELAY8))->SetCheck(BST_CHECKED);
			break;

		case 0X2100:		//����Ŵ�����
			for(i=0;i<datanums;i++)
			{
				dt1=*(data+i);
				m_Graph.AppendPoint(dt1);
				datastr.Format(_T("����� %d"),dt1);
				GetDlgItem(IDC_DATA)->SetWindowText(datastr);
			}
			break;

		case 0X2110:		//������������
			for(i=0;i<datanums;i++)
			{
				dt1=*(data+i);
				m_Graph.AppendPoint(dt1);
				datastr.Format(_T("������ %d"),dt1);
				GetDlgItem(IDC_DATA)->SetWindowText(datastr);
			}
			break;

		case 0x3000:		//RS232�ӿڿ��ƣ����߽ڵ㣩
			break;

		default:
			;
		}
	}
}


//���������οؼ���˫��
void CMy2Dlg::OnNMDblclkTreeNode(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	HTREEITEM t1 = m_ptreeview->GetSelectedItem();	
	if(t1 == NULL) return ;

	CString ttstr;
	ttstr.Format(_T("%d"),m_selectednode);
	HTREEITEM tt=m_ptreeview->FindItem(TVI_ROOT,ttstr);
	m_ptreeview->SetItemBold(tt,FALSE);	//ȡ��֮ǰѡ�нڵ�Ĵ�����ʾ
	m_ptreeview->SetItemBold(t1,TRUE);	//��ǰ��ѡ�еĽڵ������Դ�����ʾ
	DWORD data=0;
	data=m_ptreeview->GetItemData(t1);
	if(data)
	{
 		m_selectednode=0xFFFF & _tcstoul(m_ptreeview->MyGetItemText(t1),NULL,10);

		CString S1;
		S1.Format(_T("�ڵ��ַ:%d,IEEE��ַ:0x%08X%08X,RSSI:%d,LQI:%d,PANID:%d"),
			m_selectednode,((pNodeExtInfo)data)->IEEE[0],((pNodeExtInfo)data)->IEEE[1],
			((pNodeExtInfo)data)->RSSI,((pNodeExtInfo)data)->LQI,((pNodeExtInfo)data)->PANID);
		GetDlgItem(IDC_STATIC_NODEINFO)->SetWindowText(S1);
		m_mboardtype=((pNodeExtInfo)data)->mboardtype;
		m_extboardtype=((pNodeExtInfo)data)->extboardtype;
	
		GetDlgItem(IDC_BTN_SAMPLE)->ShowWindow(TRUE);
		ShowPlot(FALSE);
		GetDlgItem(IDC_MOTEXT)->SetWindowText(_T(""));
		GetDlgItem(IDC_DATA)->SetWindowText(_T(""));
		m_plotchannel=1;
		CString name=GetIDName(m_mboardtype,m_extboardtype);
		GetDlgItem(IDC_MOTEXT)->SetWindowText(name);
		GetDlgItem(IDC_BTN_SAMPLE)->SetWindowText(_T("��ʼ����"));

		switch(m_extboardtype)
		{
		case 0:			//����չģ��
			GetDlgItem(IDC_BTN_SAMPLE)->ShowWindow(FALSE);
			break;

		case 0x1001:	//˫ͨ����ѹ�ɼ�
			GetDlgItem(IDC_PLOT)->ShowWindow(TRUE);
			GetDlgItem(IDC_RADIO1)->ShowWindow(TRUE);
			GetDlgItem(IDC_RADIO2)->ShowWindow(TRUE);
			GetDlgItem(IDC_RADIO1)->SetWindowText(_T("��ѹ1ͨ��"));
			GetDlgItem(IDC_RADIO2)->SetWindowText(_T("��ѹ2ͨ��"));
			((CButton*)GetDlgItem(IDC_RADIO1))->SetCheck(TRUE);
			break;

		case 0x1011:	//˫ͨ�������ɼ�
			GetDlgItem(IDC_PLOT)->ShowWindow(TRUE);
			GetDlgItem(IDC_RADIO1)->ShowWindow(TRUE);
			GetDlgItem(IDC_RADIO2)->ShowWindow(TRUE);
			GetDlgItem(IDC_RADIO1)->SetWindowText(_T("����1ͨ��"));
			GetDlgItem(IDC_RADIO2)->SetWindowText(_T("����2ͨ��"));
			((CButton*)GetDlgItem(IDC_RADIO1))->SetCheck(TRUE);
			break;

		case 0x1019:	//˫ͨ��΢����������
			GetDlgItem(IDC_PLOT)->ShowWindow(TRUE);
			GetDlgItem(IDC_RADIO1)->ShowWindow(TRUE);
			GetDlgItem(IDC_RADIO2)->ShowWindow(TRUE);
			GetDlgItem(IDC_RADIO1)->SetWindowText(_T("΢����1ͨ��"));
			GetDlgItem(IDC_RADIO2)->SetWindowText(_T("΢����2ͨ��"));
			((CButton*)GetDlgItem(IDC_RADIO1))->SetCheck(TRUE);
			break;
		
		case 0x1032:	//ѹ�����ƾ�������
			GetDlgItem(IDC_PLOT)->ShowWindow(TRUE);
			GetDlgItem(IDC_RADIO1)->ShowWindow(TRUE);
			GetDlgItem(IDC_RADIO2)->ShowWindow(TRUE);
			GetDlgItem(IDC_RADIO1)->SetWindowText(_T("ѹ������"));
			GetDlgItem(IDC_RADIO2)->SetWindowText(_T("�ƾ�����"));
			((CButton*)GetDlgItem(IDC_RADIO1))->SetCheck(TRUE);
			break;

		case 0x2000:	//��ʪ�ȴ�����
			GetDlgItem(IDC_PLOT)->ShowWindow(TRUE);
			GetDlgItem(IDC_RADIO1)->ShowWindow(TRUE);
			GetDlgItem(IDC_RADIO2)->ShowWindow(TRUE);
			GetDlgItem(IDC_RADIO1)->SetWindowText(_T("�¶�����"));
			GetDlgItem(IDC_RADIO2)->SetWindowText(_T("ʪ������"));
			((CButton*)GetDlgItem(IDC_RADIO1))->SetCheck(TRUE);
			break;

		case 0x2001:	//��ʪ�ȡ�����
			GetDlgItem(IDC_PLOT)->ShowWindow(TRUE);
			GetDlgItem(IDC_RADIO1)->ShowWindow(TRUE);
			GetDlgItem(IDC_RADIO2)->ShowWindow(TRUE);
			GetDlgItem(IDC_RADIO3)->ShowWindow(TRUE);
			GetDlgItem(IDC_RADIO1)->SetWindowText(_T("�¶�����"));
			GetDlgItem(IDC_RADIO2)->SetWindowText(_T("ʪ������"));
			GetDlgItem(IDC_RADIO3)->SetWindowText(_T("��������"));
			((CButton*)GetDlgItem(IDC_RADIO1))->SetCheck(TRUE);
			break;

		case 0x2010:	//��ѹ���������
			GetDlgItem(IDC_BTN_SAMPLE)->ShowWindow(FALSE);
			break;

		case 0x2020:	//�̵�����GPINģ��
			GetDlgItem(IDC_BTN_SAMPLE)->ShowWindow(FALSE);
			break;

		case 0x2101:	//�������������
		case 0x2102:	//�������������+����Ŵ�����
		case 0x3000:	//RS232�ӿڿ���(���߽ڵ�)
		case 0x3010:	//RDID�ӿڿ�����
		case 0x3020:	//���´�����
		case 0x3100:	//RS232�ӿڿ���(Э����)
			GetDlgItem(IDC_BTN_SAMPLE)->ShowWindow(FALSE);
			break;

		default:
			;
		}

		m_Graph.ClearPloted();
	}
	else
	{
		NodeCommMsg nodecom;
		nodecom.head.header='@';
		nodecom.head.len=8;
		nodecom.head.id=m_selectedid;
		nodecom.head.type=GETNODEINFOMSGTYPE;
		nodecom.addr=0xFFFF & _tcstoul(m_ptreeview->GetItemText(t1),NULL,10);
		nodecom.sum=MyGetSum((BYTE*)&nodecom,nodecom.head.len);
		m_timesocket.MySend(5000,&nodecom,nodecom.head.len);
	}
	*pResult = 1;		//˫��itemʱ����չ�����۵���Ϊ0��չ�����۵�
}


void CMy2Dlg::ShowPlot(BOOL b)
{
	GetDlgItem(IDC_PLOT)->ShowWindow(b);
	((CButton*)GetDlgItem(IDC_RADIO1))->SetCheck(b);
	((CButton*)GetDlgItem(IDC_RADIO2))->SetCheck(b);
	((CButton*)GetDlgItem(IDC_RADIO3))->SetCheck(b);
	GetDlgItem(IDC_RADIO1)->ShowWindow(b);
	GetDlgItem(IDC_RADIO2)->ShowWindow(b);
	GetDlgItem(IDC_RADIO3)->ShowWindow(b);
}


void CMy2Dlg::OnBnClickedRadio1()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if(m_plotchannel != 1)
	{
		m_plotchannel=1;
		m_Graph.ClearPloted();
	}
}

void CMy2Dlg::OnBnClickedRadio2()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if(m_plotchannel != 2)
	{
		m_plotchannel=2;
		m_Graph.ClearPloted();
	}
}

void CMy2Dlg::OnBnClickedRadio3()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if(m_plotchannel != 3)
	{
		m_plotchannel=3;
		m_Graph.ClearPloted();
	}
}

CString CMy2Dlg::GetIDName(const BYTE m,const WORD e)
{
	CString name,mbname,extname;

	if(1 == m)
		mbname.Format(_T("ĸ��:���ܰ�\n"));		//��������չģ�����Ʒֿ�
	else
		if(2 == m)
			mbname.Format(_T("ĸ��:��Դ��\n"));
		else
			mbname.Format(_T("ĸ��:0x%02x\n"),m);	//δ֪ĸ�����Ͳ�����

	//���ַ�����ģ������
	int i=0;
	int left=0,right=sizeof(g_IDHEX)/sizeof(g_IDHEX[0]);
	int midle=(left+right)/2;
	BOOL bfind=FALSE;
	while(!bfind && left<=right)
	{
		if(e == g_IDHEX[midle])
			bfind=TRUE;
		else 
		{
			if(e < g_IDHEX[midle])
				right=midle-1;
			else
				left=midle+1;

			midle=(left+right)/2;
		}
	}

	if(bfind)
		extname.Format(_T("��չģ��:%s"),g_IDSTR[midle]);
	else
		extname.Format(_T("��չģ��:0x%04x"),e);	//δ֪��չģ�����Ͳ�����

	name=mbname+extname;

	return name;
}


void CMy2Dlg::OnBnClickedBtnSample()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	ADControl adctrl;
	memset(&adctrl,0,sizeof(adctrl));
	adctrl.channel=0x0C;	
	adctrl.speed=500;	//Ĭ�ϲ�������500ms
	CString temp;
	GetDlgItem(IDC_BTN_SAMPLE)->GetWindowText(temp);
	if(temp == _T("��ʼ����"))		//Ĭ�Ͽ�ʼû���ϴ�����
	{
		adctrl.mode=2;
		GetDlgItem(IDC_BTN_SAMPLE)->SetWindowText(_T("ֹͣ����"));
	}
	else
	{
		adctrl.mode=0;
		GetDlgItem(IDC_BTN_SAMPLE)->SetWindowText(_T("��ʼ����"));
	}

#pragma pack(push)
#pragma pack(1)
	struct _ExtResData1
	{
		ExternResData msg;
		ADControl adctrl;
		BYTE sum;
	}msg1;
#pragma pack(pop)

	msg1.msg.head.header='@';
	msg1.msg.head.id=m_selectedid;
	msg1.msg.head.len=21;
	msg1.msg.head.type=DEXTENDRESMSGTYPE;
	msg1.msg.addr=m_selectednode;
	msg1.msg.extcode=m_extboardtype;
	msg1.adctrl=adctrl;
	msg1.sum=MyGetSum((BYTE*)&msg1,msg1.msg.head.len);
	m_timesocket.MySend(5000,&msg1,msg1.msg.head.len);
}

void CMy2Dlg::DoSensorFixData( const char*p )
{
	SensorFixMsg *t=(SensorFixMsg*)p;
	CString addr;
	addr.Format(_T("%d"),t->addr);
	HTREEITEM node=m_ptreeview->FindItem(TVI_ROOT,addr);
	if(node)
	{
		pNodeExtInfo pinfo=(pNodeExtInfo)m_ptreeview->GetItemData(node);
		if(pinfo)
		{
			int i=0;
			for(i=0;i<3;i++)
			{
				//�ҵ����л�δ��д�Ĳ��ֻ�������ͬģ��Ĳ��ֽ�����д
				if(pinfo->fixdata[i].moduletype == 0 || 
					pinfo->fixdata[i].moduletype == t->fixdata.moduletype)
				{
					pinfo->fixdata[i].moduletype=t->fixdata.moduletype;
					pinfo->fixdata[i].maxdata=t->fixdata.maxdata;
					pinfo->fixdata[i].zerodata=t->fixdata.zerodata;
					pinfo->fixdata[i].mindata=t->fixdata.mindata;
					break;
				}
			}
		}
	}
}