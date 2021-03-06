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
    Foam::radiation::binaryAbsorptionEmission

Description
    Radiation coefficient based on two absorption models

SourceFiles
    binaryAbsorptionEmission.C

\*---------------------------------------------------------------------------*/

#ifndef radiationBinaryAbsorptionEmission_H
#define radiationBinaryAbsorptionEmission_H

#include "absorptionEmissionModel.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{
namespace radiation
{

/*---------------------------------------------------------------------------*\
                  Class binaryAbsorptionEmission Declaration
\*---------------------------------------------------------------------------*/

class binaryAbsorptionEmission
:
    public absorptionEmissionModel
{

    // Private data

        //- Coefficients dictionary
        dictionary coeffsDict_;

        //- First absorption model
        autoPtr<absorptionEmissionModel> model1_;

        //- Second absorption model
        autoPtr<absorptionEmissionModel> model2_;


public:

    //- Runtime type information
    TypeName("binaryAbsorptionEmission");


    // Constructors

        //- Construct from components
        binaryAbsorptionEmission
        (
            const dictionary& dict,
            const fvMesh& mesh
        );


    // Destructor

        ~binaryAbsorptionEmission();


    // Member Operators

        // Access

            // Absorption coefficient

                //- Absorption coefficient for continuous phase
                tmp<volScalarField> aCont() const;

                //- Absorption coefficient for dispersed phase
                tmp<volScalarField> aDisp() const;


            // Emission coefficient

                //- Emission coefficient for continuous phase
                tmp<volScalarField> eCont() const;

                //- Emission coefficient for dispersed phase
                tmp<volScalarField> eDisp() const;


            // Emission contribution

                //- Emission contribution for continuous phase
                tmp<volScalarField> ECont() const;

                //- Emission contribution for continuous phase
                tmp<volScalarField> EDisp() const;
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace radiation
} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
