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
    Foam::edge

Description
    An edge is a list of two point labels. The functionality it provides
    supports the discretisation on a 2-D flat mesh.

SourceFiles
    edgeI.H

\*---------------------------------------------------------------------------*/

#ifndef edge_H
#define edge_H

#include "FixedList.H"
#include "pointField.H"
#include "linePointRef.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

// Forward declaration of friend functions and operators

class edge;
inline bool operator==(const edge& a, const edge& b);
inline bool operator!=(const edge& a, const edge& b);


/*---------------------------------------------------------------------------*\
                           Class edge Declaration
\*---------------------------------------------------------------------------*/

class edge
:
    public FixedList<label, 2>
{

public:

    // Static data members

        static const char* const typeName;


    // Constructors

        //- Null constructor for lists
        inline edge();

        //- Construct from components
        inline edge(const label a, const label b);

        //- Construct from FixedList
        inline edge(const FixedList<label, 2>&);

        //- Construct from Istream
        inline edge(Istream&);


    // Member Functions

        //- Return start vertex label
        inline label start() const;

        //- Return start vertex label
        inline label& start();

        //- Return end vertex label
        inline label end() const;

        //- Return end vertex label
        inline label& end();

        //- Given one vertex, return the other
        inline label otherVertex(const label a) const;

        //- Return common vertex
        inline label commonVertex(const edge& a) const;

        //- Return reverse edge
        inline edge reverseEdge() const;

        //- Return centre (centroid)
        inline point centre(const pointField&) const;

        //- Return the vector (end - start)
        inline vector vec(const pointField&) const;

        //- Return scalar magnitude
        inline scalar mag(const pointField&) const;

        //- Return edge line
        inline linePointRef line(const pointField&) const;

        //- compare edges
        //  -  0: different
        //  - +1: identical
        //  - -1: same edge, but different orientation
        static inline int compare(const edge&, const edge&);


    // Friend Operators

        friend bool operator==(const edge& a, const edge& b);
        friend bool operator!=(const edge& a, const edge& b);
};


//- Hash<edge> specialisation
//  Simple commutative hash.
template<>
inline label Hash<edge>::operator()(const edge& e) const
{
    return e[0]*e[1] + e[0]+e[1];
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#include "edgeI.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
