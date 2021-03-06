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

\*---------------------------------------------------------------------------*/


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{


// Get type of octant
template <class Type>
inline label treeNode<Type>::getVolType(const label octant) const
{
    return (volType_ >> 2*octant) & 0x3;
}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

template <class Type>
inline const point& treeNode<Type>::mid() const
{
    return mid_;
}

template <class Type>
inline treeElem<Type>* const* treeNode<Type>::subNodes() const
{
    return subNodes_;
}

// octant contains pointer to treeNode(1) or treeLeaf(0)
template <class Type>
inline label treeNode<Type>::isNode(const label octant) const
{
    return subNodeTypes_ & (0x1 << octant);
}

// Get pointer to sub node
template <class Type>
inline treeNode<Type>* treeNode<Type>::getNodePtr(const label octant) const
{
#   ifdef FULLDEBUG
    if (!isNode(octant))
    {
        FatalErrorIn("treeNode::getNodePtr(const label)")
            << "not a treeNode"
            << abort(FatalError);
    }
#   endif

    return static_cast<treeNode<Type>*>(subNodes_[octant]);
}


// Get pointer to sub leaf
template <class Type>
inline treeLeaf<Type>* treeNode<Type>::getLeafPtr(const label octant) const
{
#   ifdef FULLDEBUG
    if (isNode(octant))
    {
        FatalErrorIn("treeNode::getLeafPtr(const label)")
            << "not a treeLeaf"
            << abort(FatalError);
    }
#   endif

    return static_cast<treeLeaf<Type>*>(subNodes_[octant]);
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// ************************************************************************* //
