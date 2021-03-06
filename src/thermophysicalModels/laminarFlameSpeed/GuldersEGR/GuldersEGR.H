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
    Foam::laminarFlameSpeedModels::GuldersEGR

Description
    Laminar flame speed obtained from Gulder's correlation with EGR modelling.

SourceFiles
    GuldersEGR.C

\*---------------------------------------------------------------------------*/

#ifndef GuldersEGR_H
#define GuldersEGR_H

#include "laminarFlameSpeed.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{
namespace laminarFlameSpeedModels
{

/*---------------------------------------------------------------------------*\
                           Class GuldersEGR Declaration
\*---------------------------------------------------------------------------*/

class GuldersEGR
:
    public laminarFlameSpeed
{
    // Private Data

        dictionary coeffsDict_;

        scalar W_;
        scalar eta_;
        scalar xi_;
        scalar f_;
        scalar alpha_;
        scalar beta_;


    // Private member functions

        inline scalar SuRef
        (
            scalar phi
        ) const;

        inline scalar Su0pTphi
        (
            scalar p,
            scalar Tu,
            scalar phi,
            scalar Yres
        ) const;

        tmp<volScalarField> Su0pTphi
        (
            const volScalarField& p,
            const volScalarField& Tu,
            scalar phi
        ) const;

        tmp<volScalarField> Su0pTphi
        (
            const volScalarField& p,
            const volScalarField& Tu,
            const volScalarField& phi,
            const volScalarField& egr
        ) const;

        //- Construct as copy (not implemented)
        GuldersEGR(const GuldersEGR&);

        void operator=(const GuldersEGR&);


public:

    //- Runtime type information
    TypeName("GuldersEGR");

    // Constructors

        //- Construct from dictionary and hhuCombustionThermo
        GuldersEGR
        (
            const dictionary&,
            const hhuCombustionThermo&
        );


    // Destructor

        ~GuldersEGR();


    // Member functions

        //- Return the laminar flame speed [m/s]
        tmp<volScalarField> operator()() const;
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End laminarFlameSpeedModels
} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
