/*
---------------------------------------------------------------------------
Open Asset Import Library (assimp)
---------------------------------------------------------------------------

Copyright (c) 2006-2012, assimp team

All rights reserved.

Redistribution and use of this software in source and binary forms, 
with or without modification, are permitted provided that the following 
conditions are met:

* Redistributions of source code must retain the above
  copyright notice, this list of conditions and the
  following disclaimer.

* Redistributions in binary form must reproduce the above
  copyright notice, this list of conditions and the
  following disclaimer in the documentation and/or other
  materials provided with the distribution.

* Neither the name of the assimp team, nor the names of its
  contributors may be used to endorse or promote products
  derived from this software without specific prior
  written permission of the assimp team.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT 
OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT 
LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY 
THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
---------------------------------------------------------------------------
*/

/** @file mesh.h
 *  @brief Declares the data structures in which the imported geometry is 
    returned by ASSIMP: aiMesh, aiFace and aiBone data structures.
 */
#ifndef INCLUDED_AI_MESH_H
#define INCLUDED_AI_MESH_H

#include "types.h"

#include <cmath>
#include <climits>
#include <cfloat>

#ifdef __cplusplus
#include "iostream"
extern "C" {
#endif

// ---------------------------------------------------------------------------
// Limits. These values are required to match the settings Assimp was 
// compiled against. Therfore, do not redefine them unless you build the 
// library from source using the same definitions.
// ---------------------------------------------------------------------------

/** @def AI_MAX_FACE_INDICES
 *  Maximum number of indices per face (polygon). */

#ifndef AI_MAX_FACE_INDICES 
#	define AI_MAX_FACE_INDICES 0x7fff
#endif

/** @def AI_MAX_BONE_WEIGHTS
 *  Maximum number of indices per face (polygon). */

#ifndef AI_MAX_BONE_WEIGHTS
#	define AI_MAX_BONE_WEIGHTS 0x7fffffff
#endif

/** @def AI_MAX_VERTICES
 *  Maximum number of vertices per mesh.  */

#ifndef AI_MAX_VERTICES
#	define AI_MAX_VERTICES 0x7fffffff
#endif

/** @def AI_MAX_FACES
 *  Maximum number of faces per mesh. */

#ifndef AI_MAX_FACES
#	define AI_MAX_FACES 0x7fffffff
#endif

/** @def AI_MAX_NUMBER_OF_COLOR_SETS
 *  Supported number of vertex color sets per mesh. */

#ifndef AI_MAX_NUMBER_OF_COLOR_SETS
#	define AI_MAX_NUMBER_OF_COLOR_SETS 0x8
#endif // !! AI_MAX_NUMBER_OF_COLOR_SETS

/** @def AI_MAX_NUMBER_OF_TEXTURECOORDS
 *  Supported number of texture coord sets (UV(W) channels) per mesh */

#ifndef AI_MAX_NUMBER_OF_TEXTURECOORDS
#	define AI_MAX_NUMBER_OF_TEXTURECOORDS 0x8
#endif // !! AI_MAX_NUMBER_OF_TEXTURECOORDS

// ---------------------------------------------------------------------------
/** @brief A single face in a mesh, referring to multiple vertices. 
 *
 * If mNumIndices is 3, we call the face 'triangle', for mNumIndices > 3 
 * it's called 'polygon' (hey, that's just a definition!).
 * <br>
 * aiMesh::mPrimitiveTypes can be queried to quickly examine which types of
 * primitive are actually present in a mesh. The #aiProcess_SortByPType flag 
 * executes a special post-processing algorithm which splits meshes with
 * *different* primitive types mixed up (e.g. lines and triangles) in several
 * 'clean' submeshes. Furthermore there is a configuration option (
 * #AI_CONFIG_PP_SBP_REMOVE) to force #aiProcess_SortByPType to remove 
 * specific kinds of primitives from the imported scene, completely and forever.
 * In many cases you'll probably want to set this setting to 
 * @code 
 * aiPrimitiveType_LINE|aiPrimitiveType_POINT
 * @endcode
 * Together with the #aiProcess_Triangulate flag you can then be sure that
 * #aiFace::mNumIndices is always 3. 
 * @note Take a look at the @link data Data Structures page @endlink for
 * more information on the layout and winding order of a face.
 */
struct aiFace
{
	//! Number of indices defining this face. 
	//! The maximum value for this member is #AI_MAX_FACE_INDICES.
	unsigned int mNumIndices; 

	//! Pointer to the indices array. Size of the array is given in numIndices.
	unsigned int* mIndices;   

#ifdef __cplusplus

	//! Default constructor
	aiFace()
	{
		mNumIndices = 0; mIndices = NULL;
	}

	//! Default destructor. Delete the index array
	~aiFace()
	{
		delete [] mIndices;
	}

	//! Copy constructor. Copy the index array
	aiFace( const aiFace& o)
	{
		mIndices = NULL;
		*this = o;
	}

	//! Assignment operator. Copy the index array
	const aiFace& operator = ( const aiFace& o)
	{
		if (&o == this)
			return *this;

		delete[] mIndices;
		mNumIndices = o.mNumIndices;
		mIndices = new unsigned int[mNumIndices];
		::memcpy( mIndices, o.mIndices, mNumIndices * sizeof( unsigned int));
		return *this;
	}

	//! Comparison operator. Checks whether the index array 
	//! of two faces is identical
	bool operator== (const aiFace& o) const
	{
		if (mIndices == o.mIndices)return true;
		else if (mIndices && mNumIndices == o.mNumIndices)
		{
			for (unsigned int i = 0;i < this->mNumIndices;++i)
				if (mIndices[i] != o.mIndices[i])return false;
			return true;
		}
		return false;
	}

	//! Inverse comparison operator. Checks whether the index 
	//! array of two faces is NOT identical
	bool operator != (const aiFace& o) const
	{
		return !(*this == o);
	}
#endif // __cplusplus
}; // struct aiFace


// ---------------------------------------------------------------------------
/** @brief A single influence of a bone on a vertex.
 */
struct aiVertexWeight
{
	//! Index of the vertex which is influenced by the bone.
	unsigned int mVertexId;

	//! The strength of the influence in the range (0...1).
	//! The influence from all bones at one vertex amounts to 1.
	float mWeight;     

#ifdef __cplusplus

	//! Default constructor
	aiVertexWeight() { }

	//! Initialisation from a given index and vertex weight factor
	//! \param pID ID
	//! \param pWeight Vertex weight factor
	aiVertexWeight( unsigned int pID, float pWeight) 
		: mVertexId( pID), mWeight( pWeight) 
	{ /* nothing to do here */ }

#endif // __cplusplus
};


// ---------------------------------------------------------------------------
/** @brief A single bone of a mesh.
 *
 *  A bone has a name by which it can be found in the frame hierarchy and by
 *  which it can be addressed by animations. In addition it has a number of 
 *  influences on vertices.
 */
struct aiBone
{
	//! The name of the bone. 
	C_STRUCT aiString mName;

	//! The number of vertices affected by this bone
	//! The maximum value for this member is #AI_MAX_BONE_WEIGHTS.
	unsigned int mNumWeights;

	//! The vertices affected by this bone
	C_STRUCT aiVertexWeight* mWeights;

	//! Matrix that transforms from mesh space to bone space in bind pose
	C_STRUCT aiMatrix4x4 mOffsetMatrix;

#ifdef __cplusplus

	//! Default constructor
	aiBone()
	{
		mNumWeights = 0; mWeights = NULL;
	}

	//! Copy constructor
	aiBone(const aiBone& other)
	{
		mNumWeights = other.mNumWeights;
		mOffsetMatrix = other.mOffsetMatrix;
		mName = other.mName;

		if (other.mWeights && other.mNumWeights)
		{
			mWeights = new aiVertexWeight[mNumWeights];
			::memcpy(mWeights,other.mWeights,mNumWeights * sizeof(aiVertexWeight));
		}
	}

	//! Destructor - deletes the array of vertex weights
	~aiBone()
	{
		delete [] mWeights;
	}
#endif // __cplusplus
};


// ---------------------------------------------------------------------------
/** @brief Enumerates the types of geometric primitives supported by Assimp.
 *  
 *  @see aiFace Face data structure
 *  @see aiProcess_SortByPType Per-primitive sorting of meshes
 *  @see aiProcess_Triangulate Automatic triangulation
 *  @see AI_CONFIG_PP_SBP_REMOVE Removal of specific primitive types.
 */
enum aiPrimitiveType
{
	/** A point primitive. 
	 *
	 * This is just a single vertex in the virtual world, 
	 * #aiFace contains just one index for such a primitive.
	 */
	aiPrimitiveType_POINT       = 0x1,

	/** A line primitive. 
	 *
	 * This is a line defined through a start and an end position.
	 * #aiFace contains exactly two indices for such a primitive.
	 */
	aiPrimitiveType_LINE        = 0x2,

	/** A triangular primitive. 
	 *
	 * A triangle consists of three indices.
	 */
	aiPrimitiveType_TRIANGLE    = 0x4,

	/** A higher-level polygon with more than 3 edges.
	 *
	 * A triangle is a polygon, but polygon in this context means
	 * "all polygons that are not triangles". The "Triangulate"-Step
	 * is provided for your convenience, it splits all polygons in
	 * triangles (which are much easier to handle).
	 */
	aiPrimitiveType_POLYGON     = 0x8,


	/** This value is not used. It is just here to force the
	 *  compiler to map this enum to a 32 Bit integer.
	 */
#ifndef SWIG
	_aiPrimitiveType_Force32Bit = 0x9fffffff
#endif
}; //! enum aiPrimitiveType

// Get the #aiPrimitiveType flag for a specific number of face indices
#define AI_PRIMITIVE_TYPE_FOR_N_INDICES(n) \
	((n) > 3 ? aiPrimitiveType_POLYGON : (aiPrimitiveType)(1u << ((n)-1)))



// ---------------------------------------------------------------------------
/** @brief NOT CURRENTLY IN USE. An AnimMesh is an attachment to an #aiMesh stores per-vertex 
 *  animations for a particular frame.
 *  
 *  You may think of an #aiAnimMesh as a `patch` for the host mesh, which
 *  replaces only certain vertex data streams at a particular time. 
 *  Each mesh stores n attached attached meshes (#aiMesh::mAnimMeshes).
 *  The actual relationship between the time line and anim meshes is 
 *  established by #aiMeshAnim, which references singular mesh attachments
 *  by their ID and binds them to a time offset.
*/
struct aiAnimMesh
{
	/** Replacement for aiMesh::mVertices. If this array is non-NULL, 
	 *  it *must* contain mNumVertices entries. The corresponding
	 *  array in the host mesh must be non-NULL as well - animation
	 *  meshes may neither add or nor remove vertex components (if
	 *  a replacement array is NULL and the corresponding source
	 *  array is not, the source data is taken instead)*/
	C_STRUCT aiVector3D* mVertices;

	/** Replacement for aiMesh::mNormals.  */
	C_STRUCT aiVector3D* mNormals;

	/** Replacement for aiMesh::mTangents. */
	C_STRUCT aiVector3D* mTangents;

	/** Replacement for aiMesh::mBitangents. */
	C_STRUCT aiVector3D* mBitangents;

	/** Replacement for aiMesh::mColors */
	C_STRUCT aiColor4D* mColors[AI_MAX_NUMBER_OF_COLOR_SETS];

	/** Replacement for aiMesh::mTextureCoords */
	C_STRUCT aiVector3D* mTextureCoords[AI_MAX_NUMBER_OF_TEXTURECOORDS];

	/** The number of vertices in the aiAnimMesh, and thus the length of all
	 * the member arrays.
	 *
	 * This has always the same value as the mNumVertices property in the
	 * corresponding aiMesh. It is duplicated here merely to make the length
	 * of the member arrays accessible even if the aiMesh is not known, e.g.
	 * from language bindings.
	 */
	unsigned int mNumVertices;

#ifdef __cplusplus

	aiAnimMesh()
		: mVertices()
		, mNormals()
		, mTangents()
		, mBitangents()
	{
		// fixme consider moving this to the ctor initializer list as well
		for( unsigned int a = 0; a < AI_MAX_NUMBER_OF_TEXTURECOORDS; a++){
			mTextureCoords[a] = NULL;
		}
		for( unsigned int a = 0; a < AI_MAX_NUMBER_OF_COLOR_SETS; a++) {
			mColors[a] = NULL;
		}
	}
	
	~aiAnimMesh()
	{
		delete [] mVertices; 
		delete [] mNormals;
		delete [] mTangents;
		delete [] mBitangents;
		for( unsigned int a = 0; a < AI_MAX_NUMBER_OF_TEXTURECOORDS; a++) {
			delete [] mTextureCoords[a];
		}
		for( unsigned int a = 0; a < AI_MAX_NUMBER_OF_COLOR_SETS; a++) {
			delete [] mColors[a];
		}
	}

	/** Check whether the anim mesh overrides the vertex positions 
	 *  of its host mesh*/ 
	bool HasPositions() const {
		return mVertices != NULL; 
	}

	/** Check whether the anim mesh overrides the vertex normals
	 *  of its host mesh*/ 
	bool HasNormals() const { 
		return mNormals != NULL; 
	}

	/** Check whether the anim mesh overrides the vertex tangents
	 *  and bitangents of its host mesh. As for aiMesh,
	 *  tangents and bitangents always go together. */ 
	bool HasTangentsAndBitangents() const { 
		return mTangents != NULL; 
	}

	/** Check whether the anim mesh overrides a particular
	 * set of vertex colors on his host mesh. 
	 *  @param pIndex 0<index<AI_MAX_NUMBER_OF_COLOR_SETS */ 
	bool HasVertexColors( unsigned int pIndex) const	{ 
		return pIndex >= AI_MAX_NUMBER_OF_COLOR_SETS ? false : mColors[pIndex] != NULL; 
	}

	/** Check whether the anim mesh overrides a particular
	 * set of texture coordinates on his host mesh. 
	 *  @param pIndex 0<index<AI_MAX_NUMBER_OF_TEXTURECOORDS */ 
	bool HasTextureCoords( unsigned int pIndex) const	{ 
		return pIndex >= AI_MAX_NUMBER_OF_TEXTURECOORDS ? false : mTextureCoords[pIndex] != NULL; 
	}

#endif
};


// ---------------------------------------------------------------------------
/** @brief A mesh represents a geometry or model with a single material. 
*
* It usually consists of a number of vertices and a series of primitives/faces 
* referencing the vertices. In addition there might be a series of bones, each 
* of them addressing a number of vertices with a certain weight. Vertex data 
* is presented in channels with each channel containing a single per-vertex 
* information such as a set of texture coords or a normal vector.
* If a data pointer is non-null, the corresponding data stream is present.
* From C++-programs you can also use the comfort functions Has*() to
* test for the presence of various data streams.
*
* A Mesh uses only a single material which is referenced by a material ID.
* @note The mPositions member is usually not optional. However, vertex positions 
* *could* be missing if the #AI_SCENE_FLAGS_INCOMPLETE flag is set in 
* @code
* aiScene::mFlags
* @endcode
*/
struct aiMesh
{
	/** Bitwise combination of the members of the #aiPrimitiveType enum.
	 * This specifies which types of primitives are present in the mesh.
	 * The "SortByPrimitiveType"-Step can be used to make sure the 
	 * output meshes consist of one primitive type each.
	 */
	unsigned int mPrimitiveTypes;

	/** The number of vertices in this mesh. 
	* This is also the size of all of the per-vertex data arrays.
	* The maximum value for this member is #AI_MAX_VERTICES.
	*/
	unsigned int mNumVertices;

	/** The number of primitives (triangles, polygons, lines) in this  mesh. 
	* This is also the size of the mFaces array.
	* The maximum value for this member is #AI_MAX_FACES.
	*/
	unsigned int mNumFaces;

	/** Vertex positions. 
	* This array is always present in a mesh. The array is 
	* mNumVertices in size. 
	*/
	C_STRUCT aiVector3D* mVertices;

	/** Vertex normals. 
	* The array contains normalized vectors, NULL if not present. 
	* The array is mNumVertices in size. Normals are undefined for
	* point and line primitives. A mesh consisting of points and
	* lines only may not have normal vectors. Meshes with mixed
	* primitive types (i.e. lines and triangles) may have normals,
	* but the normals for vertices that are only referenced by
	* point or line primitives are undefined and set to QNaN (WARN:
	* qNaN compares to inequal to *everything*, even to qNaN itself.
	* Using code like this to check whether a field is qnan is:
	* @code
	* #define IS_QNAN(f) (f != f)
	* @endcode
	* still dangerous because even 1.f == 1.f could evaluate to false! (
	* remember the subtleties of IEEE754 artithmetics). Use stuff like
	* @c fpclassify instead.
	* @note Normal vectors computed by Assimp are always unit-length.
	* However, this needn't apply for normals that have been taken
	*   directly from the model file.
	*/
    C_STRUCT aiVector3D* mNormals;

	/** Vertex tangents. 
	* The tangent of a vertex points in the direction of the positive 
	* X texture axis. The array contains normalized vectors, NULL if
	* not present. The array is mNumVertices in size. A mesh consisting 
	* of points and lines only may not have normal vectors. Meshes with 
	* mixed primitive types (i.e. lines and triangles) may have 
	* normals, but the normals for vertices that are only referenced by
	* point or line primitives are undefined and set to qNaN.  See
	* the #mNormals member for a detailled discussion of qNaNs.
	* @note If the mesh contains tangents, it automatically also 
	* contains bitangents.
	*/
	C_STRUCT aiVector3D* mTangents;

	/** Vertex bitangents. 
	* The bitangent of a vertex points in the direction of the positive 
	* Y texture axis. The array contains normalized vectors, NULL if not
	* present. The array is mNumVertices in size. 
	* @note If the mesh contains tangents, it automatically also contains
	* bitangents.  
	*/
	C_STRUCT aiVector3D* mBitangents;

	/** Vertex color sets. 
	* A mesh may contain 0 to #AI_MAX_NUMBER_OF_COLOR_SETS vertex 
	* colors per vertex. NULL if not present. Each array is
	* mNumVertices in size if present.
	*/
	C_STRUCT aiColor4D* mColors[AI_MAX_NUMBER_OF_COLOR_SETS];

	/** Vertex texture coords, also known as UV channels.
	* A mesh may contain 0 to AI_MAX_NUMBER_OF_TEXTURECOORDS per
	* vertex. NULL if not present. The array is mNumVertices in size. 
	*/
	C_STRUCT aiVector3D* mTextureCoords[AI_MAX_NUMBER_OF_TEXTURECOORDS];

	/** Specifies the number of components for a given UV channel.
	* Up to three channels are supported (UVW, for accessing volume
	* or cube maps). If the value is 2 for a given channel n, the
	* component p.z of mTextureCoords[n][p] is set to 0.0f.
	* If the value is 1 for a given channel, p.y is set to 0.0f, too.
	* @note 4D coords are not supported 
	*/
	unsigned int mNumUVComponents[AI_MAX_NUMBER_OF_TEXTURECOORDS];

	/** The faces the mesh is constructed from. 
	* Each face refers to a number of vertices by their indices. 
	* This array is always present in a mesh, its size is given 
	* in mNumFaces. If the #AI_SCENE_FLAGS_NON_VERBOSE_FORMAT
	* is NOT set each face references an unique set of vertices.
	*/
	C_STRUCT aiFace* mFaces;

	/** The number of bones this mesh contains. 
	* Can be 0, in which case the mBones array is NULL. 
	*/
	unsigned int mNumBones;

	/** The bones of this mesh. 
	* A bone consists of a name by which it can be found in the
	* frame hierarchy and a set of vertex weights.
	*/
	C_STRUCT aiBone** mBones;

	/** The material used by this mesh. 
	 * A mesh does use only a single material. If an imported model uses
	 * multiple materials, the import splits up the mesh. Use this value 
	 * as index into the scene's material list.
	 */
	unsigned int mMaterialIndex;


    /** The material groups of this mesh
     * A mech can now containt more than one material.
     * mMaterialGroups enumerate the number of vertices for each group.
     * mGroupsMaterialIndex contains an materialIndex for each group.
     */
    unsigned int mNumMaterialGroups;
    C_STRUCT unsigned int * mMaterialGroups;
    C_STRUCT unsigned int * mGroupsMaterialIndex;

	/** Name of the mesh. Meshes can be named, but this is not a
	 *  requirement and leaving this field empty is totally fine.
	 *  There are mainly three uses for mesh names: 
	 *   - some formats name nodes and meshes independently.
	 *   - importers tend to split meshes up to meet the
	 *      one-material-per-mesh requirement. Assigning
	 *      the same (dummy) name to each of the result meshes
	 *      aids the caller at recovering the original mesh
	 *      partitioning.
	 *   - Vertex animations refer to meshes by their names.
	 **/
	C_STRUCT aiString mName;


	/** NOT CURRENTLY IN USE. The number of attachment meshes */
	unsigned int mNumAnimMeshes;

	/** NOT CURRENTLY IN USE. Attachment meshes for this mesh, for vertex-based animation. 
	 *  Attachment meshes carry replacement data for some of the
	 *  mesh'es vertex components (usually positions, normals). */
	C_STRUCT aiAnimMesh** mAnimMeshes;

    // Zykl.io begin
    bool isVisualMesh;
    // Zykl.io end

#ifdef __cplusplus

	//! Default constructor. Initializes all members to 0
	aiMesh()
	{
		mNumVertices    = 0; 
		mNumFaces       = 0;

		mNumAnimMeshes = 0;

		mPrimitiveTypes = 0;
		mVertices = NULL; mFaces    = NULL;
		mNormals  = NULL; mTangents = NULL;
		mBitangents = NULL;
		mAnimMeshes = NULL;

		for( unsigned int a = 0; a < AI_MAX_NUMBER_OF_TEXTURECOORDS; a++)
		{
			mNumUVComponents[a] = 0;
			mTextureCoords[a] = NULL;
		}
		for( unsigned int a = 0; a < AI_MAX_NUMBER_OF_COLOR_SETS; a++)
			mColors[a] = NULL;
		mNumBones = 0; mBones = NULL;
		mMaterialIndex = 0;
		mNumAnimMeshes = 0;
		mAnimMeshes = NULL;

        mNumMaterialGroups = 0;
        mMaterialGroups = NULL;
        mGroupsMaterialIndex = NULL;

        // Zykl.io begin
        isVisualMesh = false;
        // Zykl.io end
	}

	//! Deletes all storage allocated for the mesh
	~aiMesh()
	{
		delete [] mVertices; 
		delete [] mNormals;
		delete [] mTangents;
		delete [] mBitangents;
        delete [] mMaterialGroups;
        delete [] mGroupsMaterialIndex;

		for( unsigned int a = 0; a < AI_MAX_NUMBER_OF_TEXTURECOORDS; a++) {
			delete [] mTextureCoords[a];
		}
		for( unsigned int a = 0; a < AI_MAX_NUMBER_OF_COLOR_SETS; a++) {
			delete [] mColors[a];
		}

		// DO NOT REMOVE THIS ADDITIONAL CHECK
		if (mNumBones && mBones)	{
			for( unsigned int a = 0; a < mNumBones; a++) {
				delete mBones[a];
			}
			delete [] mBones;
		}

		if (mNumAnimMeshes && mAnimMeshes)	{
			for( unsigned int a = 0; a < mNumAnimMeshes; a++) {
				delete mAnimMeshes[a];
			}
			delete [] mAnimMeshes;
		}

		delete [] mFaces;
	}

	//! Check whether the mesh contains positions. Provided no special
	//! scene flags are set (such as #AI_SCENE_FLAGS_ANIM_SKELETON_ONLY), 
	//! this will always be true 
	bool HasPositions() const 
		{ return mVertices != NULL && mNumVertices > 0; }

	//! Check whether the mesh contains faces. If no special scene flags
	//! are set this should always return true
	bool HasFaces() const 
		{ return mFaces != NULL && mNumFaces > 0; }

	//! Check whether the mesh contains normal vectors
	bool HasNormals() const 
		{ return mNormals != NULL && mNumVertices > 0; }

	//! Check whether the mesh contains tangent and bitangent vectors
	//! It is not possible that it contains tangents and no bitangents
	//! (or the other way round). The existence of one of them
	//! implies that the second is there, too.
	bool HasTangentsAndBitangents() const 
		{ return mTangents != NULL && mBitangents != NULL && mNumVertices > 0; }

	//! Check whether the mesh contains a vertex color set
	//! \param pIndex Index of the vertex color set
	bool HasVertexColors( unsigned int pIndex) const
	{ 
		if( pIndex >= AI_MAX_NUMBER_OF_COLOR_SETS) 
			return false; 
		else 
			return mColors[pIndex] != NULL && mNumVertices > 0; 
	}

	//! Check whether the mesh contains a texture coordinate set
	//! \param pIndex Index of the texture coordinates set
	bool HasTextureCoords( unsigned int pIndex) const
	{ 
		if( pIndex >= AI_MAX_NUMBER_OF_TEXTURECOORDS) 
			return false; 
		else 
			return mTextureCoords[pIndex] != NULL && mNumVertices > 0; 
	}

	//! Get the number of UV channels the mesh contains
	unsigned int GetNumUVChannels() const 
	{
		unsigned int n = 0;
		while (n < AI_MAX_NUMBER_OF_TEXTURECOORDS && mTextureCoords[n])++n;
		return n;
	}

	//! Get the number of vertex color channels the mesh contains
	unsigned int GetNumColorChannels() const 
	{
		unsigned int n = 0;
		while (n < AI_MAX_NUMBER_OF_COLOR_SETS && mColors[n])++n;
		return n;
	}

	//! Check whether the mesh contains bones
	inline bool HasBones() const
		{ return mBones != NULL && mNumBones > 0; }

    friend std::ostream& operator << (std::ostream& os, const aiMesh& mesh) {
        os << "aiMesh - faces/vertices: " << mesh.mNumFaces << "/" << mesh.mNumVertices;
        return os;
    }

#endif // __cplusplus
};


struct aiRigidBodyConstraint {
    aiString mName;
    bool mInterpenetrate;
    aiString mObject1;
    aiString mObject2;
    aiMatrix4x4 mObject1Transform;
    aiMatrix4x4 mObject2Transform;
    aiVector3D mMin;
    aiVector3D mMax;
    enum Type {
        TRANSLATION,
        ROTATION,
    } mType;
    double mPos0;
    double mPos1;
    double mDelta_t;
    int mBinaryIO;
    bool mForceSensitive;
};

struct aiPhysicsModel {

    //! The name of the connected geometry
    C_STRUCT aiString mMeshId;

    C_STRUCT aiString mSid;

    C_STRUCT aiString mParentId;

    bool isGhost;
    double tolerance; // for ghost only

    //! The mass of the object
    float mass;

    //! The Dynamic flag: true=use physics on this object; false=static object
    bool dynamic;

    C_STRUCT aiVector3D mMassFrame;

    C_STRUCT aiMatrix4x4 mTransform;

    C_STRUCT aiMatrix4x4 mToolTransform;

    bool isTool;

    aiString mToolId;

    unsigned int mNumWhitelist;
    aiString** mWhitelist;

    // Zykl.io begin
    aiString* mVisualModel;
    // Zykl.io end

    unsigned int mNumSubModels;

    aiPhysicsModel** mSubModels;

    aiPhysicsModel()
    {
        mNumSubModels = 0;
        mNumWhitelist = 0;
        mSubModels = NULL;
        mWhitelist = NULL;
        // Zykl.io begin
        mVisualModel = NULL;
        // Zykl.io end
        dynamic = false;
        mass = 0.0;
        isGhost = false;
        tolerance = 0.0;
    }

    ~aiPhysicsModel() {
        if (mNumSubModels && mSubModels)	{
            for( unsigned int a = 0; a < mNumSubModels; a++) {
                delete mSubModels[a];
            }
            delete [] mSubModels;
        }
        mNumSubModels = 0;

        if (mNumWhitelist && mWhitelist) {
            for (unsigned int a = 0; a < mNumWhitelist; a++) {
                delete mWhitelist[a];
            }
            delete [] mWhitelist;
        }
        mNumWhitelist = 0;

        // Zykl.io begin
        if (mVisualModel) {
            delete mVisualModel;
        }
        // Zykl.io end
    }
};





// ---------------------------------------------------------------------------
/** @brief A single joint
 *
 *
 */
struct aiJoint
{
    //! The name of the joint.
    C_STRUCT aiString mName;

    //! The id of the joint.
    C_STRUCT aiString mId;

    //! The name of the object moved by the joint.
    C_STRUCT aiString mObjName;

    //! The id of the corresponding kinematic scene
    C_STRUCT aiString mKinematicsId;

    //! The name of the corresponding kinematic scene
    C_STRUCT aiString mKinematicsName;

    aiRigidBodyConstraint * mConstraint;

    bool isRobot;
    bool isToolAttached;  // True when this is the endpoint where a tool is attached
    bool isToolJoint;     // True when this joint belongs to a tool

    C_STRUCT aiString mToolId;
    C_STRUCT aiMatrix4x4 mToolTransform;

    //! The Absolute Translation of the joint
    C_STRUCT aiVector3D mTranslation;
    C_STRUCT aiQuaternion mRotationQuat;

    //! The Relative Translation of the joint
    C_STRUCT aiVector3D mTranslationRel;


    //! The free axis for this joint
    C_STRUCT aiVector3D mAxis;

    enum JointAxis {
        X_AXIS,
        Y_AXIS,
        Z_AXIS,
        UNDEFINED,
    };

    JointAxis getAxis() {
        if (fabs(fabs(mAxis.x) - 1) < FLT_EPSILON) return X_AXIS;
        if (fabs(fabs(mAxis.y) - 1) < FLT_EPSILON) return Y_AXIS;
        if (fabs(fabs(mAxis.z) - 1) < FLT_EPSILON) return Z_AXIS;
        return UNDEFINED;
    }


    bool isInverted() {

        // Hacky way (don't use this):  return (&mAxis.x)[(int)getAxis()] < 0.0;
        switch (getAxis()) {
        case X_AXIS:
            return (mAxis.x < 0.0);
        case Y_AXIS:
            return (mAxis.y < 0.0);
        case Z_AXIS:
            return (mAxis.z < 0.0);
        default:
            return false;
        }
    }

    enum JointType {
        REVOLUTE,
        PRISMATIC,
    } mType;

    //! Limit Min
    double mMin;

    //! Limit Max
    double mMax;

    //! Value
    double mValue;

    //!
    C_STRUCT aiString mPreLinkId;

    int mNumPostLinkIds;
    C_STRUCT aiString **mPostLinkIds;

    C_STRUCT aiString mLinkId;



#ifdef __cplusplus

    //! Default constructor
    aiJoint()
    {
        mMin = -DBL_MAX;
        mMax = DBL_MAX;
        mValue = 0;
        mNumPostLinkIds = 0;
        isToolAttached = false;
        isToolJoint = false;
        isRobot = false;
        mConstraint = NULL;
    }

    //! Copy constructor
    aiJoint(const aiJoint& other)
    {
        mName = other.mName;
        mId = other.mId;
        mObjName = other.mObjName;
        mAxis = other.mAxis;
        mMin = other.mMin;
        mMax = other.mMax;
        mTranslationRel = other.mTranslationRel;
        mTranslation = other.mTranslation;
        mLinkId = other.mLinkId;
        mPreLinkId = other.mPreLinkId;
        mValue = other.mValue;
        mNumPostLinkIds = other.mNumPostLinkIds;

        mPostLinkIds = new aiString*[mNumPostLinkIds];
        for (int i = 0; i < mNumPostLinkIds; i++) {
            mPostLinkIds[i] = new aiString(*other.mPostLinkIds[i]);
        }
    }

    //! Destructor
    ~aiJoint()
    {
        if (mNumPostLinkIds && mPostLinkIds)	{
            for(int a = 0; a < mNumPostLinkIds; a++) {
                if (mPostLinkIds[a]) delete mPostLinkIds[a];
            }
            delete [] mPostLinkIds;
        }
        mNumPostLinkIds = 0;
    }


#endif // __cplusplus
};




#ifdef __cplusplus


}
#endif //! extern "C"
#endif // __AI_MESH_H_INC

