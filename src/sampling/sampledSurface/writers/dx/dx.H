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
    Foam::dx

Description

SourceFiles
    dx.C

\*---------------------------------------------------------------------------*/

#ifndef dx_H
#define dx_H

#include "surfaceWriter.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

/*---------------------------------------------------------------------------*\
                           Class dx Declaration
\*---------------------------------------------------------------------------*/

template<class Type>
class dx
:
    public surfaceWriter<Type>
{

    // Private Member Functions

        void writeDXGeometry
        (
            const pointField& points,
            const faceList& faces,
            Ostream& os
        ) const;

        void writeDXData
        (
            const pointField& points,
            const scalarField& values,
            Ostream& os
        ) const;

        void writeDXData
        (
            const pointField& points,
            const vectorField& values,
            Ostream& os
        ) const;

        void writeDXData
        (
            const pointField& points,
            const sphericalTensorField& values,
            Ostream& os
        ) const;

        void writeDXData
        (
            const pointField& points,
            const symmTensorField& values,
            Ostream& os
        ) const;

        void writeDXData
        (
            const pointField& points,
            const tensorField& values,
            Ostream& os
        ) const;

        void writeDXTrailer(Ostream& os) const;

public:

    //- Runtime type information
    TypeName("dx");


    // Constructors

        //- Construct null
        dx();


    // Destructor

        virtual ~dx();


    // Member Functions

        // Write

        //- Writes single surface to file.
        virtual void write
        (
            const fileName& samplePath,
            const fileName& timeDir,
            const fileName& surfaceName,
            const pointField& points,
            const faceList& faces,
            const fileName& fieldName,
            const Field<Type>& values,
            const bool verbose = false
        ) const;
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#ifdef NoRepository
#   include "dx.C"
#endif

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
