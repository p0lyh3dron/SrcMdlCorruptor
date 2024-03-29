#pragma once

#include "util.h"

/*
* Contain a bunch of junk about model information
*/


#define IDSTUDIOHEADER    (('T'<<24)+('S'<<16)+('D'<<8)+'I')


struct MDLHeader
{
    int		id;		// Model format ID, such as "IDST" (0x49 0x44 0x53 0x54)
	int		version;	// Format version number, such as 48 (0x30,0x00,0x00,0x00)
	int		checksum;	// This has to be the same in the phy and vtx files to load!
	char	name[64];		// The internal name of the model, padding with null bytes.
					// Typically "my_model.mdl" will have an internal name of "my_model"
	int		dataLength;	// Data size of MDL file in bytes.
 
	// A vector is 12 bytes, three 4-byte float-values in a row.
	vec3		eyeposition;	// Position of player viewpoint relative to model origin
	vec3		illumposition;	// ?? Presumably the point used for lighting when per-vertex lighting is not enabled.
	vec3		hull_min;	// Corner of model hull box with the least X/Y/Z values
	vec3		hull_max;	// Opposite corner of model hull box
	vec3	  	view_bbmin;
	vec3	 	view_bbmax;
 
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
};


// ======================================================================
// Data Map Thing idk
// ======================================================================


// i was gonna put some big alightment type here,
// but the size is different in debug so uh idk
#define DECLARE_BYTESWAP_DATADESC()


// ======================================================================
// Animation Info
// ======================================================================


// animation frames
union AnimValue
{
	struct 
	{
		byte	valid;
		byte	total;
	} num;
	short		value;
};


// huh
struct mstudioanim_valueptr_t
{
	DECLARE_BYTESWAP_DATADESC();

	short	offset[3];
	inline AnimValue *pAnimvalue( int i ) const
	{
		if (offset[i] > 0)
			return (AnimValue *)(((byte *)this) + offset[i]);
		else
			return NULL;
	};
};


#define STUDIO_ANIM_RAWPOS	0x01 // Vector48
#define STUDIO_ANIM_RAWROT	0x02 // Quaternion48
#define STUDIO_ANIM_ANIMPOS	0x04 // mstudioanim_valueptr_t
#define STUDIO_ANIM_ANIMROT	0x08 // mstudioanim_valueptr_t
#define STUDIO_ANIM_DELTA	0x10
#define STUDIO_ANIM_RAWROT2	0x20 // Quaternion64


struct AnimMovement
{
	int					endframe;				
	int					motionflags;
	float				v0;			// velocity at start of block
	float				v1;			// velocity at end of block
	float				angle;		// YAW rotation at end of this blocks movement
	vec3				vector;		// movement vector relative to this blocks initial angle
	vec3				position;	// relative to start of animation???
};


struct ANIBlock
{
	int                    datastart;
	int                    dataend;
};

struct AnimSection
{
	int                    animBlock;
	int                    animIndex;
};


struct ANIAnim
{
	byte bone;
	byte flags;
	short nextoffset;

	/*inline byte                *pData( void ) const { return (((byte *)this) + sizeof( AnimatedCursorBlock )); };


	inline AnimatedCursorBlock    *pNext( void ) const { if (nextoffset != 0) return  (AnimatedCursorBlock *)(((byte *)this) + nextoffset); else return NULL; };*/
};


struct ANIAnimDesc
{
	int                    baseptr;

	int                    sznameindex;

	float                  fps;        // frames per second    
	int                    flags;        // looping/non-looping flags

	int                    numframes;

	// piecewise movement
	int                    nummovements;
	int                    movementindex;

	int                    unused1[6];            // remove as appropriate (and zero if loading older versions)    

	int                    animblock;
	int                    animindex;     // non-zero when anim data isn't in sections

	int                    numikrules;
	int                    ikruleindex;    // non-zero when IK data is stored in the mdl
	int                    animblockikruleindex; // non-zero when IK data is stored in animblock file

	int                    numlocalhierarchy;
	int                    localhierarchyindex;

	int                    sectionindex;
	int                    sectionframes; // number of frames used in each fast lookup section, zero if not used

	short                  zeroframespan;    // frames per span
	short                  zeroframecount; // number of spans
	int                    zeroframeindex;
	float          		   zeroframestalltime;        // saved during read stalls
};


struct ANISeqDesc
{
	int					baseptr;

	int					szlabelindex;

	int					szactivitynameindex;

	int					flags;		// looping/non-looping flags

	int					activity;	// initialized at loadtime to game DLL values
	int					actweight;

	int					numevents;
	int					eventindex;

	vec3				bbmin;		// per sequence bounding box
	vec3				bbmax;		

	int					numblends;

	// Index into array of shorts which is groupsize[0] x groupsize[1] in length
	int					animindexindex;

	int					movementindex;	// [blend] float array for blended movement
	int					groupsize[2];
	int					paramindex[2];	// X, Y, Z, XR, YR, ZR
	float				paramstart[2];	// local (0..1) starting value
	float				paramend[2];	// local (0..1) ending value
	int					paramparent;

	float				fadeintime;		// ideal cross fate in time (0.2 default)
	float				fadeouttime;	// ideal cross fade out time (0.2 default)

	int					localentrynode;		// transition node at entry
	int					localexitnode;		// transition node at exit
	int					nodeflags;		// transition rules

	float				entryphase;		// used to match entry gait
	float				exitphase;		// used to match exit gait

	float				lastframe;		// frame that should generation EndOfSequence

	int					nextseq;		// auto advancing sequences
	int					pose;			// index of delta animation between end and nextseq

	int					numikrules;

	int					numautolayers;	//
	int					autolayerindex;

	int					weightlistindex;

	// FIXME: make this 2D instead of 2x1D arrays
	int					posekeyindex;

	int					numiklocks;
	int					iklockindex;

	// Key values
	int					keyvalueindex;
	int					keyvaluesize;

	int					cycleposeindex;		// index of pose parameter to use as cycle index

	int					unused[7];		// remove/add as appropriate (grow back to 8 ints on version change!)
};


struct ANIEvent
{
	float				cycle;
	int					event;
	int					type;

	char				options[64];

	int					szeventindex;
};


// ======================================================================
// Class to contain all information about a model
// ======================================================================

class Model
{
public:
	std::vector< char >     aBufferMdl;
	std::vector< char >     aBufferAni;

	bool                    aHasAni = false;

	MDLHeader*              aHeader;

	// Animation Parts
	ANIBlock*               aAnimBlocks;
	ANIAnimDesc*            aAnimDescs;
	ANISeqDesc*             aAnimSeqs;
	ANIEvent**              aSeqEvents;

	size_t                  aAnimBlocksCount;
	size_t                  aAnimDescsCount;
	size_t                  aAnimSeqsCount;
	size_t                  aSeqEventsCount;
};

