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

#include "PCG.H"

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

namespace Foam
{
    defineTypeNameAndDebug(PCG, 0);

    lduSolver::addsymMatrixConstructorToTable<PCG>
        addPCGSymMatrixConstructorToTable_;
}


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

Foam::PCG::PCG
(
    const word& fieldName,
    const lduMatrix& matrix,
    const FieldField<Field, scalar>& coupleBouCoeffs,
    const FieldField<Field, scalar>& coupleIntCoeffs,
    const lduInterfaceFieldPtrsList& interfaces,
    Istream& solverData
)
:
    lduSolver
    (
        fieldName,
        matrix,
        coupleBouCoeffs,
        coupleIntCoeffs,
        interfaces,
        solverData
    )
{}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

Foam::lduSolverPerformance Foam::PCG::solve
(
    scalarField& x,
    const scalarField& b,
    const direction cmpt
) const
{
    // Setup class containing solver performance data
    lduSolverPerformance solverPerf(typeName, fieldName());

    register label nCells = x.size();

    scalar* __restrict__ xPtr = x.begin();

    scalarField pA(nCells);
    scalar* __restrict__ pAPtr = pA.begin();

    scalarField wA(nCells);
    scalar* __restrict__ wAPtr = wA.begin();

    // Calculate A.x
    matrix_.Amul(wA, x, coupleBouCoeffs_, interfaces_, cmpt);

    // Calculate initial residual field
    scalarField rA(b - wA);
    scalar* __restrict__ rAPtr = rA.begin();

    // Calculate normalisation factor
    scalar normFactor = this->normFactor(x, b, wA, pA, cmpt);

    if (lduMatrix::debug >= 2)
    {
        Info<< "   Normalisation factor = " << normFactor << endl;
    }

    // Calculate normalised residual norm
    solverPerf.initialResidual() = gSumMag(rA)/normFactor;
    solverPerf.finalResidual() = solverPerf.initialResidual();

    // Check convergence, solve if not converged
    if (!stop(solverPerf))
    {
        scalar wArA = matrix_.great_;
        scalar wArAold = wArA;

        // Select and construct the preconditioner
        autoPtr<lduPreconditioner> preconPtr;

        // For backward compatibility, check type of preconditioner entry
        // HJ, 2/Oct/2007

        if (dict().isDict("preconditioner"))
        {
            // New format: preconditioner is dictionary
            preconPtr =
                lduPreconditioner::New
                (
                    matrix_,
                    coupleBouCoeffs_,
                    coupleIntCoeffs_,
                    interfaces_,
                    dict().subDict("preconditioner")
                );
        }
        else
        {
            // Old format: manufacture a dictionary
            dictionary preconDict;
            preconDict.add("type", word(dict().lookup("preconditioner")));

            preconPtr =
                lduPreconditioner::New
                (
                    matrix_,
                    coupleBouCoeffs_,
                    coupleIntCoeffs_,
                    interfaces_,
                    preconDict
                );
        }

        // Solver iteration
        do
        {
            // Store previous wArA
            wArAold = wArA;

            // Precondition residual
            preconPtr->precondition(wA, rA, cmpt);

            // Update search directions:
            wArA = gSumProd(wA, rA);

            if (solverPerf.nIterations() == 0)
            {
                #ifdef ICC_IA64_PREFETCH
                #pragma ivdep
                #endif

                for (register label cell=0; cell<nCells; cell++)
                {
                    #ifdef ICC_IA64_PREFETCH
                    __builtin_prefetch (&pAPtr[cell+96],0,1);
                    __builtin_prefetch (&wAPtr[cell+96],0,1);
                    #endif

                    pAPtr[cell] = wAPtr[cell];
                }
            }
            else
            {
                scalar beta = wArA/wArAold;

                #ifdef ICC_IA64_PREFETCH
                #pragma ivdep
                #endif

                for (register label cell=0; cell<nCells; cell++)
                {
                    #ifdef ICC_IA64_PREFETCH
                    __builtin_prefetch (&pAPtr[cell+96],0,1);
                    __builtin_prefetch (&wAPtr[cell+96],0,1);
                    #endif

                    pAPtr[cell] = wAPtr[cell] + beta*pAPtr[cell];
                }
            }


            // Update preconditioned residual
            matrix_.Amul(wA, pA, coupleBouCoeffs_, interfaces_, cmpt);

            scalar wApA = gSumProd(wA, pA);


            // Test for singularity
            if (solverPerf.checkSingularity(mag(wApA)/normFactor)) break;


            // Update solution and residual:

            scalar alpha = wArA/wApA;

            #ifdef ICC_IA64_PREFETCH
            #pragma ivdep
            #endif

            for (register label cell=0; cell<nCells; cell++)
            {
                #ifdef ICC_IA64_PREFETCH
                __builtin_prefetch (&pAPtr[cell+96],0,1);
                __builtin_prefetch (&wAPtr[cell+96],0,1);
                __builtin_prefetch (&xPtr[cell+96],0,1);
                __builtin_prefetch (&rAPtr[cell+96],0,1);
                #endif

                xPtr[cell] += alpha*pAPtr[cell];
                rAPtr[cell] -= alpha*wAPtr[cell];
            }

            solverPerf.finalResidual() = gSumMag(rA)/normFactor;
            solverPerf.nIterations()++;
        } while (!stop(solverPerf));
    }

    return solverPerf;
}


// ************************************************************************* //
