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

Description
    Residual error estimate for the fv source operators.

\*---------------------------------------------------------------------------*/

#include "resErrorSup.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace resError
{

template<class Type>
tmp<errorEstimate<Type> >
Sp
(
    const volScalarField& sp,
    GeometricField<Type, fvPatchField, volMesh>& vf
)
{
    return tmp<errorEstimate<Type> >
    (
        new errorEstimate<Type>
        (
            vf,
            sp.dimensions()*vf.dimensions()*dimVolume,
            sp.internalField()*vf.internalField(),
            scalarField(vf.internalField().size(), 0)
        )
    );
}

template<class Type>
tmp<errorEstimate<Type> >
Sp
(
    const tmp<volScalarField>& tsp,
    GeometricField<Type, fvPatchField, volMesh>& vf
)
{
    tmp<errorEstimate<Type> > tee = resError::Sp(tsp(), vf);
    tsp.clear();
    return tee;
}


template<class Type>
tmp<errorEstimate<Type> >
Sp
(
    const dimensionedScalar& sp,
    GeometricField<Type, fvPatchField, volMesh>& vf
)
{
    return tmp<errorEstimate<Type> >
    (
        new errorEstimate<Type>
        (
            vf,
            sp.dimensions()*vf.dimensions()*dimVolume,
            sp.value()*vf.internalField(),
            scalarField(vf.internalField().size(), 0)
        )
    );
}


template<class Type>
tmp<errorEstimate<Type> >
SuSp
(
    const volScalarField& sp,
    GeometricField<Type, fvPatchField, volMesh>& vf
)
{
    return Sp(sp, vf);
}

template<class Type>
tmp<errorEstimate<Type> >
SuSp
(
    const tmp<volScalarField>& tsp,
    GeometricField<Type, fvPatchField, volMesh>& vf
)
{
    tmp<errorEstimate<Type> > tee = resError::SuSp(tsp(), vf);
    tsp.clear();
    return tee;
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace resError

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// ************************************************************************* //


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

