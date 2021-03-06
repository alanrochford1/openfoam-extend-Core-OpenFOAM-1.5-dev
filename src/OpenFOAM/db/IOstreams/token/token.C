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

#include "token.H"

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

const char* const Foam::token::typeName = "token";
Foam::token Foam::token::undefinedToken;

defineTypeNameAndDebug(Foam::token::compound, 0);
defineRunTimeSelectionTable(Foam::token::compound, Istream);


// * * * * * * * * * * * * Private Member Functions  * * * * * * * * * * * * //

void Foam::token::parseError(const char* expected) const
{
    FatalIOError
        << "Parse error, expected a " << expected
        << ", found \n    " << info() << endl;
}


// * * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * * //

Foam::token::compound::~compound()
{}

// * * * * * * * * * * * * * * * * Selectors * * * * * * * * * * * * * * * * //

Foam::autoPtr<Foam::token::compound> Foam::token::compound::New
(
    const Foam::word& compoundType,
    Foam::Istream& is
)
{
    IstreamConstructorTable::iterator cstrIter =
        IstreamConstructorTablePtr_->find(compoundType);

    if (cstrIter == IstreamConstructorTablePtr_->end())
    {
        FatalErrorIn("token::compound::New(Istream&)")
            << "Unknown compound type " << compoundType
            << endl << endl
            << "Valid compound types are :" << endl
            << IstreamConstructorTablePtr_->toc()
            << abort(FatalError);
    }

    return autoPtr<Foam::token::compound>(cstrIter()(is));
}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

bool Foam::token::compound::isCompound(const Foam::word& name)
{
    return
    (
        IstreamConstructorTablePtr_
     && IstreamConstructorTablePtr_->found(name)
    );
}


Foam::token::compound& Foam::token::transferCompoundToken()
{
    if (type_ == COMPOUND)
    {
        if (compoundTokenPtr_->empty())
        {
            FatalErrorIn("token::transferCompoundToken()")
                << "compound has already been transfered from token\n    "
                << info() << abort(FatalError);
        }
        else
        {
            compoundTokenPtr_->empty() = true;
        }

        return *compoundTokenPtr_;
    }
    else
    {
        parseError("compound");
        return *compoundTokenPtr_;
    }
}


// ************************************************************************* //
