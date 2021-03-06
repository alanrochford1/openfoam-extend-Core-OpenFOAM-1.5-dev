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

#include "fvScalarMatrix.H"
#include "zeroGradientFvPatchFields.H"

// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

template<>
void Foam::fvMatrix<Foam::scalar>::setComponentReference
(
    const label patchi,
    const label facei,
    const direction,
    const scalar value
)
{
    if (psi_.needReference())
    {
        if (Pstream::master())
        {
            internalCoeffs_[patchi][facei] +=
                diag()[psi_.mesh().boundary()[patchi].faceCells()[facei]];

            boundaryCoeffs_[patchi][facei] += 
                diag()[psi_.mesh().boundary()[patchi].faceCells()[facei]]
               *value;
        }
    }
}


template<>
Foam::autoPtr<Foam::fvMatrix<Foam::scalar>::fvSolver>
Foam::fvMatrix<Foam::scalar>::solver
(
    Istream& solverControls
)
{
    if (debug)
    {
        Info<< "fvMatrix<scalar>::solver(Istream& solverControls) : "
               "solver for fvMatrix<scalar>"
            << endl;
    }

    scalarField saveDiag = diag();
    addBoundaryDiag(diag(), 0);

    // Make a copy of interfaces: no longer a reference
    // HJ, 20/Nov/2007
    lduInterfaceFieldPtrsList interfaces = psi_.boundaryField().interfaces();

    autoPtr<fvMatrix<scalar>::fvSolver> solverPtr
    (
        new fvMatrix<scalar>::fvSolver
        (
            *this,
            lduSolver::New
            (
                psi_.name(),
                *this,
                boundaryCoeffs_,
                internalCoeffs_,
                interfaces,
                solverControls
            )
        )
    );

    diag() = saveDiag;

    return solverPtr;
}


template<>
Foam::lduMatrix::solverPerformance Foam::fvMatrix<Foam::scalar>::fvSolver::solve
(
    Istream& solverControls
)
{
    scalarField saveDiag = fvMat_.diag();
    fvMat_.addBoundaryDiag(fvMat_.diag(), 0);

    scalarField totalSource = fvMat_.source();
    fvMat_.addBoundarySource(totalSource, false);

    solver_->read(solverControls);
    lduSolverPerformance solverPerf = 
        solver_->solve(fvMat_.psi().internalField(), totalSource);

    solverPerf.print();

    fvMat_.diag() = saveDiag;

    fvMat_.psi().correctBoundaryConditions();

    return solverPerf;
}


template<>
Foam::lduMatrix::solverPerformance Foam::fvMatrix<Foam::scalar>::solve
(
    Istream& solverControls
)
{
    if (debug)
    {
        Info<< "fvMatrix<scalar>::solve(Istream& solverControls) : "
               "solving fvMatrix<scalar>"
            << endl;
    }

    scalarField saveDiag = diag();
    addBoundaryDiag(diag(), 0);

    scalarField totalSource = source_;
    addBoundarySource(totalSource, false);

    // Make a copy of interfaces: no longer a reference
    // HJ, 20/Nov/2007
    lduInterfaceFieldPtrsList interfaces = psi_.boundaryField().interfaces();

    // Solver call
    lduSolverPerformance solverPerf = lduSolver::New
    (
        psi_.name(),
        *this,
        boundaryCoeffs_,
        internalCoeffs_,
        interfaces,
        solverControls
    )->solve(psi_.internalField(), totalSource);

    solverPerf.print();

    diag() = saveDiag;

    psi_.correctBoundaryConditions();

    return solverPerf;
}


template<>
Foam::tmp<Foam::scalarField> Foam::fvMatrix<Foam::scalar>::residual() const
{
    scalarField boundaryDiag(psi_.size(), 0.0);
    addBoundaryDiag(boundaryDiag, 0);

    // Make a copy of interfaces: no longer a reference
    // HJ, 20/Nov/2007
    lduInterfaceFieldPtrsList interfaces = psi_.boundaryField().interfaces();

    tmp<scalarField> tres
    (
        lduMatrix::residual
        (
            psi_.internalField(),
            source_ - boundaryDiag*psi_.internalField(),
            boundaryCoeffs_,
            interfaces,
            0
        )
    );

    addBoundarySource(tres());

    return tres;
}


template<>
Foam::tmp<Foam::volScalarField> Foam::fvMatrix<Foam::scalar>::H() const
{
    tmp<volScalarField> tHphi
    (
        new volScalarField
        (
            IOobject
            (
                "H("+psi_.name()+')',
                psi_.instance(),
                psi_.mesh(),
                IOobject::NO_READ,
                IOobject::NO_WRITE
            ),
            psi_.mesh(),
            dimensions_/dimVol,
            zeroGradientFvPatchScalarField::typeName
        )
    );
    volScalarField& Hphi = tHphi();

    Hphi.internalField() = (lduMatrix::H(psi_.internalField()) + source_);
    addBoundarySource(Hphi.internalField());

    Hphi.internalField() /= psi_.mesh().V();
    Hphi.correctBoundaryConditions();

    return tHphi;
}


template<>
Foam::tmp<Foam::volScalarField> Foam::fvMatrix<Foam::scalar>::H1() const
{
    tmp<volScalarField> tH1
    (
        new volScalarField
        (
            IOobject
            (
                "H(1)",
                psi_.instance(),
                psi_.mesh(),
                IOobject::NO_READ,
                IOobject::NO_WRITE
            ),
            psi_.mesh(),
            dimensions_/(dimVol*psi_.dimensions()),
            zeroGradientFvPatchScalarField::typeName
        )
    );
    volScalarField& H1_ = tH1();

    H1_.internalField() = lduMatrix::H1();
    //addBoundarySource(Hphi.internalField());

    H1_.internalField() /= psi_.mesh().V();
    H1_.correctBoundaryConditions();

    return tH1;
}


// ************************************************************************* //
