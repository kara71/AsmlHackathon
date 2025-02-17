//
//! \file
//
// Created by Sander van Woensel / Umut Uyumaz
// Copyright (c) 2018 ASML Netherlands B.V. All rights reserved.
//
//! Example Task
#include "Tasks_ExampleTransmitTask.hpp"

#include "Debug.hpp"
#include "Constants.hpp"

#include "Facilities_MeshNetwork.hpp"

#include <functional>

namespace Tasks {


ExampleTransmitTask::ExampleTransmitTask(Facilities::MeshNetwork& mesh) :
   Task(TASK_SECOND , TASK_FOREVER, std::bind(&ExampleTransmitTask::execute, this)),
   m_mesh(mesh)
{

}

void ExampleTransmitTask::execute()
{
    if(MASTER) {
        String msg = F("Ping from node ");
        msg += m_mesh.getMyNodeId();
        m_mesh.sendBroadcast( msg );
    }
}

} // namespace Tasks
