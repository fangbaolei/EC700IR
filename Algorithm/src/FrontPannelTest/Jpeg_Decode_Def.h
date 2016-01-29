#ifndef JPEG_DECODE_DEF__
#define JPEG_DECODE_DEF__

#define JPEG_MAXIMUM_COMPONENTS 3 

#define JPEG_FIXSHIFT	5

#define JEPG_MAX_MCU_ROW 400
#define JPEG_MAX_MCU_COL 600
#define JPEG_MAXIMUM_SCAN_COMPONENT_FACTORS 10

#define JPEG_DCTSIZE	8
#define JPEG_DCTSIZE2	JPEG_DCTSIZE * JPEG_DCTSIZE
#define JPEG_FIXED_TYPE short

enum JPEG_Marker
{
	JPEG_Marker_APP0 =  0xFFE0,
	JPEG_Marker_APP1 =  0xFFE1,
	JPEG_Marker_APP2 =  0xFFE2, 
	JPEG_Marker_APP3 =  0xFFE3, 
	JPEG_Marker_APP4 =  0xFFE4, 
	JPEG_Marker_APP5 =  0xFFE5, 
	JPEG_Marker_APP6 =  0xFFE6, 
	JPEG_Marker_APP7 =  0xFFE7, 
	JPEG_Marker_APP8 =  0xFFE8, 
	JPEG_Marker_APP9 =  0xFFE9, 
	JPEG_Marker_APP10 = 0xFFEA, 
	JPEG_Marker_APP11 = 0xFFEB, 
	JPEG_Marker_APP12 = 0xFFEC, 
	JPEG_Marker_APP13 = 0xFFED, 
	JPEG_Marker_APP14 = 0xFFEE, 
	JPEG_Marker_APP15 = 0xFFEF, 
	JPEG_Marker_COM =	0xFFFE, 
	JPEG_Marker_DHT =	0xFFC4, 
	JPEG_Marker_DQT =	0xFFDB, 
	JPEG_Marker_DRI =	0xFFDD, 
	JPEG_Marker_EOI =	0xFFD9, 
	JPEG_Marker_SOF0 =	0xFFC0, 
	JPEG_Marker_SOI =	0xFFD8, 
	JPEG_Marker_SOS =	0xFFDA 
};

typedef enum JPEG_Marker JPEG_Marker;

#define JPEG_Assert(TEST) if (!(TEST)) {return 0;}

#define JPEG_ITOFIX(A) ((A) << JPEG_FIXSHIFT)

#define JPEG_BITS_START() \
	unsigned int bits_left = 0; \
	unsigned int bits_data = 0

#define JPEG_BITS_CHECK() \
	while (bits_left < 25) \
{ \
	bits_data = (bits_data << 8) | (*data ++); \
	if (data [-1] == 0xFF) data ++; \
	bits_left += 8; \
}

#define JPEG_BITS_GET(COUNT) \
	((bits_data >> (bits_left -= (COUNT))) & ((1 << (COUNT)) - 1));

#define JPEG_BITS_PEEK(COUNT) \
	((bits_data >> (bits_left - (COUNT))) & ((1 << (COUNT)) - 1))

#define JPEG_BITS_DROP(COUNT) \
	(bits_left -= (COUNT))

#define JPEG_HuffmanTable_Decode(TABLE, OUT) \
	result = JPEG_BITS_PEEK (8); \
	if ((bitcount = (TABLE)->look_nbits [result]) != 0) \
{ \
	JPEG_BITS_DROP (bitcount); \
	result = (TABLE)->look_sym [result]; \
} \
	else \
{ \
	i = 7; \
	JPEG_BITS_DROP (8); \
	do \
{  \
	result = (result << 1) | JPEG_BITS_GET (1); \
	i++; \
	if (i == 16) return 0;	\
} \
	while (result > (TABLE)->maxcode [i]); \
	result = (TABLE)->valptr [i] [result]; \
	JPEG_BITS_CHECK (); \
} \
	(OUT) = result;

#define JPEG_Value(COUNT, OUT) \
	value = JPEG_BITS_GET (COUNT); \
	if (value < (unsigned int) (1 << ((unsigned int) (COUNT - 1)))) \
	value += (-1 << COUNT) + 1; \
	(OUT) = value;

#define JPEG_BITS_REWIND() \
	if (bits_left > 32)	\
{	\
	return 0;	\
}	\
	unsigned int count = bits_left >> 3; \
	while (count --) \
{ \
	data --; \
	if (data [-1] == 0xFF) \
	data --; \
} \
	bits_left = 0; \
	bits_data = 0;

#define JPEG_ConvertMCU_Limit(v) (v + 256)>>1

#define MIN_INT( int1, int2 )	( ( int1 ) < ( int2 ) ? ( int1 ) : ( int2 ) )
#define MAX_INT( int1, int2 )	( ( int1 ) > ( int2 ) ? ( int1 ) : ( int2 ) )

#endif
