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

#include "syringePressureFvPatchScalarField.H"
#include "volMesh.H"
#include "addToRunTimeSelectionTable.H"
#include "fvPatchFieldMapper.H"
#include "surfaceFields.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

syringePressureFvPatchScalarField::syringePressureFvPatchScalarField
(
    const fvPatch& p,
    const DimensionedField<scalar, volMesh>& iF
)
:
    fixedValueFvPatchScalarField(p, iF),
    curTimeIndex_(-1)
{}


syringePressureFvPatchScalarField::syringePressureFvPatchScalarField
(
    const syringePressureFvPatchScalarField& sppsf,
    const fvPatch& p,
    const DimensionedField<scalar, volMesh>& iF,
    const fvPatchFieldMapper& mapper
)
:
    fixedValueFvPatchScalarField(sppsf, p, iF, mapper),
    Ap_(sppsf.Ap_),
    Sp_(sppsf.Sp_),
    VsI_(sppsf.VsI_),
    tas_(sppsf.tas_),
    tae_(sppsf.tae_),
    tds_(sppsf.tds_),
    tde_(sppsf.tde_),
    psI_(sppsf.psI_),
    psi_(sppsf.psi_),
    ams_(sppsf.ams_),
    ams0_(sppsf.ams0_),
    curTimeIndex_(-1)
{}


syringePressureFvPatchScalarField::syringePressureFvPatchScalarField
(
    const fvPatch& p,
    const DimensionedField<scalar, volMesh>& iF,
    const dictionary& dict
)
:
    fixedValueFvPatchScalarField(p, iF),
    Ap_(readScalar(dict.lookup("Ap"))),
    Sp_(readScalar(dict.lookup("Sp"))),
    VsI_(readScalar(dict.lookup("VsI"))),
    tas_(readScalar(dict.lookup("tas"))),
    tae_(readScalar(dict.lookup("tae"))),
    tds_(readScalar(dict.lookup("tds"))),
    tde_(readScalar(dict.lookup("tde"))),
    psI_(readScalar(dict.lookup("psI"))),
    psi_(readScalar(dict.lookup("psi"))),
    ams_(readScalar(dict.lookup("ams"))),
    ams0_(ams_),
    curTimeIndex_(-1)
{
    scalar ps = (psI_*VsI_ + ams_/psi_)/Vs(db().time().value());
    fvPatchField<scalar>::operator=(ps);
}


syringePressureFvPatchScalarField::syringePressureFvPatchScalarField
(
    const syringePressureFvPatchScalarField& sppsf,
    const DimensionedField<scalar, volMesh>& iF
)
:
    fixedValueFvPatchScalarField(sppsf, iF),
    Ap_(sppsf.Ap_),
    Sp_(sppsf.Sp_),
    VsI_(sppsf.VsI_),
    tas_(sppsf.tas_),
    tae_(sppsf.tae_),
    tds_(sppsf.tds_),
    tde_(sppsf.tde_),
    psI_(sppsf.psI_),
    psi_(sppsf.psi_),
    ams_(sppsf.ams_),
    ams0_(sppsf.ams0_),
    curTimeIndex_(-1)
{}


syringePressureFvPatchScalarField::syringePressureFvPatchScalarField
(
    const syringePressureFvPatchScalarField& sppsf
)
:
    fixedValueFvPatchScalarField(sppsf),
    Ap_(sppsf.Ap_),
    Sp_(sppsf.Sp_),
    VsI_(sppsf.VsI_),
    tas_(sppsf.tas_),
    tae_(sppsf.tae_),
    tds_(sppsf.tds_),
    tde_(sppsf.tde_),
    psI_(sppsf.psI_),
    psi_(sppsf.psi_),
    ams_(sppsf.ams_),
    ams0_(sppsf.ams0_),
    curTimeIndex_(-1)
{}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

scalar syringePressureFvPatchScalarField::Vs(const scalar t) const
{
    if (t < tas_)
    {
        return VsI_;
    }
    else if (t < tae_)
    {
        return 
            VsI_
          + 0.5*Ap_*Sp_*sqr(t - tas_)/(tae_ - tas_);
    }
    else if (t < tds_)
    {
        return 
            VsI_ 
          + 0.5*Ap_*Sp_*(tae_ - tas_)
          + Ap_*Sp_*(t - tae_);
    }
    else if (t < tde_)
    {
        return 
            VsI_ 
          + 0.5*Ap_*Sp_*(tae_ - tas_) 
          + Ap_*Sp_*(tds_ - tae_)
          + Ap_*Sp_*(t - tds_)
          - 0.5*Ap_*Sp_*sqr(t - tds_)/(tde_ - tds_);
    }
    else
    {
        return 
            VsI_ 
          + 0.5*Ap_*Sp_*(tae_ - tas_) 
          + Ap_*Sp_*(tds_ - tae_)
          + 0.5*Ap_*Sp_*(tde_ - tds_);
    }
}


void syringePressureFvPatchScalarField::updateCoeffs()
{
    if (updated())
    {
        return;
    }

    if (curTimeIndex_ != db().time().timeIndex())
    {
        ams0_ = ams_;
        curTimeIndex_ = db().time().timeIndex();
    }

    scalar t = db().time().value();
    scalar deltaT = db().time().deltaT().value();

    const surfaceScalarField& phi = db().lookupObject<surfaceScalarField>
    (
        "phi"
    );

    const fvsPatchField<scalar>& phip =
        patch().patchField<surfaceScalarField, scalar>(phi);

    if (phi.dimensions() == dimVelocity*dimArea)
    {
        ams_ = ams0_ + deltaT*sum((*this*psi_)*phip);
    }
    else if (phi.dimensions() == dimDensity*dimVelocity*dimArea)
    {
        ams_ = ams0_ + deltaT*sum(phip);
    }
    else
    {
        FatalErrorIn("syringePressureFvPatchScalarField::updateCoeffs()")
            << "dimensions of phi are not correct"
            << "\n    on patch " << this->patch().name()
            << " of field " << this->dimensionedInternalField().name()
            << " in file " << this->dimensionedInternalField().objectPath()
            << exit(FatalError);
    }

    scalar ps = (psI_*VsI_ + ams_/psi_)/Vs(t);

    operator==(ps);

    fixedValueFvPatchScalarField::updateCoeffs();
}


void syringePressureFvPatchScalarField::write(Ostream& os) const
{
    fvPatchScalarField::write(os);

    os.writeKeyword("Ap") << Ap_ << token::END_STATEMENT << nl;
    os.writeKeyword("Sp") << Sp_ << token::END_STATEMENT << nl;
    os.writeKeyword("VsI") << VsI_ << token::END_STATEMENT << nl;
    os.writeKeyword("tas") << tas_ << token::END_STATEMENT << nl;
    os.writeKeyword("tae") << tae_ << token::END_STATEMENT << nl;
    os.writeKeyword("tds") << tds_ << token::END_STATEMENT << nl;
    os.writeKeyword("tde") << tde_ << token::END_STATEMENT << nl;
    os.writeKeyword("psI") << psI_ << token::END_STATEMENT << nl;
    os.writeKeyword("psi") << psi_ << token::END_STATEMENT << nl;
    os.writeKeyword("ams") << ams_ << token::END_STATEMENT << nl;

    writeEntry("value", os);
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

makePatchTypeField
(
    fvPatchScalarField,
    syringePressureFvPatchScalarField
);

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// ************************************************************************* //
