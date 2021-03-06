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

#include "bound.H"
#include "volFields.H"
#include "fvc.H"

// * * * * * * * * * * * * * * * Global Functions  * * * * * * * * * * * * * //

void Foam::bound(volScalarField& vsf, const dimensionedScalar& vsf0)
{
    scalar minVsf = min(vsf).value();

    if (minVsf < vsf0.value())
    {
        Info<< "bounding " << vsf.name()
            << ", min: " << gMin(vsf.internalField())
            << " max: " << gMax(vsf.internalField())
            << " average: " << gAverage(vsf.internalField())
            << endl;

        vsf.internalField() = max
        (
            max
            (
                vsf.internalField(),
                fvc::average(max(vsf, vsf0))().internalField()
                // Bug fix: was assuming bound on zero.  HJ, 25/Nov/2008
                *pos(vsf0.value() - vsf.internalField())
            ),
            vsf0.value()
        );

        vsf.correctBoundaryConditions();
        vsf.boundaryField() = max(vsf.boundaryField(), vsf0.value());
    }
}


void Foam::boundMinMax
(
    volScalarField& vsf,
    const dimensionedScalar& vsf0,
    const dimensionedScalar& vsf1
)
{
    scalar minVsf = min(vsf).value();
    scalar maxVsf = max(vsf).value();

    if (minVsf < vsf0.value() || maxVsf > vsf1.value())
    {
        Info<< "bounding " << vsf.name()
            << ", min: " << gMin(vsf.internalField())
            << " max: " << gMax(vsf.internalField())
            << " average: " << gAverage(vsf.internalField())
            << endl;
    }

    if (minVsf < vsf0.value())
    {
        vsf.internalField() = max
        (
            max
            (
                vsf.internalField(),
                fvc::average(max(vsf, vsf0))().internalField()
                *pos(vsf0.value() - vsf.internalField())
            ),
            vsf0.value()
        );

        vsf.correctBoundaryConditions();
        vsf.boundaryField() = max(vsf.boundaryField(), vsf0.value());
    }

    if (maxVsf > vsf1.value())
    {
        vsf.internalField() = min
        (
            min
            (
                vsf.internalField(),
                fvc::average(min(vsf, vsf1))().internalField()
                *neg(vsf1.value() - vsf.internalField())
                // This is needed when all values are above max
                // HJ, 18/Apr/2009
              + pos(vsf1.value() - vsf.internalField())*vsf1.value()
            ),
            vsf1.value()
        );

        vsf.correctBoundaryConditions();
        vsf.boundaryField() = min(vsf.boundaryField(), vsf1.value());
    }
}


// ************************************************************************* //
