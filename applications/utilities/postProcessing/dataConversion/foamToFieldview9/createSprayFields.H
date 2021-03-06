// Construct of ptrs to all spray fields
List<IOField<scalar>* > sprayScalarFieldPtrs
(
    sprayScalarNames.size(),
    reinterpret_cast<IOField<scalar>*>(NULL)
);

List<IOField<vector>* > sprayVectorFieldPtrs
(
    sprayVectorNames.size(),
    reinterpret_cast<IOField<vector>*>(NULL)
);

{
     int sprayFieldI = 0;

    // Set scalar fields
    forAll(sprayScalarNames, fieldI)
    {
        IOobject ioHeader
        (
            sprayScalarNames[fieldI],
            runTime.timeName(),
            "lagrangian",
            mesh,
            IOobject::MUST_READ,
            IOobject::NO_WRITE
        );

        if (ioHeader.headerOk())
        {
            sprayScalarFieldPtrs[sprayFieldI] = new IOField<scalar>(ioHeader);
        }
        else
        {
            Info<< "    dummy lagrangian field for "
                << sprayScalarNames[fieldI] << endl;
        }

        sprayFieldI++;
    }
}


// Set vector fields

{
    int sprayFieldI = 0;

    forAll(sprayVectorNames, fieldI)
    {
        IOobject ioHeader
        (
            sprayVectorNames[fieldI],
            runTime.timeName(),
            "lagrangian",
            mesh,
            IOobject::MUST_READ,
            IOobject::NO_WRITE
        );

        if (ioHeader.headerOk())
        {
            sprayVectorFieldPtrs[sprayFieldI] = new IOField<vector>(ioHeader);
        }
        else
        {
            Info<< "    dummy lagrangian field for "
                << sprayVectorNames[fieldI] << endl;
        }

        sprayFieldI++;
    }
}

