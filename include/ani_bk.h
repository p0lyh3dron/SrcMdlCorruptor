#include <stdio.h>
#include <cmath>

struct datamap_t;

#define DECLARE_BYTESWAP_DATADESC() \
    static datamap_t m_DataMap; \
    static datamap_t *GetBaseMap(); \
    template <typename T> friend void DataMapAccess(T *, datamap_t **p); \
    template <typename T> friend datamap_t *DataMapInit(T *);

#define CHECK_VALID( _v)    0

#define STUDIO_ANIM_RAWPOS	0x01 // Vector48
#define STUDIO_ANIM_RAWROT	0x02 // Quaternion48
#define STUDIO_ANIM_ANIMPOS	0x04 // mstudioanim_valueptr_t
#define STUDIO_ANIM_ANIMROT	0x08 // mstudioanim_valueptr_t
#define STUDIO_ANIM_DELTA	0x10
#define STUDIO_ANIM_RAWROT2	0x20 // Quaternion64

typedef unsigned char byte;
typedef float vec_t;
typedef unsigned long uint64;

typedef struct
{
    float f1, f2, f3;
}Vector;

class Quaternion                // same data-layout as engine's vec4_t,
{                                //        which is a vec_t[4]
public:
    inline Quaternion(void)    { 
    
    // Initialize to NAN to catch errors
#ifdef _DEBUG
#ifdef VECTOR_PARANOIA
        x = y = z = w = VEC_T_NAN;
#endif
#endif
    }
    inline Quaternion(vec_t ix, vec_t iy, vec_t iz, vec_t iw) : x(ix), y(iy), z(iz), w(iw) { }
    inline Quaternion(RadianEuler const &angle);    // evil auto type promotion!!!

    inline void Init(vec_t ix=0.0f, vec_t iy=0.0f, vec_t iz=0.0f, vec_t iw=0.0f)    { x = ix; y = iy; z = iz; w = iw; }

    bool IsValid() const;
    void Invalidate();

    bool operator==( const Quaternion &src ) const;
    bool operator!=( const Quaternion &src ) const;

    vec_t* Base() { return (vec_t*)this; }
    const vec_t* Base() const { return (vec_t*)this; }

    // array access...
    vec_t operator[](int i) const;
    vec_t& operator[](int i);

    vec_t x, y, z, w;
};

class Quaternion48
{
public:
    // Construction/destruction:
    Quaternion48(void); 
    Quaternion48(vec_t X, vec_t Y, vec_t Z);

    // assignment
    // Quaternion& operator=(const Quaternion48 &vOther);
    Quaternion48& operator=(const Quaternion &vOther);
    operator Quaternion ();
private:
    unsigned short x:16;
    unsigned short y:16;
    unsigned short z:15;
    unsigned short wneg:1;
};


inline Quaternion48::operator Quaternion ()    
{
    Quaternion tmp;

    tmp.x = ((int)x - 32768) * (1 / 32768.0);
    tmp.y = ((int)y - 32768) * (1 / 32768.0);
    tmp.z = ((int)z - 16384) * (1 / 16384.0);
    tmp.w = sqrt( 1 - tmp.x * tmp.x - tmp.y * tmp.y - tmp.z * tmp.z );
    if (wneg)
        tmp.w = -tmp.w;
    return tmp; 
}

inline Quaternion48& Quaternion48::operator=(const Quaternion &vOther)    
{
    CHECK_VALID(vOther);

    x = Clamp( (int)(vOther.x * 32768) + 32768, 0, 65535 );
    y = Clamp( (int)(vOther.y * 32768) + 32768, 0, 65535 );
    z = Clamp( (int)(vOther.z * 16384) + 16384, 0, 32767 );
    wneg = (vOther.w < 0);
    return *this; 
}

class Quaternion64
{
public:
    // Construction/destruction:
    Quaternion64(void); 
    Quaternion64(vec_t X, vec_t Y, vec_t Z);

    // assignment
    // Quaternion& operator=(const Quaternion64 &vOther);
    Quaternion64& operator=(const Quaternion &vOther);
    operator Quaternion ();
private:
    uint64 x:21;
    uint64 y:21;
    uint64 z:21;
    uint64 wneg:1;
};


inline Quaternion64::operator Quaternion ()    
{
    Quaternion tmp;

    // shift to -1048576, + 1048575, then round down slightly to -1.0 < x < 1.0
    tmp.x = ((int)x - 1048576) * (1 / 1048576.5f);
    tmp.y = ((int)y - 1048576) * (1 / 1048576.5f);
    tmp.z = ((int)z - 1048576) * (1 / 1048576.5f);
    tmp.w = sqrt( 1 - tmp.x * tmp.x - tmp.y * tmp.y - tmp.z * tmp.z );
    if (wneg)
        tmp.w = -tmp.w;
    return tmp; 
}

inline Quaternion64& Quaternion64::operator=(const Quaternion &vOther)    
{
    CHECK_VALID(vOther);

    x = Clamp( (int)(vOther.x * 1048576) + 1048576, 0, 2097151 );
    y = Clamp( (int)(vOther.y * 1048576) + 1048576, 0, 2097151 );
    z = Clamp( (int)(vOther.z * 1048576) + 1048576, 0, 2097151 );
    wneg = (vOther.w < 0);
    return *this; 
}

typedef struct
{
    typedescription_t    *dataDesc;
    int                    dataNumFields;
    char const            *dataClassName;
    datamap_t            *baseMap;

    bool                chains_validated;
    // Have the "packed" offsets been computed
    bool                packed_offsets_computed;
    int                    packed_size;

#if defined( _DEBUG )
    bool                bValidityChecked;
#endif // _DEBUG
}datamap_t;

typedef union mstudioanimvalue_t
{
	struct 
	{
		byte	valid;
		byte	total;
	} num;
	short		value;
}mstudioanimvalue_t;

typedef struct
{
	DECLARE_BYTESWAP_DATADESC();
	short	offset[3];
	inline mstudioanimvalue_t *pAnimvalue( int i ) const { if (offset[i] > 0) return  (mstudioanimvalue_t *)(((byte *)this) + offset[i]); else return NULL; };
}mstudioanim_valueptr_t;

typedef struct
{
    int		id;		// Model format ID, such as "IDST" (0x49 0x44 0x53 0x54)
	int		version;	// Format version number, such as 48 (0x30,0x00,0x00,0x00)
	int		checksum;	// This has to be the same in the phy and vtx files to load!
	char		name[64];		// The internal name of the model, padding with null bytes.
					// Typically "my_model.mdl" will have an internal name of "my_model"
	int		dataLength;	// Data size of MDL file in bytes.
 
	// A vector is 12 bytes, three 4-byte float-values in a row.
	Vector		eyeposition;	// Position of player viewpoint relative to model origin
	Vector		illumposition;	// ?? Presumably the point used for lighting when per-vertex lighting is not enabled.
	Vector		hull_min;	// Corner of model hull box with the least X/Y/Z values
	Vector		hull_max;	// Opposite corner of model hull box
	Vector	  	view_bbmin;
	Vector	 	view_bbmax;
 
	int		flags;		// Binary flags in little-endian order. 
					// ex (00000001,00000000,00000000,11000000) means flags for position 0, 30, and 31 are set. 
					// Set model flags section for more information
 
	/*
	 * After this point, the header contains many references to offsets
	 * within the MDL file and the number of items at those offsets.
	 *
	 * Offsets are from the very beginning of the file.
	 * 
	 * Note that indexes/counts are not always paired and ordered consistently.
	 */	
 
	// mstudiobone_t
	int		bone_count;	// Number of data sections (of type mstudiobone_t)
	int		bone_offset;	// Offset of first data section
 
	// mstudiobonecontroller_t
	int		bonecontroller_count;
	int		bonecontroller_offset;
 
	// mstudiohitboxset_t
	int		hitbox_count;
	int		hitbox_offset;
 
	// mstudioanimdesc_t
	int		localanim_count;
	int		localanim_offset;
 
	// mstudioseqdesc_t
	int		localseq_count;
	int		localseq_offset;
 
	int		activitylistversion; // ??
	int		eventsindexed;	// ??
 
	// VMT texture filenames
	// mstudiotexture_t
	int		texture_count;
	int		texture_offset;
 
	// This offset points to a series of ints.
        // Each int value, in turn, is an offset relative to the start of this header/the-file,
        // At which there is a null-terminated string.
	int		texturedir_count;
	int		texturedir_offset;
 
	// Each skin-family assigns a texture-id to a skin location
	int		skinreference_count;
	int		skinrfamily_count;
	int             skinreference_index;
 
	// mstudiobodyparts_t
	int		bodypart_count;
	int		bodypart_offset;
 
        // Local attachment points		
	// mstudioattachment_t
	int		attachment_count;
	int		attachment_offset;
 
	// Node values appear to be single bytes, while their names are null-terminated strings.
	int		localnode_count;
	int		localnode_index;
	int		localnode_name_index;
 
	// mstudioflexdesc_t
	int		flexdesc_count;
	int		flexdesc_index;
 
	// mstudioflexcontroller_t
	int		flexcontroller_count;
	int		flexcontroller_index;
 
	// mstudioflexrule_t
	int		flexrules_count;
	int		flexrules_index;
 
	// IK probably referse to inverse kinematics
	// mstudioikchain_t
	int		ikchain_count;
	int		ikchain_index;
 
	// Information about any "mouth" on the model for speech animation
	// More than one sounds pretty creepy.
	// mstudiomouth_t
	int		mouths_count; 
	int		mouths_index;
 
	// mstudioposeparamdesc_t
	int		localposeparam_count;
	int		localposeparam_index;
 
	/*
	 * For anyone trying to follow along, as of this writing,
	 * the next "surfaceprop_index" value is at position 0x0134 (308)
	 * from the start of the file.
	 */
 
	// Surface property value (single null-terminated string)
	int		surfaceprop_index;
 
	// Unusual: In this one index comes first, then count.
	// Key-value data is a series of strings. If you can't find
	// what you're interested in, check the associated PHY file as well.
	int		keyvalue_index;
	int		keyvalue_count;	
 
	// More inverse-kinematics
	// mstudioiklock_t
	int		iklock_count;
	int		iklock_index;
 
 
	float		mass; 		// Mass of object (4-bytes)
	int		contents;	// ??
 
	// Other models can be referenced for re-used sequences and animations
	// (See also: The $includemodel QC option.)
	// mstudiomodelgroup_t
	int		includemodel_count;
	int		includemodel_index;
	
	int		virtualModel;	// Placeholder for mutable-void*
 
	// mstudioanimblock_t
	int		animblocks_name_index;
	int		animblocks_count;
	int		animblocks_index;
	
	int		animblockModel; // Placeholder for mutable-void*

	// Points to a series of bytes?
	int		bonetablename_index;
	
	int		vertex_base;	// Placeholder for void*
	int		offset_base;	// Placeholder for void*
	
	// Used with $constantdirectionallight from the QC 
	// Model should have flag #13 set if enabled
	byte		directionaldotproduct;
	
	byte		rootLod;	// Preferred rather than clamped
	
	// 0 means any allowed, N means Lod 0 -> (N-1)
	byte		numAllowedRootLods;	
	
	byte		unused1; // ??
	int		unused2; // ??
	
	// mstudioflexcontrollerui_t
	int		flexcontrollerui_count;
	int		flexcontrollerui_index;
	
	/**
	 * Offset for additional header information.
	 * May be zero if not present, or also 408 if it immediately 
	 * follows this studiohdr_t
	 */
	// studiohdr2_t
	int		studiohdr2index;
	
	int		unused; // ??
	
	/**
	 * As of this writing, the header is 408 bytes long in total
	 */
}studiohdr_t;

typedef struct
{
	DECLARE_BYTESWAP_DATADESC();
	byte				bone;
	byte				flags;		// weighing options

	// valid for animating data only
	inline byte				*pData( void ) const { return (((byte *)this) + sizeof( mstudioanim_t )); };
	inline mstudioanim_valueptr_t	*pRotV( void ) const { return (mstudioanim_valueptr_t *)(pData()); };
	inline mstudioanim_valueptr_t	*pPosV( void ) const { return (mstudioanim_valueptr_t *)(pData()) + ((flags & STUDIO_ANIM_ANIMROT) != 0); };

	// valid if animation unvaring over timeline
	inline Quaternion48		*pQuat48( void ) const { return (Quaternion48 *)(pData()); };
	inline Quaternion64		*pQuat64( void ) const { return (Quaternion64 *)(pData()); };
	inline Vector48			*pPos( void ) const { return (Vector48 *)(pData() + ((flags & STUDIO_ANIM_RAWROT) != 0) * sizeof( *pQuat48() ) + ((flags & STUDIO_ANIM_RAWROT2) != 0) * sizeof( *pQuat64() ) ); };

	short				nextoffset;
	inline mstudioanim_t	*pNext( void ) const { if (nextoffset != 0) return  (mstudioanim_t *)(((byte *)this) + nextoffset); else return NULL; };
}mstudioanim_t;

typedef struct
{
	DECLARE_BYTESWAP_DATADESC();
	int					endframe;				
	int					motionflags;
	float				v0;			// velocity at start of block
	float				v1;			// velocity at end of block
	float				angle;		// YAW rotation at end of this blocks movement
	Vector				vector;		// movement vector relative to this blocks initial angle
	Vector				position;	// relative to start of animation???

	mstudiomovement_t(){}
private:
	// No copy constructors allowed
	mstudiomovement_t(const mstudiomovement_t& vOther);
}mstudiomovement_t;

typedef struct
{
    int					baseptr;
	inline studiohdr_t	*pStudiohdr( void ) const { return (studiohdr_t *)(((byte *)this) + baseptr); }

	int					sznameindex;
	inline char * const pszName( void ) const { return ((char *)this) + sznameindex; }

	float				fps;		// frames per second	
	int					flags;		// looping/non-looping flags

	int					numframes;

	// piecewise movement
	int					nummovements;
	int					movementindex;
	inline mstudiomovement_t * const pMovement( int i ) const { return (mstudiomovement_t *)(((byte *)this) + movementindex) + i; };

	int					unused1[6];			// remove as appropriate (and zero if loading older versions)	

	int					animblock;
	int					animindex;	 // non-zero when anim data isn't in sections
	mstudioanim_t *pAnimBlock( int block, int index ) const; // returns pointer to a specific anim block (local or external)
	mstudioanim_t *pAnim( int *piFrame, float &flStall ) const; // returns pointer to data and new frame index
	mstudioanim_t *pAnim( int *piFrame ) const; // returns pointer to data and new frame index

	int					numikrules;
	int					ikruleindex;	// non-zero when IK data is stored in the mdl
	int					animblockikruleindex; // non-zero when IK data is stored in animblock file
	mstudioikrule_t *pIKRule( int i ) const;

	int					numlocalhierarchy;
	int					localhierarchyindex;
	mstudiolocalhierarchy_t *pHierarchy( int i ) const;

	int					sectionindex;
	int					sectionframes; // number of frames used in each fast lookup section, zero if not used
	inline mstudioanimsections_t * const pSection( int i ) const { return (mstudioanimsections_t *)(((byte *)this) + sectionindex) + i; }

	short				zeroframespan;	// frames per span
	short				zeroframecount; // number of spans
	int					zeroframeindex;
	byte				*pZeroFrameData( ) const { if (zeroframeindex) return (((byte *)this) + zeroframeindex); else return NULL; };
	mutable float		zeroframestalltime;		// saved during read stalls

	mstudioanimdesc_t(){}
private:
	// No copy constructors allowed
	mstudioanimdesc_t(const mstudioanimdesc_t& vOther);
}AnimatedCursorDescription;


typedef struct
{
    AnimatedCursorHeader header;
}AnimatedCursor;

extern AnimatedCursor ani_load
    ( const char* path );

extern void ani_delete
    ( AnimatedCursor a );
