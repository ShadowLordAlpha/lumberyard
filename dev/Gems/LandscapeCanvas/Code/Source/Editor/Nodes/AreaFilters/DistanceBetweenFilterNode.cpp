/*
* All or portions of this file Copyright (c) Amazon.com, Inc. or its affiliates or
* its licensors.
*
* For complete copyright and license terms please see the LICENSE at the root of this
* distribution (the "License"). All use of this software is governed by the License,
* or, if provided, by the license below or the license accompanying this file. Do not
* remove or modify any license notices. This file is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
*
*/

// Qt
#include <QObject>

// AZ
#include <AzCore/Serialization/SerializeContext.h>

// Landscape Canvas
#include "DistanceBetweenFilterNode.h"
#include <Editor/Core/GraphContext.h>

namespace LandscapeCanvas
{
    void DistanceBetweenFilterNode::Reflect(AZ::ReflectContext* context)
    {
        AZ::SerializeContext* serializeContext = azrtti_cast<AZ::SerializeContext*>(context);
        if (serializeContext)
        {
            serializeContext->Class<DistanceBetweenFilterNode, BaseAreaFilterNode>()
                ->Version(0)
                ;
        }
    }

    const QString DistanceBetweenFilterNode::TITLE = QObject::tr("Distance Between Filter");

    DistanceBetweenFilterNode::DistanceBetweenFilterNode(GraphModel::GraphPtr graph)
        : BaseAreaFilterNode(graph)
    {
        CreateSlotData();
    }
} // namespace LandscapeCanvas
