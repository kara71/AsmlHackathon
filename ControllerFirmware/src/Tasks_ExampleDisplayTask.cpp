//
//! \file
//
// Created by Sander van Woensel / Umut Uyumaz
// Copyright (c) 2018 ASML Netherlands B.V. All rights reserved.
//
//! Example Task to output something to the LED Matrix.
#include "Tasks_ExampleDisplayTask.hpp"

#include "Debug.hpp"
#include "Constants.hpp"

#include "Facilities_MeshNetwork.hpp"

#include <LEDMatrixDriver.hpp>

#include <functional>

namespace Tasks {


const int ExampleDisplayTask::LEDMATRIX_WIDTH = 31;
const int ExampleDisplayTask::LEDMATRIX_HEIGHT = 7;
const int ExampleDisplayTask::LEDMATRIX_SEGMENTS = 4;
const int ExampleDisplayTask::LEDMATRIX_INTENSITY = 5;
const int ExampleDisplayTask::LEDMATRIX_CS_PIN = 16;
const unsigned long ExampleDisplayTask::POLL_DELAY_MS = 100;

//! Initializes the LED Matrix display.
ExampleDisplayTask::ExampleDisplayTask(Facilities::MeshNetwork& mesh) :
   Task(POLL_DELAY_MS , TASK_FOREVER, std::bind(&ExampleDisplayTask::execute, this)),
   m_mesh(mesh),
   m_lmd(LEDMATRIX_SEGMENTS, LEDMATRIX_CS_PIN),
   m_x(0)
{
   m_lmd.setEnabled(true);
   m_lmd.setIntensity(LEDMATRIX_INTENSITY);

   if(MASTER) {
       m_lmd.clear();
       m_lmd.setPixel(0, 0, true);
       m_lmd.display();
   } else {
       m_mesh.onReceive(std::bind(&ExampleDisplayTask::receivedJsonPacket, this, std::placeholders::_1, std::placeholders::_2));
   }
}

//! Update display
void ExampleDisplayTask::execute()
{
    if(!MASTER) {
        m_lmd.clear();
        m_lmd.setColumn(m_x^24, 255);
        m_lmd.display();
    }
}

void ExampleDisplayTask::receivedCb(Facilities::MeshNetwork::NodeId nodeId, String& msg)
{
   MY_DEBUG_PRINTLN("Received data in ExampleDisplayTask");

   if(++m_x>LEDMATRIX_WIDTH)
   {
      m_x=0;
   }
   
}

void ExampleDisplayTask::receivedJsonPacket(Facilities::MeshNetwork::NodeId nodeId, String& msg)
{
    //parse json packet
    DynamicJsonBuffer jsonBuffer;
    JsonObject& root = jsonBuffer.parseObject(msg);
    
    if (root.containsKey("type")) {
        if (String(MSG_TYPE_IMG).equals(root["type"].as<String>())) {
            // check for on: true or false
            size_t nodeId = root["nodeId"];
            if(1) //check if destined to this nodeId == mesh.getNodeId(); 
            {
                //TODO: call local interface
            }
        }
        
    }

}

} // namespace Tasks
