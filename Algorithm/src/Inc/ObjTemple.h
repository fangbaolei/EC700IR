#ifndef _OBJTEMPLE_INCLUDED__
#define _OBJTEMPLE_INCLUDED__

// 模版匹配数据结构

#include "swimageobj.h"
#include "hvbaseobj.h"

using namespace HiVideo;

#define MAX_OBJMODEL_WIDTH		300			// 模型最大宽度（象素）
#define MAX_OBJMODEL_HEIGHT		200			// 模型最大高度（象素）

class CObjTemple
{
public:
	CObjTemple();
	~CObjTemple();

	HRESULT UpdateTemple(HV_COMPONENT_IMAGE *pImg, bool fFastMem = FALSE);
	HRESULT CalcMeanVar();			// 计算模型参数

	void Clear();

	IReferenceComponentImage *m_pObjImg;		// 模型数据

	float m_fltVar;			// 模型Var
	float m_fltMean;		// 模型Mean

protected:
	int m_nWidth;			// 模型宽度
	int m_nHeight;			// 模型高度
private:
	CFastMemAlloc m_cFastStack;

};

#endif // _OBJTEMPLE_INCLUDED__
