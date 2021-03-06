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
    Add pointZones/faceZones/cellZones to the mesh from similar named
    pointSets/faceSets/cellSets.

    There is one catch: for faceZones you also need to specify a flip
    condition which basically denotes the side of the face. In this app
    it reads a cellSet (xxxCells if 'xxx' is the name of the faceSet) and
    any face whose neighbour is in the cellSet gets a flip=true.
    There are lots of situations in which this will go wrong but it is the
    best I can think of for now.

    If one is not interested in sideNess specify the -noFlipMap
    command line option.

\*---------------------------------------------------------------------------*/

#include "argList.H"
#include "Time.H"
#include "polyMesh.H"
#include "IStringStream.H"
#include "cellSet.H"
#include "faceSet.H"
#include "pointSet.H"
#include "OFstream.H"
#include "IFstream.H"
#include "IOobjectList.H"

using namespace Foam;

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

// Main program:

int main(int argc, char *argv[])
{
    argList::validOptions.insert("noFlipMap", "");

#   include "addTimeOptions.H"
#   include "setRootCase.H"
#   include "createTime.H"


    bool noFlipMap = args.options().found("noFlipMap");

    // Get times list
    instantList Times = runTime.times();

    label startTime = Times.size()-1;
    label endTime = Times.size();

#   include "checkTimeOption.H"
#   include "checkLatestTimeOption.H"


    runTime.setTime(Times[startTime], startTime);

#   include "createPolyMesh.H"

    // Search for list of objects for the time of the mesh
    IOobjectList objects
    (
        mesh,
        mesh.pointsInstance(),
        polyMesh::meshSubDir/"sets"
    );

    Pout<< "Searched : " << mesh.pointsInstance()/polyMesh::meshSubDir/"sets"
        << nl
        << "Found    : " << objects.names() << nl
        << endl;


    IOobjectList pointObjects(objects.lookupClass(pointSet::typeName));

    Pout<< "pointSets:" << pointObjects.names() << endl;

    for
    (
        IOobjectList::const_iterator iter = pointObjects.begin();
        iter != pointObjects.end();
        ++iter
    )
    {
        // Not in memory. Load it.
        pointSet set(*iter());

        label zoneID = mesh.pointZones().findZoneID(set.name());
        if (zoneID == -1)
        {
            Info<< "Adding set " << set.name() << " as a pointZone." << endl;
            label sz = mesh.pointZones().size();
            mesh.pointZones().setSize(sz+1);
            mesh.pointZones().set
            (
                sz,
                new pointZone
                (
                    set.name(),             //name
                    set.toc(),              //addressing
                    sz,                     //index
                    mesh.pointZones()       //pointZoneMesh
                )
            );
            mesh.pointZones().writeOpt() = IOobject::AUTO_WRITE;
        }
        else
        {
            Info<< "Overwriting contents of existing pointZone " << zoneID
                << " with that of set " << set.name() << "." << endl;
            mesh.pointZones()[zoneID] = set.toc();
            mesh.pointZones().writeOpt() = IOobject::AUTO_WRITE;
        }
    }


    IOobjectList cellObjects(objects.lookupClass(cellSet::typeName));

    Pout<< "cellSets:" << cellObjects.names() << endl;

    for
    (
        IOobjectList::const_iterator iter = cellObjects.begin();
        iter != cellObjects.end();
        ++iter
    )
    {
        // Not in memory. Load it.
        cellSet set(*iter());

        label zoneID = mesh.cellZones().findZoneID(set.name());
        if (zoneID == -1)
        {
            Info<< "Adding set " << set.name() << " as a cellZone." << endl;
            label sz = mesh.cellZones().size();
            mesh.cellZones().setSize(sz+1);
            mesh.cellZones().set
            (
                sz,
                new cellZone
                (
                    set.name(),             //name
                    set.toc(),              //addressing
                    sz,                     //index
                    mesh.cellZones()        //pointZoneMesh
                )
            );
            mesh.cellZones().writeOpt() = IOobject::AUTO_WRITE;
        }
        else
        {
            Info<< "Overwriting contents of existing cellZone " << zoneID
                << " with that of set " << set.name() << "." << endl;
            mesh.cellZones()[zoneID] = set.toc();
            mesh.cellZones().writeOpt() = IOobject::AUTO_WRITE;
        }
    }


    IOobjectList faceObjects(objects.lookupClass(faceSet::typeName));

    Pout<< "faceSets:" << faceObjects.names() << endl;

    for
    (
        IOobjectList::const_iterator iter = faceObjects.begin();
        iter != faceObjects.end();
        ++iter
    )
    {
        // Not in memory. Load it.
        faceSet set(*iter());

        DynamicList<label> addressing(set.size());
        DynamicList<bool> flipMap(set.size());

        if (!noFlipMap)
        {
            word setName(set.name() + "Cells");

            Pout<< "Trying to load cellSet " << setName
                << " to find out the flipMap." << nl
                << "If the neighbour side of the face is in the cellSet"
                << " the flipMap becomes true," << nl
                << "in all other cases it stays false."
                << " If you do not care about the flipMap"
                << " (i.e. do not use the sideness)" << nl
                << "use the -noFlipMap command line option."
                << endl;

            // Load corresponding cells
            cellSet cells(mesh, setName);

            forAllConstIter(faceSet, set, iter)
            {
                label faceI = iter.key();

                bool flip = false;

                if (mesh.isInternalFace(faceI))
                {
                    if
                    (
                        cells.found(mesh.faceOwner()[faceI])
                    && !cells.found(mesh.faceNeighbour()[faceI])
                    )
                    {
                        flip = false;
                    }
                    else if
                    (
                       !cells.found(mesh.faceOwner()[faceI])
                     && cells.found(mesh.faceNeighbour()[faceI])
                    )
                    {
                        flip = true;
                    }
                    else
                    {
                        FatalErrorIn(args.executable())
                            << "One of owner or neighbour of internal face "
                            << faceI << " should be in cellSet " << cells.name()
                            << " to be able to determine orientation." << endl
                            << "Face:" << faceI
                            << " own:" << mesh.faceOwner()[faceI]
                            << " OwnInCellSet:"
                            << cells.found(mesh.faceOwner()[faceI])
                            << " nei:" << mesh.faceNeighbour()[faceI]
                            << " NeiInCellSet:"
                            << cells.found(mesh.faceNeighbour()[faceI])
                            << abort(FatalError);
                    }
                }
                else
                {
                    if (cells.found(mesh.faceOwner()[faceI]))
                    {
                        flip = false;
                    }
                    else
                    {
                        flip = true;
                    }
                }

                addressing.append(faceI);
                flipMap.append(flip);
            }
        }
        else
        {
            // No flip map.
            forAllConstIter(faceSet, set, iter)
            {
                addressing.append(iter.key());
                flipMap.append(false);
            }
        }

        label zoneID = mesh.faceZones().findZoneID(set.name());
        if (zoneID == -1)
        {
            Info<< "Adding set " << set.name() << " as a faceZone." << endl;
            label sz = mesh.faceZones().size();
            mesh.faceZones().setSize(sz+1);
            mesh.faceZones().set
            (
                sz,
                new faceZone
                (
                    set.name(),             //name
                    addressing.shrink(),    //addressing
                    flipMap.shrink(),       //flipmap
                    sz,                     //index
                    mesh.faceZones()        //pointZoneMesh
                )
            );
            mesh.faceZones().writeOpt() = IOobject::AUTO_WRITE;
        }
        else
        {
            Info<< "Overwriting contents of existing faceZone " << zoneID
                << " with that of set " << set.name() << "." << endl;
            mesh.faceZones()[zoneID].resetAddressing
            (
                addressing.shrink(),
                flipMap.shrink()
            );
            mesh.faceZones().writeOpt() = IOobject::AUTO_WRITE;
        }
    }

    Pout<< "Writing mesh." << endl;

    if (!mesh.write())
    {
        FatalErrorIn(args.executable())
            << "Failed writing polyMesh."
            << exit(FatalError);
    }

    Pout << nl << "End" << endl;

    return 0;
}


// ************************************************************************* //
