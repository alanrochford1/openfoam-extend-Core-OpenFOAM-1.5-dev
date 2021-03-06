/*---------------------------------------------------------------------------*\
 =========                   |
 \\      /   F ield          | OpenFOAM: The Open Source CFD Toolbox
  \\    /    O peration      |
   \\  /     A nd            | Copyright held by original author
    \\/      M anipulation   |
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

Application
    mdEquilibrationFOAM

Description
    Equilibrates and/or preconditions MD systems

\*---------------------------------------------------------------------------*/

#include "fvCFD.H"
#include "md.H"

int main(int argc, char *argv[])
{

#   include "setRootCase.H"
#   include "createTime.H"
#   include "createMesh.H"

    moleculeCloud molecules(mesh);

    molecules.removeHighEnergyOverlaps();

#   include "temperatureAndPressureVariables.H"

#   include "readmdEquilibrationDict.H"

    label nAveragingSteps = 0;

    Info << "\nStarting time loop\n" << endl;

    while (runTime.run())
    {
        runTime++;

        nAveragingSteps++;

        Info << "Time = " << runTime.timeName() << endl;

        molecules.integrateEquationsOfMotion();

#       include "meanMomentumEnergyAndNMols.H"

#       include "temperatureAndPressure.H"

#       include "temperatureEquilibration.H"

        runTime.write();

        if (runTime.outputTime())
        {
            nAveragingSteps = 0;
        }

        Info << "ExecutionTime = " << runTime.elapsedCpuTime() << " s"
            << "  ClockTime = " << runTime.elapsedClockTime() << " s"
            << nl << endl;
    }

    Info << "End\n" << endl;

    return(0);
}
