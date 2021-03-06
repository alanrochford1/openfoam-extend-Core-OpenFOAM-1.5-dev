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

#include "primitiveEntry.H"
#include "dictionary.H"

// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

Foam::primitiveEntry::primitiveEntry(const word& key, const ITstream& tokens)
:
    entry(key),
    ITstream(tokens)
{
    name() += "::" + keyword();
}


Foam::primitiveEntry::primitiveEntry(const word& keyword, const token& t)
:
    entry(keyword),
    ITstream(keyword, tokenList(1, t))
{}


Foam::primitiveEntry::primitiveEntry
(
    const word& keyword,
    const tokenList& tokens
)
:
    entry(keyword),
    ITstream(keyword, tokens)
{}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

Foam::label Foam::primitiveEntry::startLineNumber() const
{
    if (size())
    {
        return operator[](0).lineNumber();
    }
    else
    {
        return -1;
    }
}

Foam::label Foam::primitiveEntry::endLineNumber() const
{
    if (size())
    {
        return operator[](size()-1).lineNumber();
    }
    else
    {
        return -1;
    }
}


Foam::ITstream& Foam::primitiveEntry::stream() const
{
    ITstream& dataStream = const_cast<primitiveEntry&>(*this);
    dataStream.rewind();
    return dataStream;
}


const Foam::dictionary& Foam::primitiveEntry::dict() const
{
    FatalErrorIn("const dictionary& primitiveEntry::dict() const")
        << "Attempt to return primitive entry " << info()
        << " as a sub-dictionary"
        << abort(FatalError);

    return dictionary::null;
}


Foam::dictionary& Foam::primitiveEntry::dict()
{
    FatalErrorIn("const dictionary& primitiveEntry::dict()")
        << "Attempt to return primitive entry " << info()
        << " as a sub-dictionary"
        << abort(FatalError);

    return const_cast<dictionary&>(dictionary::null);
}


void Foam::primitiveEntry::insert(const tokenList& varTokens, const label i)
{
    tokenList& tokens = *this;

    if (!varTokens.size())
    {
        label end = tokens.size() - 1;

        for(label j=i; j<end; j++)
        {
            tokens[j] = tokens[j+1];
        }

        tokens.setSize(tokens.size() - 1);
    }
    else if (varTokens.size() > 1)
    {
        tokens.setSize(tokens.size() + varTokens.size() - 1);

        label end = tokens.size() - 1;
        label offset = varTokens.size() - 1;

        for(label j=end; j>i; j--)
        {
            tokens[j] = tokens[j-offset];
        }
    }

    forAll(varTokens, j)
    {
        tokens[i + j] = varTokens[j];
    }
}


// ************************************************************************* //
