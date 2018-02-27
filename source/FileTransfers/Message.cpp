#include "stdafx.h"
#include "Message.h"

//Ĭ�ϵĹ��캯��
CMessage::CMessage()
{
	m_nType = -1;

	m_strFileName = _T("");
	m_dwFileSize = 0;
}
 
//ֻ�跢��һ��Ŀ�����Ϣ��ʹ��
CMessage::CMessage(int nType)
{
	m_nType = nType;
	
	m_strFileName = _T("");
	m_dwFileSize = 0;
}

//��Ҫ�����ļ�������Сʱʹ��
CMessage::CMessage(int nType, CString strFileName, DWORD dwFileSize)
{
	m_nType = nType;
	
	m_strFileName = strFileName;
	m_dwFileSize = dwFileSize;
}

CMessage::~CMessage()
{
}

void CMessage::Serialize(CArchive &ar)
{
	if (ar.IsStoring())
	{
		ar << m_nType;

		ar << m_strFileName;
		ar << m_dwFileSize;
	}
	else
	{
		ar >> m_nType;

		ar >> m_strFileName;
		ar >> m_dwFileSize;
	}
}