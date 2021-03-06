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
    Foam::rawSurfaceWriter

Description

SourceFiles
    rawSurfaceWriter.C

\*---------------------------------------------------------------------------*/

#ifndef rawSurfaceWriter_H
#define rawSurfaceWriter_H

#include "surfaceWriter.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

/*---------------------------------------------------------------------------*\
                           Class raw Declaration
\*---------------------------------------------------------------------------*/

template<class Type>
class rawSurfaceWriter
:
    public surfaceWriter<Type>
{
    // Private Member Functions

        void writeGeometry
        (
            const pointField& points,
            const label& pointI,
            Ostream& os
        ) const;

        void writeGeometry
        (
            const pointField& points,
            const faceList& faces,
            const label& faceI,
            Ostream& os
        ) const;

        void writeData
        (
            const fileName& fieldName,
            const pointField& points,
            const faceList& faces,
            const scalarField& values,
            Ostream& os
        ) const;

        void writeData
        (
            const fileName& fieldName,
            const pointField& points,
            const faceList& faces,
            const vectorField& values,
            Ostream& os
        ) const;

        void writeData
        (
            const fileName& fieldName,
            const pointField& points,
            const faceList& faces,
            const sphericalTensorField& values,
            Ostream& os
        ) const;

        void writeData
        (
            const fileName& fieldName,
            const pointField& points,
            const faceList& faces,
            const symmTensorField& values,
            Ostream& os
        ) const;

        void writeData
        (
            const fileName& fieldName,
            const pointField& points,
            const faceList& faces,
            const tensorField& values,
            Ostream& os
        ) const;


public:

    //- Runtime type information
    TypeName("raw");


    // Constructors

        //- Construct null
        rawSurfaceWriter();


    // Destructor

        virtual ~rawSurfaceWriter();


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
#   include "rawSurfaceWriter.C"
#endif

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
