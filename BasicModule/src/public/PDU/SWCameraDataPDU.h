#ifndef __SW_CAMERA_DATA_PDU_H__
#define __SW_CAMERA_DATA_PDU_H__
class CSWCameraDataPDU : public CSWObject
{
	CLASSINFO(CSWCameraDataPDU, CSWObject)
public:
	CSWCameraDataPDU();
	virtual ~CSWCameraDataPDU();
	void SetLightType(DWORD dwLightType){m_dwLightType = dwLightType;}
	DWORD GetLightType(void){return m_dwLightType;}
	
	void SetCplStatus(DWORD dwCplStatus){m_dwCplStatus = dwCplStatus;}
	DWORD GetCplStatus(void){return m_dwCplStatus;}
	
	void SetPluseLevel(DWORD dwLevel){m_dwPluseLevel = dwLevel;}
	DWORD GetPluseLevel(void){return m_dwPluseLevel;}

	void SetAvgPlateY(int nPlateY) {m_nAvgPlateY = nPlateY;}
	int GetAvgPlateY() {return m_nAvgPlateY;}

	void SetWDRLevel(int nWDRLevel) {m_nWDRLevel = nWDRLevel;}
	int GetWDRLevel() {return m_nWDRLevel;}
private:
	DWORD m_dwLightType;	//相机等级
	DWORD m_dwCplStatus;	//偏光镜状态
	DWORD m_dwPluseLevel;	//频闪补光脉宽等级
	int m_nAvgPlateY;    //平均车牌亮度
	int m_nWDRLevel;    //WDR等级
};
#endif
