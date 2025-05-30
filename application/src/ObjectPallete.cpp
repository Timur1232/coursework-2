#include "pch.h"
#include "ObjectPallete.h"

namespace CW {

    void ObjectPalleteBuilder::UpdateInterface()
    {
        if (ImGui::BeginListBox("Objects"))
        {
            for (int i = 0; i < m_ObjectsLabels.size(); ++i)
            {
                bool isSelected = m_CurrentIndex == i;
                if (ImGui::Selectable(m_ObjectsLabels.at(i), isSelected))
                {
                    m_CurrentIndex = i;
                }
            }
            ImGui::EndListBox();
            settingsMenu();
        }
        else
        {
            m_CurrentIndex = 0;
        }
    }

    ObjectPallete ObjectPalleteBuilder::GetCurrentType() const
    {
        switch (m_CurrentIndex)
        {
        case 0: return ObjectPallete::None;
        case 1: return ObjectPallete::Beacon;
        case 2: return ObjectPallete::Drone;
        case 3: return ObjectPallete::Resource;
        default: return ObjectPallete::None;
        }
    }

    std::tuple<sf::Angle, TargetType> ObjectPalleteBuilder::GetDroneComponents() const
    {
        return { sf::radians(m_DroneAngle), m_DroneTarget };
    }

    std::tuple<TargetType, sf::Angle> ObjectPalleteBuilder::GetBeaconComponents() const
    {
        return { m_BeaconType, sf::radians(m_BeaconDirection) };
    }

    int ObjectPalleteBuilder::GetRsourceAmount() const
    {
        return m_Amount;
    }

    void ObjectPalleteBuilder::settingsMenu()
    {
        switch (GetCurrentType())
        {
        case ObjectPallete::Beacon:
        {
            ImGui::Text("beacon type");
            if (ImGui::RadioButton("Recource##Object-pallete", m_BeaconType == TargetType::Recource))
                m_BeaconType = TargetType::Recource;
            ImGui::SameLine();
            if (ImGui::RadioButton("Navigation##Object-pallete", m_BeaconType == TargetType::Navigation))
                m_BeaconType = TargetType::Navigation;

            ImGui::SliderAngle("beacon direction", &m_BeaconDirection, 0.0f, 360.0f);
            break;
        }
        case ObjectPallete::Drone:
        {
            ImGui::Text("target type");
            if (ImGui::RadioButton("Recource##Object-pallete", m_BeaconType == TargetType::Recource))
                m_BeaconType = TargetType::Recource;
            ImGui::SameLine();
            if (ImGui::RadioButton("Navigation##Object-pallete", m_BeaconType == TargetType::Navigation))
                m_BeaconType = TargetType::Navigation;

            ImGui::SliderAngle("angle", &m_DroneAngle, 0.0f, 360.0f);
            break;
        }
        case ObjectPallete::Resource:
        {
            ImGui::InputInt("resources amount", &m_Amount);
            break;
        }
        default: break;
        }
    }

} // CW