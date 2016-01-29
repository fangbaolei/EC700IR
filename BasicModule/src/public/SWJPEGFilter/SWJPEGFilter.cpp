#include "SWFC.h"
extern "C"
{
	#include <setjmp.h>
	#include "jpeglib.h"
	#include "jerror.h"
}
#include "SWJPEGFilter.h"

enum { eBufSize = 4096 };
class CJPGBuffer : public jpeg_destination_mgr, public jpeg_source_mgr
{
public:        
	CJPGBuffer(BYTE * bImage, int & size): jpgImage(bImage),jpgSize(size),jpgRWSize(size)
	{
		jpgRWSize = 0;

		init_destination = InitDestination;
		empty_output_buffer = EmptyOutputBuffer;
		term_destination = TermDestination;

		init_source = InitSource;
		fill_input_buffer = FillInputBuffer;
		skip_input_data = SkipInputData;
		resync_to_restart = jpeg_resync_to_restart; // use default method
		term_source = TermSource;

		next_input_byte = NULL; //* => next byte to read from buffer
		bytes_in_buffer = 0;	//* # of bytes remaining in buffer

		m_pBuffer = new BYTE[eBufSize];
	}
	virtual ~CJPGBuffer()
	{
		delete [] m_pBuffer;
	}        
	int Read(BYTE * buffer,int size)
	{
		int readSize = size;
		if(readSize > jpgSize - jpgRWSize)
		{
			readSize = jpgSize - jpgRWSize;
		}
		
		if(readSize)
		{
			memcpy(buffer, jpgImage + jpgRWSize, readSize);
			jpgRWSize += readSize;
		}
		return readSize;
	}
	int Write(BYTE * buffer,int size)
	{
		int writeSize = size;
		if(writeSize > jpgSize - jpgRWSize)
		{
			writeSize = jpgSize - jpgRWSize;
		}
		
		if(writeSize)
		{
			memcpy(jpgImage+jpgRWSize, buffer, writeSize);
			jpgRWSize += writeSize;
		}
		return writeSize;
	}
	BYTE * jpgImage;
	int    jpgSize;
	int  & jpgRWSize;
	BYTE * m_pBuffer;
	bool m_bStartOfFile;
};

extern "C"
{
	void ima_jpeg_error_exit (j_common_ptr cinfo)
	{
		/* cinfo->err really points to a my_error_mgr struct, so coerce pointer */
		jpg_error_ptr myerr = (jpg_error_ptr) cinfo->err;
		/* Create the message */
		myerr->pub.format_message (cinfo, myerr->buffer);
		/* Send it to stderr, adding a newline */
		/* Return control to the setjmp point */
		longjmp(myerr->setjmp_buffer, 1);
	}
	void InitDestination(j_compress_ptr cinfo)
	{
		CJPGBuffer* pDest = (CJPGBuffer*)cinfo->dest;
		pDest->next_output_byte = pDest->m_pBuffer;
		pDest->free_in_buffer = eBufSize;
	}

	boolean EmptyOutputBuffer(j_compress_ptr cinfo)
	{
		CJPGBuffer* pDest = (CJPGBuffer*)cinfo->dest;
		//if (pDest->m_pFile->Write(pDest->m_pBuffer,1,eBufSize)!=(size_t)eBufSize)
		if(pDest->Write(pDest->m_pBuffer,eBufSize)!=(size_t)eBufSize)
		{
			ERREXIT(cinfo, JERR_FILE_WRITE);
		}

		pDest->next_output_byte = pDest->m_pBuffer;
		pDest->free_in_buffer = eBufSize;
		return true;
	}

	void TermDestination(j_compress_ptr cinfo)
	{
		CJPGBuffer* pDest = (CJPGBuffer*)cinfo->dest;
		size_t datacount = eBufSize - pDest->free_in_buffer;
		/* Write any data remaining in the buffer */
		if (datacount > 0) {
			//if (!pDest->m_pFile->Write(pDest->m_pBuffer,1,datacount))
			if(!pDest->Write(pDest->m_pBuffer,datacount))
			{
				ERREXIT(cinfo, JERR_FILE_WRITE);
			}
		}
		//pDest->m_pFile->Flush();
		/* Make sure we wrote the output file OK */
		//if (pDest->m_pFile->Error()) ERREXIT(cinfo, JERR_FILE_WRITE);
		return;
	}

	void InitSource(j_decompress_ptr cinfo)
	{
		CJPGBuffer* pSource = (CJPGBuffer*)cinfo->src;
		pSource->m_bStartOfFile = true;
	}

	boolean FillInputBuffer(j_decompress_ptr cinfo)
	{
		size_t nbytes;
		CJPGBuffer* pSource = (CJPGBuffer*)cinfo->src;
		//nbytes = pSource->m_pFile->Read(pSource->m_pBuffer,1,eBufSize);
		nbytes = pSource->Read(pSource->m_pBuffer, eBufSize);
		if (nbytes <= 0){
			if (pSource->m_bStartOfFile)	//* Treat empty input file as fatal error
			{
				ERREXIT(cinfo, JERR_INPUT_EMPTY);
			}
			WARNMS(cinfo, JWRN_JPEG_EOF);
			// Insert a fake EOI marker
			pSource->m_pBuffer[0] = (JOCTET) 0xFF;
			pSource->m_pBuffer[1] = (JOCTET) JPEG_EOI;
			nbytes = 2;
		}
		pSource->next_input_byte = pSource->m_pBuffer;
		pSource->bytes_in_buffer = nbytes;
		pSource->m_bStartOfFile = false;
		return true;
	}

	void SkipInputData(j_decompress_ptr cinfo, long num_bytes)
	{
		CJPGBuffer* pSource = (CJPGBuffer*)cinfo->src;
		if (num_bytes > 0){
			while (num_bytes > (long)pSource->bytes_in_buffer){
				num_bytes -= (long)pSource->bytes_in_buffer;
				FillInputBuffer(cinfo);
				// note we assume that fill_input_buffer will never return false,
				// so suspension need not be handled.
			}
			pSource->next_input_byte += (size_t) num_bytes;
			pSource->bytes_in_buffer -= (size_t) num_bytes;
		}
	}

	void TermSource(j_decompress_ptr /*cinfo*/)
	{
		return;
	}
}

CSWJPEGFilter::CSWJPEGFilter():CSWBaseFilter(2, 1)
{
	GetIn(0)->AddObject(CLASSID(CSWImage));
	GetIn(1)->AddObject(CLASSID(CSWImage));
	GetOut(0)->AddObject(CLASSID(CSWImage));
	swpa_memset(&decompress_info, 0, sizeof(decompress_info));
	swpa_memset(&decompress_jerr, 0, sizeof(decompress_jerr));
	decompress_info.err = jpeg_std_error(&decompress_jerr.pub);
	decompress_jerr.pub.error_exit = ima_jpeg_error_exit;
	m_bInitDecompress = FALSE;

	swpa_memset(&compress_info, 0, sizeof(compress_info));
	swpa_memset(&compress_jerr, 0, sizeof(compress_jerr));
	compress_info.err = jpeg_std_error(&compress_jerr.pub);
	compress_jerr.pub.error_exit = ima_jpeg_error_exit;
	m_bInitCompress = FALSE;
	
	m_pYUVImage = NULL;
}

CSWJPEGFilter::~CSWJPEGFilter()
{
	if(m_bInitDecompress)
	{
		jpeg_destroy_decompress(&decompress_info);
	}
	if(m_bInitCompress)
	{
		jpeg_destroy_compress(&compress_info);
	}
	SAFE_RELEASE(m_pYUVImage);
}

HRESULT CSWJPEGFilter::Receive(CSWObject* obj)
{
	if(IsDecendant(CSWImage, obj))
	{
		CSWImage* pImage = (CSWImage *)obj;
		if(pImage->GetType() == SW_IMAGE_JPEG)
		{
			static INT iFrameCount = 0;
			static DWORD dwTotalTime = 0;
			
			DWORD dwStart = CSWDateTime::GetSystemTick();
			pImage = Decompress(pImage);
			dwTotalTime += CSWDateTime::GetSystemTick() - dwStart;

			if (iFrameCount++ == 20)
			{
				SW_TRACE_NORMAL("||||||||||||||||||||||decode jpeg escape:%d ms avg", dwTotalTime/20);
				iFrameCount = 0;
				dwTotalTime = 0;
			}
			
			if(pImage)
			{
				GetOut(0)->Deliver(pImage);
				pImage->Release();
			}
		}
		else
		{
			if(GetIn(0)->GetPin())
			{
				CSWAutoLock aLock(&m_cMutex);
				if(NULL == m_pYUVImage)
				{
					m_pYUVImage = pImage;
					m_pYUVImage->AddRef();
				}
			}
			else
			{
				GetOut(0)->Deliver(pImage);				
			}
		}
	}
	return S_OK;
}

CSWImage* CSWJPEGFilter::Decompress(CSWImage *pImage)
{
	/* Establish the setjmp return context for my_error_exit to use. */
	if (setjmp(decompress_jerr.setjmp_buffer))
	{
		/* If we get here, the JPEG code has signaled an error.
		* We need to clean up the JPEG object, close the input file, and return.
		*/
		jpeg_destroy_decompress(&decompress_info);
		m_bInitDecompress = FALSE;
		SW_TRACE_NORMAL("decompress_jerr.setjmp_buffer setjmp failed\n");
		return NULL;
	}
	if(!m_bInitDecompress)
	{
		jpeg_create_decompress(&decompress_info);
		m_bInitDecompress = TRUE;
	}
	INT iSize = pImage->GetSize();
	CJPGBuffer jpg((BYTE *)pImage->GetImageBuffer(), iSize);
	decompress_info.src = &jpg;
	jpeg_read_header(&decompress_info, TRUE);
	decompress_info.out_color_space = JCS_YCbCr;
	jpeg_start_decompress(&decompress_info);
	m_cMutex.Lock();
	CSWImage* pNewImage = m_pYUVImage;
	m_pYUVImage = NULL;
	m_cMutex.Unlock();
	if(NULL == pNewImage)
	{
		SW_TRACE_NORMAL("pNewImage is NULL\n");
		return NULL;
	}
	SW_COMPONENT_IMAGE img;
	pNewImage->GetImage(&img);		
	PBYTE y = img.rgpbData[0];
	PBYTE uv = img.rgpbData[1];

	img.iWidth = decompress_info.image_width;
	img.iHeight= decompress_info.image_height;
	img.rgiStrideWidth[0] = img.rgiStrideWidth[1] = decompress_info.image_width;
	img.cImageType = SW_IMAGE_BT1120;
	pNewImage->SetImage(img);
	pNewImage->SetFrameName("TEST");

	int row_stride = decompress_info.output_width * decompress_info.output_components;
	//一行扫描线的大小
	JSAMPARRAY buffer = (*decompress_info.mem->alloc_sarray)((j_common_ptr) &decompress_info, JPOOL_IMAGE, row_stride,1);
	
	while(decompress_info.output_scanline < decompress_info.output_height)
	{
		jpeg_read_scanlines(&decompress_info,buffer, 1);
		//YUV444 to YUV420
		//YUVYUVYUVYUV		YYYYYYYYYYYYYYYY
		//YUVYUVYUVYUV ->	UVUVUVUV
		//YUVYUVYUVYUV
		//YUVYUVYUVYUV  
		if(decompress_info.output_components == 3)
		{
			PBYTE yuv = (PBYTE)buffer[0];

			for (int i=0; i<decompress_info.output_width; i++ )
			{
				*y = *yuv;
				y++;
				yuv++;

				if((decompress_info.output_scanline&0x1) == 0 && (i&0x1) == 0)
				{
					*uv = *yuv;
					uv++;
					yuv++;
					*uv = *yuv;
					uv++;
					yuv++;
				}
				else
				{
					yuv += 2;
				}
			}

			
		}

	}
	jpeg_finish_decompress(&decompress_info);
	pNewImage->SetLastImage(pImage);
	return pNewImage;
}