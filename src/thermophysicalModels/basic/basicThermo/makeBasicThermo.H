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

InClass
    Foam::basicThermo

Description

\*---------------------------------------------------------------------------*/

#ifndef makeBasicThermo_H
#define makeBasicThermo_H

#include "basicThermo.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#define makeBasicThermo(Cthermo,Mixture,Transport,Thermo,EqnOfState)          \
                                                                              \
typedef Mixture<Transport<specieThermo<Thermo<EqnOfState> > > >               \
    Mixture##Transport##Thermo##EqnOfState;                                   \
                                                                              \
defineTemplateTypeNameAndDebugWithName                                        \
    (Mixture##Transport##Thermo##EqnOfState,                                  \
    #Mixture"<"#Transport"<specieThermo<"#Thermo"<"#EqnOfState">>>>", 0)      \
                                                                              \
typedef Cthermo<Mixture<Transport<specieThermo<Thermo<EqnOfState> > > > >     \
    Cthermo##Mixture##Transport##Thermo##EqnOfState;                          \
                                                                              \
defineTemplateTypeNameAndDebugWithName                                        \
    (Cthermo##Mixture##Transport##Thermo##EqnOfState,                         \
    #Cthermo                                                                  \
    "<"#Mixture"<"#Transport"<specieThermo<"#Thermo"<"#EqnOfState">>>>>", 0)  \
                                                                              \
addToRunTimeSelectionTable                                                    \
(                                                                             \
    basicThermo,                                                              \
    Cthermo##Mixture##Transport##Thermo##EqnOfState,                          \
    fvMesh                                                                    \
)


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
