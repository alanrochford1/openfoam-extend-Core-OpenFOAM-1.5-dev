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

#include "RNGkEpsilon.H"
#include "addToRunTimeSelectionTable.H"
#include "wallFvPatch.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{
namespace incompressible
{
namespace RASModels
{

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

defineTypeNameAndDebug(RNGkEpsilon, 0);
addToRunTimeSelectionTable(RASModel, RNGkEpsilon, dictionary);

// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

RNGkEpsilon::RNGkEpsilon
(
    const volVectorField& U,
    const surfaceScalarField& phi,
    transportModel& lamTransportModel
)
:
    RASModel(typeName, U, phi, lamTransportModel),

    Cmu_
    (
        dimensioned<scalar>::lookupOrAddToDict
        (
            "Cmu",
            coeffDict_,
            0.0845
        )
    ),
    C1_
    (
        dimensioned<scalar>::lookupOrAddToDict
        (
            "C1",
            coeffDict_,
            1.42
        )
    ),
    C2_
    (
        dimensioned<scalar>::lookupOrAddToDict
        (
            "C2",
            coeffDict_,
            1.68
        )
    ),
    alphak_
    (
        dimensioned<scalar>::lookupOrAddToDict
        (
            "alphak",
            coeffDict_,
            1.39
        )
    ),
    alphaEps_
    (
        dimensioned<scalar>::lookupOrAddToDict
        (
            "alphaEps",
            coeffDict_,
            1.39
        )
    ),
    eta0_
    (
        dimensioned<scalar>::lookupOrAddToDict
        (
            "eta0",
            coeffDict_,
            4.38
        )
    ),
    beta_
    (
        dimensioned<scalar>::lookupOrAddToDict
        (
            "beta",
            coeffDict_,
            0.012
        )
    ),

    k_
    (
        IOobject
        (
            "k",
            runTime_.timeName(),
            mesh_,
            IOobject::MUST_READ,
            IOobject::AUTO_WRITE
        ),
        mesh_
    ),

    epsilon_
    (
        IOobject
        (
            "epsilon",
            runTime_.timeName(),
            mesh_,
            IOobject::MUST_READ,
            IOobject::AUTO_WRITE
        ),
        mesh_
    ),

    nut_(Cmu_*sqr(k_)/(epsilon_ + epsilonSmall_))
{
#   include "wallViscosityI.H"

    printCoeffs();
}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //


tmp<volSymmTensorField> RNGkEpsilon::R() const
{
    return tmp<volSymmTensorField>
    (
        new volSymmTensorField
        (
            IOobject
            (
                "R",
                runTime_.timeName(),
                mesh_,
                IOobject::NO_READ,
                IOobject::NO_WRITE
            ),
            ((2.0/3.0)*I)*k_ - nut_*twoSymm(fvc::grad(U_)),
            k_.boundaryField().types()
        )
    );
}


tmp<volSymmTensorField> RNGkEpsilon::devReff() const
{
    return tmp<volSymmTensorField>
    (
        new volSymmTensorField
        (
            IOobject
            (
                "devRhoReff",
                runTime_.timeName(),
                mesh_,
                IOobject::NO_READ,
                IOobject::NO_WRITE
            ),
           -nuEff()*dev(twoSymm(fvc::grad(U_)))
        )
    );
}


tmp<fvVectorMatrix> RNGkEpsilon::divDevReff(volVectorField& U) const
{
    return
    (
      - fvm::laplacian(nuEff(), U)
      - fvc::div(nuEff()*dev(fvc::grad(U)().T()))
    );
}


bool RNGkEpsilon::read()
{
    if (RASModel::read())
    {
        Cmu_.readIfPresent(coeffDict_);
        C1_.readIfPresent(coeffDict_);
        C2_.readIfPresent(coeffDict_);
        alphak_.readIfPresent(coeffDict_);
        alphaEps_.readIfPresent(coeffDict_);
        eta0_.readIfPresent(coeffDict_);
        beta_.readIfPresent(coeffDict_);

        return true;
    }
    else
    {
        return false;
    }
}


void RNGkEpsilon::correct()
{
    transportModel_.correct();

    if (!turbulence_)
    {
        return;
    }

    RASModel::correct();

    volScalarField S2 = 2*magSqr(symm(fvc::grad(U_)));

    volScalarField G = nut_*S2;

    volScalarField eta = sqrt(S2)*k_/epsilon_;
    volScalarField R =
        ((eta*(scalar(1) - eta/eta0_))/(scalar(1) + beta_*eta*sqr(eta)));

#   include "wallFunctionsI.H"

    // Dissipation equation
    tmp<fvScalarMatrix> epsEqn
    (
        fvm::ddt(epsilon_)
      + fvm::div(phi_, epsilon_)
      + fvm::SuSp(-fvc::div(phi_), epsilon_)
      - fvm::laplacian(DepsilonEff(), epsilon_)
     ==
        (C1_ - R)*G*epsilon_/k_
    //- fvm::SuSp(R*G/k_, epsilon_)
      - fvm::Sp(C2_*epsilon_/k_, epsilon_)
    );

    epsEqn().relax();

#   include "wallDissipationI.H"

    solve(epsEqn);
    bound(epsilon_, epsilon0_);


    // Turbulent kinetic energy equation

    tmp<fvScalarMatrix> kEqn
    (
        fvm::ddt(k_)
      + fvm::div(phi_, k_)
      + fvm::SuSp(-fvc::div(phi_), k_)
      - fvm::laplacian(DkEff(), k_)
     ==
        G - fvm::Sp(epsilon_/k_, k_)
    );

    kEqn().relax();
    solve(kEqn);
    bound(k_, k0_);


    // Re-calculate viscosity
    nut_ = Cmu_*sqr(k_)/epsilon_;

#   include "wallViscosityI.H"

}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace RASModels
} // End namespace incompressible
} // End namespace Foam

// ************************************************************************* //
