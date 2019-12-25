// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GenericOctree.h"

struct FSegment
{
    FVector Center;
    FVector Extents;
    class ARider* Owner;
    int Index;
};

struct FSegmentOctreeSemantics
{
    enum { MaxElementsPerLeaf = 16 };
    enum { MinInclusiveElementsPerNode = 7 };
    enum { MaxNodeDepth = 12 };

    typedef FDefaultAllocator ElementAllocator;

    FORCEINLINE static bool AreElementsEqual(const FSegment& A, const FSegment& B)
    {
        return A.Center == B.Center && A.Extents == B.Extents;
    }

    FORCEINLINE static void SetElementId(const FSegment& Element, FOctreeElementId Id)
    {
    }

    FORCEINLINE static FBoxSphereBounds GetBoundingBox(const FSegment& Segment)
    {
        return FBoxSphereBounds(Segment.Center, Segment.Extents, Segment.Extents.GetMax());
    }
};

typedef TOctree<FSegment, FSegmentOctreeSemantics> FSegmentsOctree;