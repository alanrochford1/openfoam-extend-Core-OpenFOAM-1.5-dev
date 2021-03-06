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
    Reads the data portion of a model catalogue File.

\*---------------------------------------------------------------------------*/

#include "cellModeller.H"
#include "dictionary.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

cellModeller::cellModeller()
{
    if (modelPtrs_.size())
    {
        FatalErrorIn("cellModeller::cellModeller(const fileName&)")
            << "attempt to re-construct cellModeller when it already exists"
            << exit(FatalError);
    }

    label maxIndex = 0;
    forAll(models_, i)
    {
        if (models_[i].index() > maxIndex) maxIndex = models_[i].index();
    }

    modelPtrs_.setSize(maxIndex + 1);
    modelPtrs_ = NULL;

    // For all the words in the wordlist, set the details of the model
    // to those specified by the word name and the other parameters
    // given. This should result in an automatic 'read' of the model
    // from its File (see cellModel class).
    forAll(models_, i)
    {
        if (modelPtrs_[models_[i].index()])
        {
            FatalErrorIn("cellModeller::cellModeller(const fileName&)")
                << "more than one model share the index "
                << models_[i].index()
                << exit(FatalError);
        }

        modelPtrs_[models_[i].index()] = &models_[i];

        if (modelDictionary_.found(models_[i].name()))
        {
            FatalErrorIn("cellModeller::cellModeller(const fileName&)")
                << "more than one model share the name "
                << models_[i].name()
                << exit(FatalError);
        }

        modelDictionary_.insert(models_[i].name(), &models_[i]);
    }
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// ************************************************************************* //
