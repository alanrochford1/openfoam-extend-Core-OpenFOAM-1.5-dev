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
    Foam::faceZone

Description
    A subset of mesh faces organised as a primitive patch.

    For quick check whether a face belongs to the zone use the lookup
    mechanism in faceZoneMesh, where all the zoned faces are registered
    with their zone number.

SourceFiles
    faceZone.C
    newFaceZone.C

\*---------------------------------------------------------------------------*/

#ifndef faceZone_H
#define faceZone_H

#include "typeInfo.H"
#include "dictionary.H"
#include "labelList.H"
#include "faceZoneMeshFwd.H"
#include "boolList.H"
#include "primitiveFacePatch.H"
#include "Map.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

class mapPolyMesh;

// Forward declaration of friend functions and operators

class faceZone;
Ostream& operator<<(Ostream&, const faceZone&);


/*---------------------------------------------------------------------------*\
                           Class faceZone Declaration
\*---------------------------------------------------------------------------*/

class faceZone
:
    public labelList
{
    // Private data

        //- Name of zone
        word name_;

        //- Flip map for all faces in the zone.  Set to true if the
        //  face needs to be flipped to achieve the correct orientation.
        boolList flipMap_;

        //- Index of zone
        label index_;

        //- Reference to zone list
        const faceZoneMesh& zoneMesh_;


        // Demand-driven private data

            //- Primitive patch made out of correctly flipped faces
            mutable primitiveFacePatch* patchPtr_;

            //- Master cell layer
            mutable labelList* masterCellsPtr_;

            //- Slave cell layer
            mutable labelList* slaveCellsPtr_;

            //- Global edge addressing
            mutable labelList* mePtr_;

            //- Map of face labels in zone for fast location lookup
            mutable Map<label>* faceLookupMapPtr_;


    // Private Member Functions

        //- Disallow default bitwise copy construct
        faceZone(const faceZone&);

        //- Disallow default bitwise assignment
        void operator=(const faceZone&);

        //- Build primitive patch
        void calcFaceZonePatch() const;

        //- Return map of local face indices
        const Map<label>& faceLookupMap() const;

        //- Build map of local face indices
        void calcFaceLookupMap() const;

        //- Calculate master and slave face layer
        void calcCellLayers() const;

        //- Check addressing
        void checkAddressing() const;

        //- Clear addressing
        void clearAddressing();


public:

    //- Runtime type information
    TypeName("faceZone");


    // Declare run-time constructor selection tables

        declareRunTimeSelectionTable
        (
            autoPtr,
            faceZone,
            dictionary,
            (
                const word& name,
                const dictionary& dict,
                const label index,
                const faceZoneMesh& zm
            ),
            (name, dict, index, zm)
        );


    // Constructors

        //- Construct from components
        faceZone
        (
            const word& name,
            const labelList& addr,
            const boolList& fm,
            const label index,
            const faceZoneMesh& zm
        );

        //- Construct from dictionary
        faceZone
        (
            const word& name,
            const dictionary& dict,
            const label index,
            const faceZoneMesh& zm
        );

        //- Construct given the original zone and resetting the
        //  face list and zone mesh information
        faceZone
        (
            const faceZone& fz,
            const labelList& addr,
            const boolList& fm,
            const label index,
            const faceZoneMesh& zm
        );

        //- Construct and return a clone, resetting the zone mesh
        virtual autoPtr<faceZone> clone(const faceZoneMesh& zm) const
        {
            return autoPtr<faceZone>
            (
                new faceZone(*this, *this, flipMap(), index(), zm)
            );
        }

        //- Construct and return a clone, resetting the face list
        //  and zone mesh
        virtual autoPtr<faceZone> clone
        (
            const labelList& addr,
            const boolList& fm,
            const label index,
            const faceZoneMesh& zm
        ) const
        {
            return autoPtr<faceZone>
            (
                new faceZone(*this, addr, fm, index, zm)
            );
        }


    // Selectors

        //- Return a pointer to a new face zone
        //  created on freestore from dictionary
        static autoPtr<faceZone> New
        (
            const word& name,
            const dictionary& dict,
            const label index,
            const faceZoneMesh& zm
        );


    //- Destructor

        virtual ~faceZone();


    // Member Functions

        //- Return name
        const word& name() const
        {
            return name_;
        }

        //- Return face flip map
        const boolList& flipMap() const
        {
            return flipMap_;
        }

        //- Map storing the local face index for every global face index.
        //  Used to find out the index of face in the zone from the known global
        //  face index.  If the face is not in the zone, returns -1
        label whichFace(const label globalFaceID) const;

        //- Return reference to primitive patch
        const primitiveFacePatch& operator()() const;

        //- Return the index of this zone in zone list
        label index() const
        {
            return index_;
        }

        //- Return zoneMesh reference
        const faceZoneMesh& zoneMesh() const;


        // Addressing into mesh

            //- Return labels of master cells (cells next to the master face
            //  zone in the prescribed direction)
            const labelList& masterCells() const;

            //- Return labels of slave cells
            const labelList& slaveCells() const;

            //- Return global edge index for local edges
            const labelList& meshEdges() const;


        //- Reset addressing and flip map (clearing demand-driven data)
        void resetAddressing(const labelList&, const boolList&);

        //- Check zone definition. Return true if in error.
        bool checkDefinition(const bool report = false) const;

        //- Correct patch after moving points
        virtual void movePoints(const pointField&);

        //- Update for changes in topology  Note: Zone has been rebuilt
        //  and no mapping is required.  HJ, 1/Mar/2009
        void updateMesh();

        //- Write
        virtual void write(Ostream&) const;

        //- Write dictionary
        virtual void writeDict(Ostream&) const;


    // Ostream Operator

        friend Ostream& operator<<(Ostream&, const faceZone&);
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
