// 2Dlg.cpp : 实现文件
//

#include "stdafx.h"
#include "2.h"
#include "2Dlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


//数值大小应该递增，因为后面查找对应的模块名称时用了二分查找法
const WORD g_IDHEX[]={	0x1000,0x1001,0x1010,0x1011,0x1018,0x1019,0x1020,0x1030,0x1031,0x1032,
						0x1033,0x1040,0x1050,0x2000,0x2001,0x2010,0x2020,0x2100,0x2101,0x2102,
						0x2110,0x3000,0x3010,0x3020,0x3100
					 };
//顺序与g_IDHEX对应,以NULL结束
const TCHAR *g_IDSTR[100]={	_T("电压采集模块(单通道)"),			_T("电压采集模块(双通道)"),
							_T("电流采集模块(单通道)"),			_T("电流采集模块(双通道)"),
							_T("微电流传感器(单通道)"),			_T("微电流传感器(双通道)"),
							_T("光敏传感器(单通道)"),				_T("压力传感器模块"),
							_T("酒精传感器模块"),					_T("压力传感器模块+酒精传感器模块"),
							_T("压强传感器"),						_T("磁传感器"),
							_T("位移传感器"),						_T("温湿度传感器"),
							_T("温湿度传感器+光敏传感器(ADIN0)"),	_T("电压输出控制器"),
							_T("继电器输出控制器+GPIN传感器"),	_T("光电门传感器"),
							_T("红外输出控制器"),					_T("红外输出控制器+光电门传感器"),
							_T("超声波传感器"),					_T("RS232接口控制(无线节点)"),
							_T("RDID接口控制器"),					_T("高温传感器"),
							_T("RS232接口控制(协调器)"),			NULL
};

// CMy2Dlg 对话框

CMy2Dlg::CMy2Dlg(CWnd* pParent /*=NULL*/)
	: CDialog(CMy2Dlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

CMy2Dlg::~CMy2Dlg()
{
	if(m_timesocket.m_hSocket != INVALID_SOCKET)
	{
		WriteLog(_T("关闭连接\r\n"));
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


// CMy2Dlg 消息处理程序

BOOL CMy2Dlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
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

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
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

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
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

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CMy2Dlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标显示。
//
HCURSOR CMy2Dlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CMy2Dlg::OnOK()
{
	// TODO: 在此添加专用代码和/或调用基类

	//CDialog::OnOK();
}

void CMy2Dlg::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	//OnOK();

	CDialog::OnOK();
}

void CMy2Dlg::OnBnClickedBtnConnect()
{
	// TODO: 在此添加控件通知处理程序代码
	CString temp;
	GetDlgItem(IDC_BTN_CONNECT)->GetWindowText(temp);
	if(temp == _T("连接"))	//连接
	{
		INT_PTR nResponse = m_connectdlg.DoModal();
		if(nResponse == IDOK)
		{
			m_connectaddr=m_connectdlg.m_connectaddr;
			m_connectportstr=m_connectdlg.m_connectport;
			m_connectport=_tcstoul(m_connectportstr,NULL,10);
			if(BeginConnectToSer())
				//GetDlgItem(IDC_BTN_CONNECT)->EnableWindow(FALSE);
				GetDlgItem(IDC_BTN_CONNECT)->SetWindowText(_T("断开连接"));
		}
	}
	else					//断开连接
	{
		m_ptreeview->MyDeleteItem(_T("0"));	
		KillTimer(HEARTBEATTOSERID);
		KillTimer(NODEPARSONINFOID);
		if(m_timesocket.m_hSocket != INVALID_SOCKET)
		{
			WriteLog(_T("关闭连接\r\n"));
			m_timesocket.Close();
		}
		GetDlgItem(IDC_BTN_CONNECT)->SetWindowText(_T("连接"));
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
			WriteLog(_T("不能连接到服务器，自动重试"));
		else
			break;
	}
	if(r)
	{
		WriteLog(_T("成功连接到服务器"));
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
		while( r == -1 && (GetTickCount()-starttime) < 10000)	//要求多少秒内收到应用识别应答，这里用10秒
		{
			m_timesocket.MySend(5000,&appidentmsg,appidentmsg.head.len);
			left=m_timesocket.MyReceive(5000,m_sbuffer,SOCKETBUFFERSIZE);
			while(left >=sizeof(MsgHeader))				//一次有可能收到多个包，且有可能不是所需的应答包；当然，刚开始连接，应该不至于，但还是处理一下
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
			if(pcommack->ack == 0)		//应用识别应答代码，00正确接收
			{
				WriteLog(_T("与服务器连接完成并收到服务器对'应用识别消息'的成功应答"));
				MessageBox(_T("与服务器成功建立通迅"));
				SetTimer(HEARTBEATTOSERID,HEARTBEATINTERBALTOSER*1000,NULL);
				receivelen=0;
				leftlen=0;
				m_heartbeatfromser=GetTickCount();
				return TRUE;
			}
			else						//应用识别应答代码，未正确接收，处理...
			{
				WriteLog(_T("收到服务器对'应用识别消息'的错误应答"));
				MessageBox(_T("收到服务器对'应用识别消息'的错误应答"));
			}
		}
		else
		{
			MessageBox(_T("不能收到服务器应答"));
			WriteLog(_T("连接后接收不到服务器对'应用识别消息'的应答"));
		}
	}
	else
	{
		MessageBox(_T("不能连接到服务器，关闭连接"));
		WriteLog(_T("不能连接到服务器，关闭连接"));
		m_timesocket.Close();
	}

	return FALSE;
}

void CMy2Dlg::MyOnReceive()
{
	CString addr;		//节点地址
	CString paraddr;	//父节点地址
	CString temp;		//临时变量，用完后即清空
	DevConnectMsg *devconmsg;
	DevDisConnectMsg *devdisconmsg;
	WORD *ids;
	int idnums,i;
	CommACKMsg ack;			//一般应答
	NodeCommMsg nodecom;	//节点一般应答
	pNodeExtInfo nodedata=0;

	nodecom.head.header='@';
	nodecom.head.len=8;
	ack.head.header='@';
	ack.head.len=7;
	ack.head.id=0;

	m_pcstemp=m_pcstemp2=m_sbuffer;		//m_pcstemp2为当前处理包的基址，m_pcstemp为当前处理数据的指针
	MsgHeader *pmsgheader;
	MoveMemory(m_sbuffer,m_sbuffer+receivelen-leftlen,leftlen);
	receivelen=m_timesocket.Receive(m_sbuffer+leftlen,SOCKETBUFFERSIZE-leftlen);
	leftlen+=receivelen;
	receivelen=leftlen;
	pmsgheader=(MsgHeader*)m_pcstemp;
	while(leftlen >= sizeof(MsgHeader) && leftlen >= pmsgheader->len)	//一次可能收到多个包，判断是否还有
	{
		if('@' != pmsgheader->header)
		{
			//不是一个合法的包
		}
		else
		{
			if(MyGetSum((BYTE*)m_pcstemp,pmsgheader->len) != (BYTE)*(m_pcstemp+pmsgheader->len-1))
			{
				//校验和不正确,处理...
			}
			switch(pmsgheader->type)
			{
			case HEARTBEATMSGTYPE:		//心跳消息
				CommMsg *commmsg;
				commmsg=(CommMsg*)m_pcstemp;

				if( GetTickCount()-m_heartbeatfromser > HEARTBEATOVERFROMSER)
					m_bheartbeatfromserover=TRUE;		//超时
				else
					m_bheartbeatfromserover=FALSE;

				m_heartbeatfromser=GetTickCount();
				break;

			case DEVCONNECTMSGTYPE:		//设备与服务器连接消息
				devconmsg=(DevConnectMsg*)m_pcstemp;
				ids=(WORD*)(((BYTE*)devconmsg)+5);
				idnums=(devconmsg->head.len-6)/2;
				for(i=0;i<idnums;i++)
				{
					temp.Format(_T("%d"),i);
					m_ptreeview->MyInsertItem(TVI_ROOT,temp);		//只与一个协调器通信，不用会话ID，协调器地址为0
					temp.Empty();
					temp.Format(_T("设备与服务器连接,id为%d"),devconmsg->devsesionid);
					m_selectedid=devconmsg->devsesionid;			//保存会话ID
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
		
			case DEVDISCONNECTMSGTYPE:	//设备与服务器断开连接消息
				devdisconmsg=(DevDisConnectMsg*)m_pcstemp;
				m_ptreeview->MyDeleteItem(_T("0"));	//由于只与一个协调器通信，断开时直接清空树
				m_ptreeview->Invalidate();
				temp.Format(_T("设备与服务器断开,id为%d"),devconmsg->devsesionid);
				m_selectedid=0;
				WriteLog((LPTSTR)(LPCTSTR)temp);
				temp.Empty();
				break;

			case UNODEPARSONMSGTYPE:		//节点父子关系消息
				NodeParsonMsg *nodeparsonmsg;
				nodeparsonmsg=(NodeParsonMsg*)m_pcstemp;
				addr.Format(_T("%d"),nodeparsonmsg->NodeAddr);
				paraddr.Format(_T("%d"),nodeparsonmsg->ParNodeAddr);

				//只与一个协调器通信，直接在父节点下加子节点
				HTREEITEM par,cor,newh;
				cor=m_ptreeview->FindItem(TVI_ROOT,_T("0"));	
				if(cor != NULL)
				{
					par=m_ptreeview->FindItem(cor,paraddr);
// 					if(!par)			//若找不到父节点，则添加到协调器下
// 						par=cor;
					if(par != NULL)		//只在父子关系中的父节点下添加
					{
						newh=m_ptreeview->MyInsertItem(par,addr);
						if(newh)
						{
							temp.Format(_T("在节点%s后增加节点%s"),paraddr,addr);
							WriteLog((LPTSTR)(LPCTSTR)temp);
							temp.Empty();
							Invalidate();
							nodecom.head.id=nodeparsonmsg->head.id;
							nodecom.head.type=GETNODEINFOMSGTYPE;
							nodecom.addr=nodeparsonmsg->NodeAddr;
							nodecom.sum=MyGetSum((BYTE*)&nodecom,nodecom.head.len);
							m_timesocket.MySend(5000,&nodecom,nodecom.head.len);
						}
						else		//原有节点，保持父子关系
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
	
			case UNODEINFOMSGTYPE:		//节点信息消息
				DoNodeInfoData(m_pcstemp);
				break;

			case USENSORFIXMSGTYPE:		//传感器参数校准消息
				DoSensorFixData(m_pcstemp);
				break;

			case UEXTENDRESMSGTYPE:		//上传扩展模块资源数据消息
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
	// TODO: 在此添加消息处理程序代码和/或调用默认值

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

	//父子节点关系是否超时
	if(NODEPARSONINFOID == nIDEvent)
	{
		m_ptreeview->TreeVisitComput(_T("0"));
	}

	if(m_bheartbeatfromserover || GetTickCount()-m_heartbeatfromser > HEARTBEATOVERFROMSER)
	{
		m_bheartbeatfromserover=FALSE;
		WriteLog(_T("接收服务器心跳超时，关闭连接\r\n"));
		m_timesocket.Close();
		KillTimer(HEARTBEATTOSERID);
		KillTimer(NODEPARSONINFOID);
		MessageBox(_T("接收服务器心跳超时，自动关闭当前连接并重新连接"));
		WriteLog(_T("重新连接\r\n"));
		if(BeginConnectToSer())
			GetDlgItem(IDC_BTN_CONNECT)->EnableWindow(FALSE);
		else
			MessageBox(_T("不能连接到服务器，请重试"));
	}

	CDialog::OnTimer(nIDEvent);
}


//节点信息消息处理
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
		if(olddata)	//若原来已分配内存，则直接用原来分配的内存地址，否则新分配内存
		{
			nodeextinfo=(NodeExtInfo*)olddata;
		}
		else
		{
			nodeextinfo=(NodeExtInfo*)malloc(sizeof(NodeExtInfo));	//在堆中分配一块内存保存节点的IEEE,LQI,RSSI,PANID
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

//处理上传的扩展模块资源数据消息
void CMy2Dlg::DoExternData(const char* p)
{
	ExternResData *t=(ExternResData*)p;
	if(m_selectednode == t->addr)		//只有符合的id和node的数据才处理
	{
		UINT datanums=(t->head.len-10)/2,i=0;		//实际数据的个数
		WORD *data=(WORD*)(((BYTE*)t)+9);			//指向实际的数据
		BYTE *tdata;
		WORD dt1,dt2,dt3;
		float ft1,ft2,ft3;
		int light;
		CString datastr;
		//...处理扩展模块资源
		switch(t->extcode)
		{
		case 0X1000:		//单通道电压采集
			for(i=0;i<datanums;i++)
			{
				dt1=*(data+i);
				ft1=ComputeVoltage(dt1,m_nodeextinfo->fixdata[0].maxdata,
					m_nodeextinfo->fixdata[0].zerodata,m_nodeextinfo->fixdata[0].mindata);
				m_Graph.AppendPoint(ft1);
				datastr.Format(_T("电压采集数据 %3.2fV"),ft1);
				GetDlgItem(IDC_DATA)->SetWindowText(datastr);
			}
			break;

		case 0x1001:		//双通道电压采集
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
				datastr.Format(_T("电压 1通道:%3.2fV 2通道:%3.2fV"),ft1,ft2);
				GetDlgItem(IDC_DATA)->SetWindowText(datastr);
			}
			break;

		case 0X1010:		//单通道电流采集
			for(i=0;i<datanums;i++)
			{
				dt1=*(data+i);
				ft1=ComputeCurrent(dt1,m_nodeextinfo->fixdata[0].maxdata,
					m_nodeextinfo->fixdata[0].zerodata);
				m_Graph.AppendPoint(ft1);
				datastr.Format(_T("电流 %3.2fmA"),ft1);
				GetDlgItem(IDC_DATA)->SetWindowText(datastr);
			}
			break;

		case 0x1011:		//双通道电流采集
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
				datastr.Format(_T("电流 1通道:%3.2fmA 2通道:%3.2fmA"),ft1,ft2);
				GetDlgItem(IDC_DATA)->SetWindowText(datastr);
			}
			break;

		case 0X1020:		//光敏传感器
			for(i=0;i<datanums;i++)
			{
				dt1=*(data+i);
				light=ComputeLight(dt1);
				m_Graph.AppendPoint(light);
				datastr.Format(_T("光敏 %dLX"),light);
				GetDlgItem(IDC_DATA)->SetWindowText(datastr);
			}
			break;

		case 0X1030:		//压力传感器
			for(i=0;i<datanums;i++)
			{
				dt1=*(data+i);
				m_Graph.AppendPoint(dt1);
				datastr.Format(_T("压力 %dg"),dt1);
				GetDlgItem(IDC_DATA)->SetWindowText(datastr);
			}
			break;

		case 0X1031:		//酒精传感器
			for(i=0;i<datanums;i++)
			{
				dt1=*(data+i);
				m_Graph.AppendPoint(dt1);
				datastr.Format(_T("酒精 %d"),dt1);
				GetDlgItem(IDC_DATA)->SetWindowText(datastr);
			}
			break;

		case 0X1032:		//压力酒精传感器模块
			for(i=0;i<datanums;i+=2)
			{
				dt1=*(data+i);
				dt2=*(data+i+1);
				if(m_plotchannel == 1)
					m_Graph.AppendPoint(dt1);
				else
					if(m_plotchannel == 2)
						m_Graph.AppendPoint(dt2);
				datastr.Format(_T("压力:%dg 酒精:%d"),dt1,dt2);
				GetDlgItem(IDC_DATA)->SetWindowText(datastr);
			}
			break;

		case 0X2000:		//温湿度传感器
			for(i=0;i<datanums;i+=2)
			{
				dt1=*(data+i);
				dt2=*(data+i+1);
				if(m_plotchannel == 1)
					m_Graph.AppendPoint(dt1);
				else
					if(m_plotchannel == 2)
						m_Graph.AppendPoint(dt2);
				datastr.Format(_T("温度:%d℃ 湿度:%d%%"),dt1,dt2);
				GetDlgItem(IDC_DATA)->SetWindowText(datastr);
			}
			break;

		case 0X2001:		//温湿度传感器,光敏
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
				datastr.Format(_T("温度:%d℃ 湿度:%d%% 光敏:%dLX"),dt1,dt2,light);
				GetDlgItem(IDC_DATA)->SetWindowText(datastr);
			}
			break;

		case 0x2010:		//电压输出控制器
			GetDlgItem(IDC_DATA)->SetWindowText(_T(""));
			break;

		case 0x2020:		//继电器输出控制器
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

		case 0X2100:		//光电门传感器
			for(i=0;i<datanums;i++)
			{
				dt1=*(data+i);
				m_Graph.AppendPoint(dt1);
				datastr.Format(_T("光电门 %d"),dt1);
				GetDlgItem(IDC_DATA)->SetWindowText(datastr);
			}
			break;

		case 0X2110:		//超声波传感器
			for(i=0;i<datanums;i++)
			{
				dt1=*(data+i);
				m_Graph.AppendPoint(dt1);
				datastr.Format(_T("超声波 %d"),dt1);
				GetDlgItem(IDC_DATA)->SetWindowText(datastr);
			}
			break;

		case 0x3000:		//RS232接口控制（无线节点）
			break;

		default:
			;
		}
	}
}


//处理在树形控件中双击
void CMy2Dlg::OnNMDblclkTreeNode(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: 在此添加控件通知处理程序代码
	HTREEITEM t1 = m_ptreeview->GetSelectedItem();	
	if(t1 == NULL) return ;

	CString ttstr;
	ttstr.Format(_T("%d"),m_selectednode);
	HTREEITEM tt=m_ptreeview->FindItem(TVI_ROOT,ttstr);
	m_ptreeview->SetItemBold(tt,FALSE);	//取消之前选中节点的粗体显示
	m_ptreeview->SetItemBold(t1,TRUE);	//当前所选中的节点名称以粗体显示
	DWORD data=0;
	data=m_ptreeview->GetItemData(t1);
	if(data)
	{
 		m_selectednode=0xFFFF & _tcstoul(m_ptreeview->MyGetItemText(t1),NULL,10);

		CString S1;
		S1.Format(_T("节点地址:%d,IEEE地址:0x%08X%08X,RSSI:%d,LQI:%d,PANID:%d"),
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
		GetDlgItem(IDC_BTN_SAMPLE)->SetWindowText(_T("开始采样"));

		switch(m_extboardtype)
		{
		case 0:			//无扩展模块
			GetDlgItem(IDC_BTN_SAMPLE)->ShowWindow(FALSE);
			break;

		case 0x1001:	//双通道电压采集
			GetDlgItem(IDC_PLOT)->ShowWindow(TRUE);
			GetDlgItem(IDC_RADIO1)->ShowWindow(TRUE);
			GetDlgItem(IDC_RADIO2)->ShowWindow(TRUE);
			GetDlgItem(IDC_RADIO1)->SetWindowText(_T("电压1通道"));
			GetDlgItem(IDC_RADIO2)->SetWindowText(_T("电压2通道"));
			((CButton*)GetDlgItem(IDC_RADIO1))->SetCheck(TRUE);
			break;

		case 0x1011:	//双通道电流采集
			GetDlgItem(IDC_PLOT)->ShowWindow(TRUE);
			GetDlgItem(IDC_RADIO1)->ShowWindow(TRUE);
			GetDlgItem(IDC_RADIO2)->ShowWindow(TRUE);
			GetDlgItem(IDC_RADIO1)->SetWindowText(_T("电流1通道"));
			GetDlgItem(IDC_RADIO2)->SetWindowText(_T("电流2通道"));
			((CButton*)GetDlgItem(IDC_RADIO1))->SetCheck(TRUE);
			break;

		case 0x1019:	//双通道微电流传感器
			GetDlgItem(IDC_PLOT)->ShowWindow(TRUE);
			GetDlgItem(IDC_RADIO1)->ShowWindow(TRUE);
			GetDlgItem(IDC_RADIO2)->ShowWindow(TRUE);
			GetDlgItem(IDC_RADIO1)->SetWindowText(_T("微电流1通道"));
			GetDlgItem(IDC_RADIO2)->SetWindowText(_T("微电流2通道"));
			((CButton*)GetDlgItem(IDC_RADIO1))->SetCheck(TRUE);
			break;
		
		case 0x1032:	//压力、酒精传感器
			GetDlgItem(IDC_PLOT)->ShowWindow(TRUE);
			GetDlgItem(IDC_RADIO1)->ShowWindow(TRUE);
			GetDlgItem(IDC_RADIO2)->ShowWindow(TRUE);
			GetDlgItem(IDC_RADIO1)->SetWindowText(_T("压力数据"));
			GetDlgItem(IDC_RADIO2)->SetWindowText(_T("酒精数据"));
			((CButton*)GetDlgItem(IDC_RADIO1))->SetCheck(TRUE);
			break;

		case 0x2000:	//温湿度传感器
			GetDlgItem(IDC_PLOT)->ShowWindow(TRUE);
			GetDlgItem(IDC_RADIO1)->ShowWindow(TRUE);
			GetDlgItem(IDC_RADIO2)->ShowWindow(TRUE);
			GetDlgItem(IDC_RADIO1)->SetWindowText(_T("温度数据"));
			GetDlgItem(IDC_RADIO2)->SetWindowText(_T("湿度数据"));
			((CButton*)GetDlgItem(IDC_RADIO1))->SetCheck(TRUE);
			break;

		case 0x2001:	//温湿度、光敏
			GetDlgItem(IDC_PLOT)->ShowWindow(TRUE);
			GetDlgItem(IDC_RADIO1)->ShowWindow(TRUE);
			GetDlgItem(IDC_RADIO2)->ShowWindow(TRUE);
			GetDlgItem(IDC_RADIO3)->ShowWindow(TRUE);
			GetDlgItem(IDC_RADIO1)->SetWindowText(_T("温度数据"));
			GetDlgItem(IDC_RADIO2)->SetWindowText(_T("湿度数据"));
			GetDlgItem(IDC_RADIO3)->SetWindowText(_T("光敏数据"));
			((CButton*)GetDlgItem(IDC_RADIO1))->SetCheck(TRUE);
			break;

		case 0x2010:	//电压输出控制器
			GetDlgItem(IDC_BTN_SAMPLE)->ShowWindow(FALSE);
			break;

		case 0x2020:	//继电器与GPIN模块
			GetDlgItem(IDC_BTN_SAMPLE)->ShowWindow(FALSE);
			break;

		case 0x2101:	//红外输出控制器
		case 0x2102:	//红外输出控制器+光电门传感器
		case 0x3000:	//RS232接口控制(无线节点)
		case 0x3010:	//RDID接口控制器
		case 0x3020:	//高温传感器
		case 0x3100:	//RS232接口控制(协调器)
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
	*pResult = 1;		//双击item时，不展开或折叠；为0则展开或折叠
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
	// TODO: 在此添加控件通知处理程序代码
	if(m_plotchannel != 1)
	{
		m_plotchannel=1;
		m_Graph.ClearPloted();
	}
}

void CMy2Dlg::OnBnClickedRadio2()
{
	// TODO: 在此添加控件通知处理程序代码
	if(m_plotchannel != 2)
	{
		m_plotchannel=2;
		m_Graph.ClearPloted();
	}
}

void CMy2Dlg::OnBnClickedRadio3()
{
	// TODO: 在此添加控件通知处理程序代码
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
		mbname.Format(_T("母板:智能板\n"));		//换行与扩展模块名称分开
	else
		if(2 == m)
			mbname.Format(_T("母板:电源板\n"));
		else
			mbname.Format(_T("母板:0x%02x\n"),m);	//未知母板类型不解释

	//二分法查找模块名称
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
		extname.Format(_T("扩展模块:%s"),g_IDSTR[midle]);
	else
		extname.Format(_T("扩展模块:0x%04x"),e);	//未知扩展模块类型不解释

	name=mbname+extname;

	return name;
}


void CMy2Dlg::OnBnClickedBtnSample()
{
	// TODO: 在此添加控件通知处理程序代码
	ADControl adctrl;
	memset(&adctrl,0,sizeof(adctrl));
	adctrl.channel=0x0C;	
	adctrl.speed=500;	//默认采样周期500ms
	CString temp;
	GetDlgItem(IDC_BTN_SAMPLE)->GetWindowText(temp);
	if(temp == _T("开始采样"))		//默认开始没有上传数据
	{
		adctrl.mode=2;
		GetDlgItem(IDC_BTN_SAMPLE)->SetWindowText(_T("停止采样"));
	}
	else
	{
		adctrl.mode=0;
		GetDlgItem(IDC_BTN_SAMPLE)->SetWindowText(_T("开始采样"));
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
				//找到其中还未填写的部分或者是相同模块的部分进行填写
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