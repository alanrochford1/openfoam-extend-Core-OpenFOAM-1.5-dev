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
    cohesiveFvPatch

Description
    Cohesive fv patch.

SourceFiles
    cohesiveFvPatch.C

\*---------------------------------------------------------------------------*/

#ifndef cohesiveFvPatch_H
#define cohesiveFvPatch_H

#include "fvPatch.H"
#include "cyclicLduInterface.H"
#include "cohesivePolyPatch.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

/*---------------------------------------------------------------------------*\
                           Class cohesiveFvPatch Declaration
\*---------------------------------------------------------------------------*/

class cohesiveFvPatch
:
    public fvPatch
{
    // Private data

        const cohesivePolyPatch& cohesivePolyPatch_;


protected:

    // Protected Member Functions

        //- Make patch face non-orthogonality correction vectors
        virtual void makeCorrVecs(vectorField&) const;


public:

    //- Runtime type information
    TypeName(cohesivePolyPatch::typeName_());


    // Constructors

        //- Construct from polyPatch
        cohesiveFvPatch
        (
            const polyPatch& patch, 
            const fvBoundaryMesh& bm
        )
        :
            fvPatch(patch, bm),
            cohesivePolyPatch_
            (
                refCast<const cohesivePolyPatch>(patch)
            )
        {}


    // Destructor

        virtual ~cohesiveFvPatch()
        {}
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
