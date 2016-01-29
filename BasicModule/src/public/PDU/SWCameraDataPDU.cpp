#include "SWFC.h"
#include "SWCameraDataPDU.h"

CSWCameraDataPDU::CSWCameraDataPDU()
{
	m_dwLightType = 0;
	m_dwCplStatus = 0;
	m_dwPluseLevel = 0;
	m_nAvgPlateY = 0;
	m_nWDRLevel = 0;
}

CSWCameraDataPDU::~CSWCameraDataPDU()
{
}

