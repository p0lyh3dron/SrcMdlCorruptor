#include <stdio.h>

typedef unsigned char byte;

typedef struct
{
    float f1, f2, f3;
}Vector;

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
}MDLHead;

typedef struct
{
    int                    datastart;
    int                    dataend;
}ANIBlock;

typedef struct
{
    byte bone;
    byte flags;
    short nextoffset;

    /*inline byte                *pData( void ) const { return (((byte *)this) + sizeof( AnimatedCursorBlock )); };


    inline AnimatedCursorBlock    *pNext( void ) const { if (nextoffset != 0) return  (AnimatedCursorBlock *)(((byte *)this) + nextoffset); else return NULL; };*/
}ANIAnim;

typedef struct
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
}ANIAnimDesc;

typedef struct
{
	int					baseptr;

	int					szlabelindex;

	int					szactivitynameindex;

	int					flags;		// looping/non-looping flags

	int					activity;	// initialized at loadtime to game DLL values
	int					actweight;

	int					numevents;
	int					eventindex;
	
	Vector				bbmin;		// per sequence bounding box
	Vector				bbmax;		

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
}ANISeqDesc;

typedef struct
{
	float				cycle;
	int					event;
	int					type;
	
	char				options[64];

	int					szeventindex;
}ANIEvent;

typedef struct
{
	void* cache;

	int* boneMap;				// maps global bone to local bone
	int* masterBone;			// maps local bone to global bone
	int* masterSeq;			// maps local sequence to master sequence
	int* masterAnim;			// maps local animation to master animation
	int* masterAttachment;	// maps local attachment to global
	int* masterPose;			// maps local pose parameter to global
	int* masterNode;			// maps local transition nodes to global
}ANIVirtualGroup;

typedef struct
{
	int	flags;
	int activity;
	int group;
	int index;
}ANIVirtualSequence;

typedef struct
{
	int group;
	int index;		
}ANIVirtualGeneric;

typedef struct
{
	ANIVirtualSequence* m_seq;
	ANIVirtualGeneric* m_anim;
	ANIVirtualGeneric* m_attachment;
	ANIVirtualGeneric* m_pose;
	ANIVirtualGroup* m_group;
	ANIVirtualGeneric* m_node;
	ANIVirtualGeneric* m_iklock;
	unsigned short* m_autoplaySequences;
}ANIVirtualModel;

typedef struct
{
    MDLHead header;
    ANIBlock* animationBlocks;
	ANIAnimDesc* animDescs;
	ANISeqDesc* animSeqs;
	ANIEvent** seqEvents;

    char* mdlBuffer;
	char* aniBuffer;
}ANI;

extern ANI ani_load
    ( const char* mdlPath, const char* aniPath );

extern void ani_label_index_corrupt
    ( ANI* a );

extern void ani_activity_index_corrupt
    ( ANI* a );

extern void ani_anim_index_corrupt
    ( ANI* a );

extern void ani_event_index_corrupt
    ( ANI* a );

extern void ani_write
    ( ANI a, const char* path );

extern void ani_delete
    ( ANI a );

extern void ani_seq_dump
	( ANI a );
