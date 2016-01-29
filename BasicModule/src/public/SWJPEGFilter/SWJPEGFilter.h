#ifndef __SW_JPEG_FILTER_H__
#define __SW_JPEG_FILTER_H__
#include "SWBaseFilter.h"

extern "C"
{
	struct jpg_error_mgr
	{
		struct jpeg_error_mgr pub;	/* "public" fields */
		jmp_buf setjmp_buffer;		/* for return to caller */
		char buffer[1024];		/* error message <CSC>*/
	};
	typedef jpg_error_mgr *jpg_error_ptr;
	void ima_jpeg_error_exit (j_common_ptr cinfo);
	void InitDestination(j_compress_ptr cinfo);
	boolean EmptyOutputBuffer(j_compress_ptr cinfo);
	void TermDestination(j_compress_ptr cinfo);
	void InitSource(j_decompress_ptr cinfo);
	boolean FillInputBuffer(j_decompress_ptr cinfo);
	void SkipInputData(j_decompress_ptr cinfo, long num_bytes);
	void TermSource(j_decompress_ptr /*cinfo*/);
};

class CSWJPEGFilter : public CSWBaseFilter
{
	CLASSINFO(CSWJPEGFilter, CSWBaseFilter)
public:
	CSWJPEGFilter();
	virtual ~CSWJPEGFilter();
protected:
	virtual HRESULT Receive(CSWObject* obj);	
	CSWImage* Decompress(CSWImage *pImage);
private:
	struct jpeg_decompress_struct decompress_info;
	struct jpg_error_mgr decompress_jerr;
	BOOL   m_bInitDecompress;

	struct jpeg_compress_struct compress_info;
	struct jpg_error_mgr compress_jerr;
	BOOL   m_bInitCompress;
	
	CSWMutex  m_cMutex;
	CSWImage* m_pYUVImage;
};
REGISTER_CLASS(CSWJPEGFilter)
#endif