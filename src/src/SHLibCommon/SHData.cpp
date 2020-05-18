#include <SHData.h>
#include <string.h>

CSHData::CSHData(void * data, int size) 
{
	m_dataSize = size;
	SetMaxSize(size  );
	if ( data != NULL )
		SetData(data, size );
}
CSHData::CSHData(int maxSize) 
{
	SetDataSize( maxSize );
	SetMaxSize(maxSize);
	Zero();	
}
bool CSHData::SetData(CSHData &data )
{	
	SetMaxSize( data.GetMaxSize() );
	return SetData(data.GetData(),data.GetDataSize());
}
CSHData::~CSHData()
{
}
int CSHData::GetMaxSize()
{
    return m_maxSize;
}

bool CSHData::SetMaxSize(int size)
{
	bool ret = false;
	if ( size <= SH_MAX_BUF )
	{
		ret = true;
		m_maxSize = size;
	}
	else
		m_maxSize= SH_MAX_BUF;
	return ret;
}

char * CSHData::GetData()
{
	return m_data;
}
int CSHData::GetDataSize()
{   
	return m_dataSize;
}
bool CSHData::SetData(void * data, int len)
{
	bool ret = false;
	if ( data != NULL )
	{
		if ( SetDataSize(len) )
		{
			memcpy(m_data,data,GetDataSize());
			ret = true;
		}
	}
	return ret;
}
bool CSHData::SetDataSize(int size) 
{
	bool ret = false;
	if ( size <= SH_MAX_BUF )
	{
		ret = true;
		m_dataSize = size;
	}
	else
		m_dataSize= SH_MAX_BUF;
	return ret;
}
void CSHData::Zero()
{
	memset(m_data,0,SH_MAX_BUF);
}
