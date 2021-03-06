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

#include "FDICPreconditioner.H"
#include "addToRunTimeSelectionTable.H"

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

namespace Foam
{
    defineTypeNameAndDebug(FDICPreconditioner, 0);

    addToRunTimeSelectionTable
    (
        lduPreconditioner,
        FDICPreconditioner,
        dictionary
    );
}


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

Foam::FDICPreconditioner::FDICPreconditioner
(
    const lduMatrix& matrix,
    const FieldField<Field, scalar>& coupleBouCoeffs,
    const FieldField<Field, scalar>& coupleIntCoeffs,
    const lduInterfaceFieldPtrsList& interfaces,
    const dictionary& 
)
:
    lduPreconditioner
    (
        matrix,
        coupleBouCoeffs,
        coupleIntCoeffs,
        interfaces
    ),
    rD_(matrix.diag()),
    rDuUpper_(matrix.upper().size()),
    rDlUpper_(matrix.upper().size())
{
    scalar* __restrict__ rDPtr = rD_.begin();
    scalar* __restrict__ rDuUpperPtr = rDuUpper_.begin();
    scalar* __restrict__ rDlUpperPtr = rDlUpper_.begin();

    const label* const __restrict__ uPtr =
        matrix_.lduAddr().upperAddr().begin();
    const label* const __restrict__ lPtr =
        matrix_.lduAddr().lowerAddr().begin();
    const scalar* const __restrict__ upperPtr = matrix_.upper().begin();

    register label nCells = rD_.size();
    register label nFaces = matrix_.upper().size();

    for (register label face=0; face<nFaces; face++)
    {
        #ifdef ICC_IA64_PREFETCH
        __builtin_prefetch (&uPtr[face+96],0,0);
        __builtin_prefetch (&lPtr[face+96],0,0);
        __builtin_prefetch (&upperPtr[face+96],0,1);
        __builtin_prefetch (&rDPtr[lPtr[face+24]],0,1);
        __builtin_prefetch (&rDPtr[uPtr[face+24]],1,1);
        #endif

        rDPtr[uPtr[face]] -= sqr(upperPtr[face])/rDPtr[lPtr[face]];
    }

    #ifdef ICC_IA64_PREFETCH
    #pragma ivdep
    #endif

    // Generate reciprocal FDIC
    for (register label cell=0; cell<nCells; cell++)
    {
        #ifdef ICC_IA64_PREFETCH
        __builtin_prefetch (&rDPtr[cell+96],0,1);
        #endif

        rDPtr[cell] = 1.0/rDPtr[cell];
    }

    #ifdef ICC_IA64_PREFETCH
    #pragma ivdep
    #endif

    for (register label face=0; face<nFaces; face++)
    {
        #ifdef ICC_IA64_PREFETCH
        __builtin_prefetch (&uPtr[face+96],0,0);
        __builtin_prefetch (&lPtr[face+96],0,0);
        __builtin_prefetch (&upperPtr[face+96],0,0);
        __builtin_prefetch (&rDuUpperPtr[face+96],0,0);
        __builtin_prefetch (&rDlUpperPtr[face+96],0,0);
        __builtin_prefetch (&rDPtr[uPtr[face+32]],0,1);
        __builtin_prefetch (&rDPtr[lPtr[face+32]],0,1);
        #endif

        rDuUpperPtr[face] = rDPtr[uPtr[face]]*upperPtr[face];
        rDlUpperPtr[face] = rDPtr[lPtr[face]]*upperPtr[face];
    }
}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

void Foam::FDICPreconditioner::precondition
(
    scalarField& wA,
    const scalarField& rA,
    const direction
) const
{
    scalar* __restrict__ wAPtr = wA.begin();
    const scalar* __restrict__ rAPtr = rA.begin();
    const scalar* __restrict__ rDPtr = rD_.begin();

    const label* const __restrict__ uPtr =
        matrix_.lduAddr().upperAddr().begin();
    const label* const __restrict__ lPtr =
        matrix_.lduAddr().lowerAddr().begin();

    const scalar* const __restrict__ rDuUpperPtr = rDuUpper_.begin();
    const scalar* const __restrict__ rDlUpperPtr = rDlUpper_.begin();

    register label nCells = wA.size();
    register label nFaces = matrix_.upper().size();
    register label nFacesM1 = nFaces - 1;

    #ifdef ICC_IA64_PREFETCH
    #pragma ivdep
    #endif

    for (register label cell=0; cell<nCells; cell++)
    {
        #ifdef ICC_IA64_PREFETCH
        __builtin_prefetch (&wAPtr[cell+96],0,1);
        __builtin_prefetch (&rDPtr[cell+96],0,1);
        __builtin_prefetch (&rAPtr[cell+96],0,1);
        #endif

        wAPtr[cell] = rDPtr[cell]*rAPtr[cell];
    }

    #ifdef ICC_IA64_PREFETCH
    #pragma noprefetch uPtr,lPtr,rDuUpperPtr,wAPtr
    #pragma nounroll
    #endif

    for (register label face=0; face<nFaces; face++)
    {
        #ifdef ICC_IA64_PREFETCH
        __builtin_prefetch (&uPtr[face+96],0,0);
        __builtin_prefetch (&lPtr[face+96],0,0);
        __builtin_prefetch (&rDuUpperPtr[face+96],0,0);
        __builtin_prefetch (&wAPtr[uPtr[face+32]],0,1); 
        __builtin_prefetch (&wAPtr[lPtr[face+32]],0,1);
        #endif

        wAPtr[uPtr[face]] -= rDuUpperPtr[face]*wAPtr[lPtr[face]];
    }

    #ifdef ICC_IA64_PREFETCH
    #pragma noprefetch uPtr,lPtr,rDlUpperPtr,wAPtr
    #pragma nounroll
    #endif

    for (register label face=nFacesM1; face>=0; face--)
    {
        #ifdef ICC_IA64_PREFETCH
        __builtin_prefetch (&uPtr[face-95],0,0);
        __builtin_prefetch (&lPtr[face-95],0,0);
        __builtin_prefetch (&rDlUpperPtr[face-95],0,0);
        __builtin_prefetch (&wAPtr[lPtr[face-16]],0,1);
        __builtin_prefetch (&wAPtr[uPtr[face-16]],0,1);
        __builtin_prefetch (&wAPtr[lPtr[face-24]],0,1);
        __builtin_prefetch (&wAPtr[uPtr[face-24]],0,1);
        __builtin_prefetch (&wAPtr[lPtr[face-32]],0,1);
        __builtin_prefetch (&wAPtr[uPtr[face-32]],0,1);
        #endif

        wAPtr[lPtr[face]] -= rDlUpperPtr[face]*wAPtr[uPtr[face]];
    }
}


// ************************************************************************* //
