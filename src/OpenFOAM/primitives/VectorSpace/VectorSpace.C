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

\*---------------------------------------------------------------------------*/

#include "VectorSpace.H"
#include "IOstreams.H"

#include <sstream>

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

// Construct from Istream
template<class Form, class Cmpt, int nCmpt>
VectorSpace<Form, Cmpt, nCmpt>::VectorSpace
(
    Istream& is
)
{
    // Read beginning of VectorSpace<Cmpt>
    is.readBegin("VectorSpace<Form, Cmpt, nCmpt>");

    for (int i=0; i<nCmpt; i++)
    {
        is >> v_[i];
    }

    // Read end of VectorSpace<Cmpt>
    is.readEnd("VectorSpace<Form, Cmpt, nCmpt>");

    // Check state of Istream
    is.check("VectorSpace<Form, Cmpt, nCmpt>::VectorSpace(Istream& is)");
}


//- Return a string representation
template<class Form, class Cmpt, int nCmpt>
word name
(
    const VectorSpace<Form, Cmpt, nCmpt>& vs
)
{
    std::ostringstream osBuffer;

    osBuffer << '(';

    for (int i=0; i<nCmpt-1; i++)
    {
        osBuffer << vs.v_[i] << ',';
    }

    osBuffer << vs.v_[nCmpt-1] << ')';

    return osBuffer.str();
}


// * * * * * * * * * * * * * * * IOstream Operators  * * * * * * * * * * * * //

template<class Form, class Cmpt, int nCmpt>
Istream& operator>>
(
    Istream& is,
    VectorSpace<Form, Cmpt, nCmpt>& vs
)
{
    // Read beginning of VectorSpace<Cmpt, nCmpt>
    is.readBegin("VectorSpace<Form, Cmpt, nCmpt>");

    for (int i=0; i<nCmpt; i++)
    {
        is >> vs.v_[i];
    }

    // Read end of VectorSpace<Cmpt, nCmpt>
    is.readEnd("VectorSpace<Form, Cmpt, nCmpt>");

    // Check state of Istream
    is.check("operator>>(Istream&, VectorSpace<Form, Cmpt, nCmpt>&)");

    return is;
}


template<class Form, class Cmpt, int nCmpt>
Ostream& operator<<
(
    Ostream& os,
    const VectorSpace<Form, Cmpt, nCmpt>& vs
)
{
    os << token::BEGIN_LIST;

    for (int i=0; i<nCmpt-1; i++)
    {
        os << vs.v_[i] << token::SPACE;
    }

    os << vs.v_[nCmpt-1] << token::END_LIST;

    // Check state of Ostream
    os.check("operator<<(Ostream&, const VectorSpace<Form, Cmpt, nCmpt>&)");

    return os;
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// ************************************************************************* //
