/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | Copyright held by original author
     \\/     M anipulation  |
-------------------------------------------------------------------------------
License
    This file is part of OpenFOAM.

    OpenFOAM is free software; you can redistribute it and/or modify it
    under the terms of the GNU General Public License as published by the
    Free Software Foundation; either version 2 of the License, or (at your
    option) any later version.

    OpenFOAM is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
    for more details.

    You should have received a copy of the GNU General Public License
    along with OpenFOAM; if not, write to the Free Software Foundation,
    Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA

Class
    tetPointMapperCellDecomp

Description
    Point mapper for the face tetFem decomposition

SourceFiles
    tetPointMapperCellDecomp.C

\*---------------------------------------------------------------------------*/

#ifndef tetPointMapperCellDecomp_H
#define tetPointMapperCellDecomp_H

#include "morphFieldMapper.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

// Class forward declarations
class tetPolyMeshCellDecomp;
class mapPolyMesh;
class pointMapper;
class cellMapper;

/*---------------------------------------------------------------------------*\
                   Class tetPointMapperCellDecomp Declaration
\*---------------------------------------------------------------------------*/

class tetPointMapperCellDecomp
:
    public morphFieldMapper
{
    // Private data

        //- Reference to mesh
        const tetPolyMeshCellDecomp& mesh_;

        //- Reference to mapPolyMesh
        const mapPolyMesh& mpm_;

        //- Reference to point mapper
        const pointMapper& pointMap_;

        //- Reference to face mapper
        const cellMapper& cellMap_;


        //- Size of target object
        const label size_;


    // Demand-driven private data

        //- Is the mapping direct
        mutable bool* directPtr_;

        //- Direct addressing (only one for of addressing is used)
        mutable labelList* directAddrPtr_;

        //- Interpolated addressing (only one for of addressing is used)
        mutable labelListList* interpolationAddrPtr_;

        //- Interpolation weights
        mutable scalarListList* weightsPtr_;

        //- Are there any inserted (unmapped) objects
        mutable bool* insertedObjectsPtr_;

        //- Inserted faces
        mutable labelList* insertedObjectLabelsPtr_;



    // Private Member Functions

        //- Disallow default bitwise copy construct
        tetPointMapperCellDecomp(const tetPointMapperCellDecomp&);

        //- Disallow default bitwise assignment
        void operator=(const tetPointMapperCellDecomp&);


        //- Calculate addressing
        void calcAddressing() const;

        //- Clear out local storage
        void clearOut();


public:

    // Constructors

        //- Construct from components
        tetPointMapperCellDecomp
        (
            const tetPolyMeshCellDecomp& mesh,
            const mapPolyMesh& meshMap,
            const pointMapper& pMapper,
            const cellMapper& cMapper
        );


    // Destructor

        virtual ~tetPointMapperCellDecomp();


    // Member Functions

        //- Return size
        virtual label size() const;

        //- Return size of field before mapping
        virtual label sizeBeforeMapping() const;

        //- Is the mapping direct
        virtual bool direct() const;

        //- Return direct addressing
        virtual const unallocLabelList& directAddressing() const;

        //- Return interpolated addressing
        virtual const labelListList& addressing() const;

        //- Return interpolaion weights
        virtual const scalarListList& weights() const;

        //- Are there any inserted faces
        bool insertedObjects() const;

        //- Return list of inserted faces
        const labelList& insertedObjectLabels() const;
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
