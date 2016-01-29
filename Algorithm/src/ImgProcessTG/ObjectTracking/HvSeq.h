#ifndef __HV_SEQ_H__
#define __HV_SEQ_H__

#include "HvBaseType.h"

#define HV_STORAGE_MAGIC_VAL    0x42890000

#define HV_MAGIC_MASK       0xFFFF0000
#define HV_MAT_MAGIC_VAL    0x42420000


inline int hvFloor( double value )
{
	int i = hvRound(value);
	Hv32suf diff;
	diff.f = (float)(value - i);
	return i - (diff.i < 0);
}

/* default storage block size */
#define  HV_STORAGE_BLOCK_SIZE   ((1<<16) - 128)
/* default alignment for dynamic data strucutures, resided in storages. */
#define  HV_STRUCT_ALIGN    ((int)sizeof(double))

#define HV_MEMCPY_INT( dst, src, len )                  \
{                                                       \
	size_t _ihv_memcpy_i_, _ihv_memcpy_len_ = (len);    \
	int* _ihv_memcpy_dst_ = (int*)(dst);                \
	const int* _ihv_memcpy_src_ = (const int*)(src);    \
	\
	for(_ihv_memcpy_i_=0;_ihv_memcpy_i_<_ihv_memcpy_len_;_ihv_memcpy_i_++)  \
	_ihv_memcpy_dst_[_ihv_memcpy_i_] = _ihv_memcpy_src_[_ihv_memcpy_i_];\
}

typedef struct HvString
{
	int len;
	char* ptr;
}
HvString;

typedef struct HvMemBlock
{
	struct HvMemBlock*  prev;
	struct HvMemBlock*  next;
}
HvMemBlock;

typedef struct HvMemStorage
{
	int signature;
	HvMemBlock* bottom;           /* First allocated block.                   */
	HvMemBlock* top;              /* Current memory block - top of the stack. */
	struct  HvMemStorage* parent; /* We get new blocks from parent as needed. */
	int block_size;               /* Block size.                              */
	int free_space;               /* Remaining free space in current block.   */
}
HvMemStorage;

#define HV_IS_STORAGE(storage)  \
	((storage) != NULL &&       \
	(((HvMemStorage*)(storage))->signature & HV_MAGIC_MASK) == HV_STORAGE_MAGIC_VAL)


typedef struct HvMemStoragePos
{
	HvMemBlock* top;
	int free_space;
}
HvMemStoragePos;


HvMemStorage* hvCreateMemStorage( int block_size HV_DEFAULT(0));
HvMemStorage* hvCreateChildMemStorage( HvMemStorage* parent );
void hvReleaseMemStorage( HvMemStorage** storage );
void hvClearMemStorage( HvMemStorage* storage );
void hvSaveMemStoragePos( const HvMemStorage* storage, HvMemStoragePos* pos );
void hvRestoreMemStoragePos( HvMemStorage* storage, HvMemStoragePos* pos );
void* hvMemStorageAlloc( HvMemStorage* storage, size_t size );
HvString hvMemStorageAllocString( HvMemStorage* storage, const char* ptr, int len HV_DEFAULT(-1) );

/* Release all blocks of the storage (or return them to parent, if any): */
void ihvDestroyMemStorage( HvMemStorage* storage );
/* Moves stack pointer to next block. If no blocks, allocate new one and link it to the storage: */
void ihvGoNextMemBlock( HvMemStorage * storage );

 void hvReleaseMemStorage( HvMemStorage** storage );
 void hvClearMemStorage( HvMemStorage * storage );

//------------------------------------------------------------------------------------------------
/*********************************** Sequence *******************************************/

typedef struct HvSeqBlock
{
	struct HvSeqBlock*  prev; /* Previous sequence block.                   */
	struct HvSeqBlock*  next; /* Next sequence block.                       */
	int    start_index;         /* Index of the first element in the block +  */
	/* sequence->first->start_index.              */
	int    count;             /* Number of elements in the block.           */
	schar* data;              /* Pointer to the first element of the block. */
}
HvSeqBlock;


#define HV_TREE_NODE_FIELDS(node_type)                               \
	int       flags;             /* Miscellaneous flags.     */      \
	int       header_size;       /* Size of sequence header. */      \
struct    node_type* h_prev; /* Previous sequence.       */      \
struct    node_type* h_next; /* Next sequence.           */      \
struct    node_type* v_prev; /* 2nd previous sequence.   */      \
struct    node_type* v_next  /* 2nd next sequence.       */

/*
Read/Write sequence.
Elements can be dynamically inserted to or deleted from the sequence.
*/
#define HV_SEQUENCE_FIELDS()                                              \
	HV_TREE_NODE_FIELDS(HvSeq);                                           \
	int       total;          /* Total number of elements.            */  \
	int       elem_size;      /* Size of sequence element in bytes.   */  \
	schar*    block_max;      /* Maximal bound of the last block.     */  \
	schar*    ptr;            /* Current write pointer.               */  \
	int       delta_elems;    /* Grow seq this many at a time.        */  \
	HvMemStorage* storage;    /* Where the seq is stored.             */  \
	HvSeqBlock* free_blocks;  /* Free blocks list.                    */  \
	HvSeqBlock* first;        /* Pointer to the first sequence block. */

typedef struct HvSeq
{
	HV_SEQUENCE_FIELDS()
}
HvSeq;

#define HV_TYPE_NAME_SEQ             "hv-sequence"
#define HV_TYPE_NAME_SEQ_TREE        "hv-sequence-tree"

/*************************************** Set ********************************************/
/*
Set.
Order is not preserved. There can be gaps between sequence elements.
After the element has been inserted it stays in the same place all the time.
The MSB(most-significant or sign bit) of the first field (flags) is 0 iff the element exists.
*/
#define HV_SET_ELEM_FIELDS(elem_type)   \
	int  flags;                         \
struct elem_type* next_free;

typedef struct HvSetElem
{
	HV_SET_ELEM_FIELDS(HvSetElem)
}
HvSetElem;

#define HV_SET_FIELDS()      \
	HV_SEQUENCE_FIELDS()     \
	HvSetElem* free_elems;   \
	int active_count;

typedef struct HvSet
{
	HV_SET_FIELDS()
}
HvSet;


#define HV_SET_ELEM_IDX_MASK   ((1 << 26) - 1)
#define HV_SET_ELEM_FREE_FLAG  (1 << (sizeof(int)*8-1))

/* Checks whether the element pointed by ptr belongs to a set or not */
#define HV_IS_SET_ELEM( ptr )  (((HvSetElem*)(ptr))->flags >= 0)


/*********************************** Chain/Countour *************************************/

typedef struct HvChain
{
	HV_SEQUENCE_FIELDS()
		HV_POINT  origin;
}
HvChain;

#define HV_CONTOUR_FIELDS()  \
	HV_SEQUENCE_FIELDS()     \
	HvRect rect;             \
	int color;               \
	int reserved[3];

typedef struct HvContour
{
	HV_CONTOUR_FIELDS()
}
HvContour;

typedef HvContour HvPoint2DSeq;


/****************************************************************************************\
*                                    Sequence types                                      *
\****************************************************************************************/

#define HV_SEQ_MAGIC_VAL             0x42990000

#define HV_IS_SEQ(seq) \
	((seq) != NULL && (((HvSeq*)(seq))->flags & HV_MAGIC_MASK) == HV_SEQ_MAGIC_VAL)

#define HV_SET_MAGIC_VAL             0x42980000
#define HV_IS_SET(set) \
	((set) != NULL && (((HvSeq*)(set))->flags & HV_MAGIC_MASK) == HV_SET_MAGIC_VAL)

#define HV_SEQ_ELTYPE_BITS           9
#define HV_SEQ_ELTYPE_MASK           ((1 << HV_SEQ_ELTYPE_BITS) - 1)

#define HV_SEQ_ELTYPE_POINT          HV_32SC2  /* (x,y) */
#define HV_SEQ_ELTYPE_CODE           HV_8UC1   /* freeman code: 0..7 */
#define HV_SEQ_ELTYPE_GENERIC        0
#define HV_SEQ_ELTYPE_PTR            HV_USRTYPE1
#define HV_SEQ_ELTYPE_PPOINT         HV_SEQ_ELTYPE_PTR  /* &(x,y) */
#define HV_SEQ_ELTYPE_INDEX          HV_32SC1  /* #(x,y) */
#define HV_SEQ_ELTYPE_GRAPH_EDGE     0  /* &next_o, &next_d, &vtx_o, &vtx_d */
#define HV_SEQ_ELTYPE_GRAPH_VERTEX   0  /* first_edge, &(x,y) */
#define HV_SEQ_ELTYPE_TRIAN_ATR      0  /* vertex of the binary tree   */
#define HV_SEQ_ELTYPE_CONNECTED_COMP 0  /* connected component  */
#define HV_SEQ_ELTYPE_POINT3D        HV_32FC3  /* (x,y,z)  */

#define HV_SEQ_KIND_BITS        3
#define HV_SEQ_KIND_MASK        (((1 << HV_SEQ_KIND_BITS) - 1)<<HV_SEQ_ELTYPE_BITS)

/* types of sequences */
#define HV_SEQ_KIND_GENERIC     (0 << HV_SEQ_ELTYPE_BITS)
#define HV_SEQ_KIND_CURVE       (1 << HV_SEQ_ELTYPE_BITS)
#define HV_SEQ_KIND_BIN_TREE    (2 << HV_SEQ_ELTYPE_BITS)

/* types of sparse sequences (sets) */
#define HV_SEQ_KIND_GRAPH       (3 << HV_SEQ_ELTYPE_BITS)
#define HV_SEQ_KIND_SUBDIV2D    (4 << HV_SEQ_ELTYPE_BITS)

#define HV_SEQ_FLAG_SHIFT       (HV_SEQ_KIND_BITS + HV_SEQ_ELTYPE_BITS)

/* flags for curves */
#define HV_SEQ_FLAG_CLOSED     (1 << HV_SEQ_FLAG_SHIFT)
#define HV_SEQ_FLAG_SIMPLE     (2 << HV_SEQ_FLAG_SHIFT)
#define HV_SEQ_FLAG_CONVEX     (4 << HV_SEQ_FLAG_SHIFT)
#define HV_SEQ_FLAG_HOLE       (8 << HV_SEQ_FLAG_SHIFT)

/* flags for graphs */
#define HV_GRAPH_FLAG_ORIENTED (1 << HV_SEQ_FLAG_SHIFT)

#define HV_GRAPH               HV_SEQ_KIND_GRAPH
#define HV_ORIENTED_GRAPH      (HV_SEQ_KIND_GRAPH|HV_GRAPH_FLAG_ORIENTED)

/* point sets */
#define HV_SEQ_POINT_SET       (HV_SEQ_KIND_GENERIC| HV_SEQ_ELTYPE_POINT)
#define HV_SEQ_POINT3D_SET     (HV_SEQ_KIND_GENERIC| HV_SEQ_ELTYPE_POINT3D)
#define HV_SEQ_POLYLINE        (HV_SEQ_KIND_CURVE  | HV_SEQ_ELTYPE_POINT)
#define HV_SEQ_POLYGON         (HV_SEQ_FLAG_CLOSED | HV_SEQ_POLYLINE )
#define HV_SEQ_CONTOUR         HV_SEQ_POLYGON
#define HV_SEQ_SIMPLE_POLYGON  (HV_SEQ_FLAG_SIMPLE | HV_SEQ_POLYGON  )

/* chain-coded curves */
#define HV_SEQ_CHAIN           (HV_SEQ_KIND_CURVE  | HV_SEQ_ELTYPE_CODE)
#define HV_SEQ_CHAIN_CONTOUR   (HV_SEQ_FLAG_CLOSED | HV_SEQ_CHAIN)

/* binary tree for the contour */
#define HV_SEQ_POLYGON_TREE    (HV_SEQ_KIND_BIN_TREE  | HV_SEQ_ELTYPE_TRIAN_ATR)

/* sequence of the connected components */
#define HV_SEQ_CONNECTED_COMP  (HV_SEQ_KIND_GENERIC  | HV_SEQ_ELTYPE_CONNECTED_COMP)

/* sequence of the integer numbers */
#define HV_SEQ_INDEX           (HV_SEQ_KIND_GENERIC  | HV_SEQ_ELTYPE_INDEX)

#define HV_SEQ_ELTYPE( seq )   ((seq)->flags & HV_SEQ_ELTYPE_MASK)
#define HV_SEQ_KIND( seq )     ((seq)->flags & HV_SEQ_KIND_MASK )

/* flag checking */
#define HV_IS_SEQ_INDEX( seq )      ((HV_SEQ_ELTYPE(seq) == HV_SEQ_ELTYPE_INDEX) && \
	(HV_SEQ_KIND(seq) == HV_SEQ_KIND_GENERIC))

#define HV_IS_SEQ_CURVE( seq )      (HV_SEQ_KIND(seq) == HV_SEQ_KIND_CURVE)
#define HV_IS_SEQ_CLOSED( seq )     (((seq)->flags & HV_SEQ_FLAG_CLOSED) != 0)
#define HV_IS_SEQ_CONVEX( seq )     (((seq)->flags & HV_SEQ_FLAG_CONVEX) != 0)
#define HV_IS_SEQ_HOLE( seq )       (((seq)->flags & HV_SEQ_FLAG_HOLE) != 0)
#define HV_IS_SEQ_SIMPLE( seq )     ((((seq)->flags & HV_SEQ_FLAG_SIMPLE) != 0) || \
	HV_IS_SEQ_CONVEX(seq))

/* type checking macros */
#define HV_IS_SEQ_POINT_SET( seq ) \
	((HV_SEQ_ELTYPE(seq) == HV_32SC2 || HV_SEQ_ELTYPE(seq) == HV_32FC2))

#define HV_IS_SEQ_POINT_SUBSET( seq ) \
	(HV_IS_SEQ_INDEX( seq ) || HV_SEQ_ELTYPE(seq) == HV_SEQ_ELTYPE_PPOINT)

#define HV_IS_SEQ_POLYLINE( seq )   \
	(HV_SEQ_KIND(seq) == HV_SEQ_KIND_CURVE && HV_IS_SEQ_POINT_SET(seq))

#define HV_IS_SEQ_POLYGON( seq )   \
	(HV_IS_SEQ_POLYLINE(seq) && HV_IS_SEQ_CLOSED(seq))

#define HV_IS_SEQ_CHAIN( seq )   \
	(HV_SEQ_KIND(seq) == HV_SEQ_KIND_CURVE && (seq)->elem_size == 1)

#define HV_IS_SEQ_CONTOUR( seq )   \
	(HV_IS_SEQ_CLOSED(seq) && (HV_IS_SEQ_POLYLINE(seq) || HV_IS_SEQ_CHAIN(seq)))

#define HV_IS_SEQ_CHAIN_CONTOUR( seq ) \
	(HV_IS_SEQ_CHAIN( seq ) && HV_IS_SEQ_CLOSED( seq ))

#define HV_IS_SEQ_POLYGON_TREE( seq ) \
	(HV_SEQ_ELTYPE (seq) ==  HV_SEQ_ELTYPE_TRIAN_ATR &&    \
	HV_SEQ_KIND( seq ) ==  HV_SEQ_KIND_BIN_TREE )

#define HV_IS_GRAPH( seq )    \
	(HV_IS_SET(seq) && HV_SEQ_KIND((HvSet*)(seq)) == HV_SEQ_KIND_GRAPH)

#define HV_IS_GRAPH_ORIENTED( seq )   \
	(((seq)->flags & HV_GRAPH_FLAG_ORIENTED) != 0)

#define HV_IS_SUBDIV2D( seq )  \
	(HV_IS_SET(seq) && HV_SEQ_KIND((HvSet*)(seq)) == HV_SEQ_KIND_SUBDIV2D)

/****************************************************************************************/
/*                            Sequence writer & reader                                  */
/****************************************************************************************/

#define HV_SEQ_WRITER_FIELDS()                                     \
	int          header_size;                                      \
	HvSeq*       seq;        /* the sequence written */            \
	HvSeqBlock*  block;      /* current block */                   \
	schar*       ptr;        /* pointer to free space */           \
	schar*       block_min;  /* pointer to the beginning of block*/\
	schar*       block_max;  /* pointer to the end of block */

typedef struct HvSeqWriter
{
	HV_SEQ_WRITER_FIELDS()
}
HvSeqWriter;


#define HV_SEQ_READER_FIELDS()                                      \
	int          header_size;                                       \
	HvSeq*       seq;        /* sequence, beign read */             \
	HvSeqBlock*  block;      /* current block */                    \
	schar*       ptr;        /* pointer to element be read next */  \
	schar*       block_min;  /* pointer to the beginning of block */\
	schar*       block_max;  /* pointer to the end of block */      \
	int          delta_index;/* = seq->first->start_index   */      \
	schar*       prev_elem;  /* pointer to previous element */


typedef struct HvSeqReader
{
	HV_SEQ_READER_FIELDS()
}
HvSeqReader;


/****************************************************************************************/
/*                                Operations on sequences                               */
/****************************************************************************************/

#define  HV_SEQ_ELEM( seq, elem_type, index )                    \
	/* assert gives some guarantee that <seq> parameter is valid */  \
	(   assert(sizeof((seq)->first[0]) == sizeof(HvSeqBlock) &&      \
	(seq)->elem_size == sizeof(elem_type)),                      \
	(elem_type*)((seq)->first && (unsigned)index <               \
	(unsigned)((seq)->first->count) ?                            \
	(seq)->first->data + (index) * sizeof(elem_type) :           \
	hvGetSeqElem( (HvSeq*)(seq), (index) )))
#define HV_GET_SEQ_ELEM( elem_type, seq, index ) HV_SEQ_ELEM( (seq), elem_type, (index) )

/* Add element to sequence: */
#define HV_WRITE_SEQ_ELEM_VAR( elem_ptr, writer )     \
{                                                     \
	if( (writer).ptr >= (writer).block_max )          \
{                                                 \
	hvCreateSeqBlock( &writer);                   \
}                                                 \
	memcpy((writer).ptr, elem_ptr, (writer).seq->elem_size);\
	(writer).ptr += (writer).seq->elem_size;          \
}

#define HV_WRITE_SEQ_ELEM( elem, writer )             \
{                                                     \
	if( (writer).ptr >= (writer).block_max )          \
{                                                 \
	hvCreateSeqBlock( &writer);                   \
}                                                 \
	memcpy((writer).ptr, &(elem), sizeof(elem));      \
	(writer).ptr += sizeof(elem);                     \
}


/* Move reader position forward: */
#define HV_NEXT_SEQ_ELEM( elem_size, reader )                 \
{                                                             \
	if( ((reader).ptr += (elem_size)) >= (reader).block_max ) \
{                                                         \
	hvChangeSeqBlock( &(reader), 1 );                     \
}                                                         \
}


/* Move reader position backward: */
#define HV_PREV_SEQ_ELEM( elem_size, reader )                \
{                                                            \
	if( ((reader).ptr -= (elem_size)) < (reader).block_min ) \
{                                                        \
	hvChangeSeqBlock( &(reader), -1 );                   \
}                                                        \
}

/* Read element and move read position forward: */
#define HV_READ_SEQ_ELEM( elem, reader )                       \
{                                                              \
	memcpy( &(elem), (reader).ptr, sizeof((elem)));            \
	HV_NEXT_SEQ_ELEM( sizeof(elem), reader )                   \
}

/* Read element and move read position backward: */
#define HV_REV_READ_SEQ_ELEM( elem, reader )                     \
{                                                                \
	memcpy(&(elem), (reader).ptr, sizeof((elem)));               \
	HV_PREV_SEQ_ELEM( sizeof(elem), reader )                     \
}


#define HV_READ_CHAIN_POINT( _pt, reader )                              \
{                                                                       \
	(_pt) = (reader).pt;                                                \
	if( (reader).ptr )                                                  \
{                                                                   \
	HV_READ_SEQ_ELEM( (reader).code, (reader));                     \
	(reader).pt.x += (reader).deltas[(int)(reader).code][0];        \
	(reader).pt.y += (reader).deltas[(int)(reader).code][1];        \
}                                                                   \
}

#define HV_CURRENT_POINT( reader )  (*((HV_POINT*)((reader).ptr)))
#define HV_PREV_POINT( reader )     (*((HV_POINT*)((reader).prev_elem)))

#define HV_READ_EDGE( pt1, pt2, reader )               \
{                                                      \
	assert( sizeof(pt1) == sizeof(HV_POINT) &&          \
	sizeof(pt2) == sizeof(HV_POINT) &&          \
	reader.seq->elem_size == sizeof(HV_POINT)); \
	(pt1) = HV_PREV_POINT( reader );                   \
	(pt2) = HV_CURRENT_POINT( reader );                \
	(reader).prev_elem = (reader).ptr;                 \
	HV_NEXT_SEQ_ELEM( sizeof(HV_POINT), (reader));      \
}




//-------------------------------------------------------------------------
/* Creates new empty sequence that will reside in the specified storage */
HvSeq* hvCreateSeq( int seq_flags, int header_size,
						   int elem_size, HvMemStorage* storage );

/* Changes default size (granularity) of sequence blocks.
The default size is ~1Kbyte */
void hvSetSeqBlockSize( HvSeq* seq, int delta_elems );


/* Adds new element to the end of sequence. Returns pointer to the element */
schar* hvSeqPush( HvSeq* seq, const void* element HV_DEFAULT(NULL));


/* Adds new element to the beginning of sequence. Returns pointer to it */
schar* hvSeqPushFront( HvSeq* seq, const void* element HV_DEFAULT(NULL));


/* Removes the last element from sequence and optionally saves it */
void hvSeqPop( HvSeq* seq, void* element HV_DEFAULT(NULL));


/* Removes the first element from sequence and optioanally saves it */
void hvSeqPopFront( HvSeq* seq, void* element HV_DEFAULT(NULL));


#define HV_FRONT 1
#define HV_BACK 0
/* Adds several new elements to the end of sequence */
void hvSeqPushMulti( HvSeq* seq, const void* elements,
							int count, int in_front HV_DEFAULT(0) );

/* Removes several elements from the end of sequence and optionally saves them */
void hvSeqPopMulti( HvSeq* seq, void* elements,
						   int count, int in_front HV_DEFAULT(0) );

/* Inserts a new element in the middle of sequence.
hvSeqInsert(seq,0,elem) == hvSeqPushFront(seq,elem) */
schar* hvSeqInsert( HvSeq* seq, int before_index,
						   const void* element HV_DEFAULT(NULL));

/* Removes specified sequence element */
void hvSeqRemove( HvSeq* seq, int index );


/* Removes all the elements from the sequence. The freed memory
can be reused later only by the same sequence unless hvClearMemStorage
or hvRestoreMemStoragePos is called */
void hvClearSeq( HvSeq* seq );


/* Retrieves pointer to specified sequence element.
Negative indices are supported and mean counting from the end
(e.g -1 means the last sequence element) */
schar* hvGetSeqElem( const HvSeq* seq, int index );

/* Calculates index of the specified sequence element.
Returns -1 if element does not belong to the sequence */
int hvSeqElemIdx( const HvSeq* seq, const void* element,
						 HvSeqBlock** block HV_DEFAULT(NULL) );

/* Initializes sequence writer. The new elements will be added to the end of sequence */
void hvStartAppendToSeq( HvSeq* seq, HvSeqWriter* writer );


/* Combination of hvCreateSeq and hvStartAppendToSeq */
void hvStartWriteSeq( int seq_flags, int header_size,
							 int elem_size, HvMemStorage* storage,
							 HvSeqWriter* writer );

/* Closes sequence writer, updates sequence header and returns pointer
to the resultant sequence
(which may be useful if the sequence was created using hvStartWriteSeq))
*/
HvSeq* hvEndWriteSeq( HvSeqWriter* writer );


/* Updates sequence header. May be useful to get access to some of previously
written elements via hvGetSeqElem or sequence reader */
void hvFlushSeqWriter( HvSeqWriter* writer );


/* Initializes sequence reader.
The sequence can be read in forward or backward direction */
void hvStartReadSeq( const HvSeq* seq, HvSeqReader* reader,
						   int reverse HV_DEFAULT(0) );


/* Returns current sequence reader position (currently observed sequence element) */
int hvGetSeqReaderPos( HvSeqReader* reader );


/* Changes sequence reader position. It may seek to an absolute or
to relative to the current position */
void hvSetSeqReaderPos( HvSeqReader* reader, int index,
								int is_relative HV_DEFAULT(0));

/* Copies sequence content to a continuous piece of memory */
void* hvCvtSeqToArray( const HvSeq* seq, void* elements,
							  HvSlice slice HV_DEFAULT(HV_WHOLE_SEQ) );

/* Creates sequence header for array.
After that all the operations on sequences that do not alter the content
can be applied to the resultant sequence */
HvSeq* hvMakeSeqHeaderForArray( int seq_type, int header_size,
									  int elem_size, void* elements, int total,
									  HvSeq* seq, HvSeqBlock* block );

/* Extracts sequence slice (with or without copying sequence elements) */
HvSeq* hvSeqSlice( const HvSeq* seq, HvSlice slice,
						 HvMemStorage* storage HV_DEFAULT(NULL),
						 int copy_data HV_DEFAULT(0));

inline HvSeq* hvCloneSeq( const HvSeq* seq, HvMemStorage* storage HV_DEFAULT(NULL))
{
	return hvSeqSlice( seq, HV_WHOLE_SEQ, storage, 1 );
}

/* Removes sequence slice */
void hvSeqRemoveSlice( HvSeq* seq, HvSlice slice );

/* Inserts a sequence or array into another sequence */
void hvSeqInsertSlice( HvSeq* seq, int before_index, const HvArr* from_arr );

/* a < b ? -1 : a > b ? 1 : 0 */
typedef int (/*__cdecl*/* HvCmpFunc)(const void* a, const void* b, void* userdata );

/* Sorts sequence in-place given element comparison function */
void hvSeqSort( HvSeq* seq, HvCmpFunc func, void* userdata HV_DEFAULT(NULL) );

/* Finds element in a [sorted] sequence */
schar* hvSeqSearch( HvSeq* seq, const void* elem, HvCmpFunc func,
						  int is_sorted, int* elem_idx,
						  void* userdata HV_DEFAULT(NULL) );

/* Reverses order of sequence elements in-place */
void hvSeqInvert( HvSeq* seq );

/* Splits sequence into one or more equivalence classes using the specified criteria */
int hvSeqPartition( const HvSeq* seq, HvMemStorage* storage,
						   HvSeq** labels, HvCmpFunc is_equal, void* userdata );

HvSeq *hvCreateSeq( int seq_flags, int header_size, int elem_size, HvMemStorage * storage );




/************ Internal sequence functions ************/
void hvChangeSeqBlock( void* _reader, int direction );
void hvCreateSeqBlock( HvSeqWriter * writer );


/* Creates a new set */
HvSet* hvCreateSet( int set_flags, int header_size,
						   int elem_size, HvMemStorage* storage );

/* Adds new element to the set and returns pointer to it */
int hvSetAdd( HvSet* set_header, HvSetElem* elem HV_DEFAULT(NULL),
					 HvSetElem** inserted_elem HV_DEFAULT(NULL) );

/* Fast variant of hvSetAdd */
inline HvSetElem* hvSetNew( HvSet* set_header )
{
	HvSetElem* elem = set_header->free_elems;
	if( elem )
	{
		set_header->free_elems = elem->next_free;
		elem->flags = elem->flags & HV_SET_ELEM_IDX_MASK;
		set_header->active_count++;
	}
	else
		hvSetAdd( set_header, NULL, (HvSetElem**)&elem );
	return elem;
}

/* Removes set element given its pointer */
inline void hvSetRemoveByPtr( HvSet* set_header, void* elem )
{
	HvSetElem* _elem = (HvSetElem*)elem;
	//assert( _elem->flags >= 0 /*&& (elem->flags & HV_SET_ELEM_IDX_MASK) < set_header->total*/ );
	_elem->next_free = set_header->free_elems;
	_elem->flags = (_elem->flags & HV_SET_ELEM_IDX_MASK) | HV_SET_ELEM_FREE_FLAG;
	set_header->free_elems = _elem;
	set_header->active_count--;
}

/* Removes element from the set by its index  */
void  hvSetRemove( HvSet* set_header, int index );

/* Returns a set element by index. If the element doesn't belong to the set,
NULL is returned */
inline HvSetElem* hvGetSetElem( const HvSet* set_header, int index )
{
	HvSetElem* elem = (HvSetElem*)hvGetSeqElem( (HvSeq*)set_header, index );
	return elem && HV_IS_SET_ELEM( elem ) ? elem : 0;
}

/* Removes all the elements from the set */
void hvClearSet( HvSet* set_header );


#endif		// #ifndef __HV_DATASTRUCT_H__



/******************* Iteration through the sequence tree *****************/
typedef struct HvTreeNodeIterator
{
	const void* node;
	int level;
	int max_level;
}
HvTreeNodeIterator;

void hvInitTreeNodeIterator( HvTreeNodeIterator* tree_iterator,
								   const void* first, int max_level );
void* hvNextTreeNode( HvTreeNodeIterator* tree_iterator );
void* hvPrevTreeNode( HvTreeNodeIterator* tree_iterator );

/* Inserts sequence into tree with specified "parent" sequence.
If parent is equal to frame (e.g. the most external contour),
then added contour will have null pointer to parent. */
void hvInsertNodeIntoTree( void* node, void* parent, void* frame );

/* Removes contour from tree (together with the contour children). */
void hvRemoveNodeFromTree( void* node, void* frame );

/* Gathers pointers to all the sequences,
accessible from the <first>, to the single sequence */
HvSeq* hvTreeToNodeSeq( const void* first, int header_size,
							  HvMemStorage* storage );


//-------------------------------------------------------------------------
HvRect hvBoundingRectSeq( HvArr* points, int update HV_DEFAULT(0) );
