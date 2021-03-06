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
    Foam::inverseVolumeDiffusivity

Description
    Inverse cell-volume motion diffusivity.

SourceFiles
    inverseVolumeDiffusivity.C

\*---------------------------------------------------------------------------*/

#ifndef inverseVolumeDiffusivity_H
#define inverseVolumeDiffusivity_H

#include "uniformDiffusivity.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

/*---------------------------------------------------------------------------*\
                           Class inverseVolumeDiffusivity Declaration
\*---------------------------------------------------------------------------*/

class inverseVolumeDiffusivity
:
    public uniformDiffusivity
{
    // Private Member Functions

        //- Disallow default bitwise copy construct
        inverseVolumeDiffusivity(const inverseVolumeDiffusivity&);

        //- Disallow default bitwise assignment
        void operator=(const inverseVolumeDiffusivity&);


public:

    //- Runtime type information
    TypeName("inverseVolume");


    // Constructors

        //- Construct for the given fvMotionSolver and data Istream
        inverseVolumeDiffusivity
        (
            const fvMotionSolver& mSolver,
            Istream& mdData
        );


    // Destructor

        virtual ~inverseVolumeDiffusivity();


    // Member Functions

        //- Correct the motion diffusivity
        virtual void correct();
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
