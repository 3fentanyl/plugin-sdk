#include <plugin.h>
#include "game_sa\CCarCtrl.h"
#include "game_sa\common.h"
#include "game_sa\CStreaming.h"
#include "game_sa\CAutomobile.h"
#include "game_sa\CTheScripts.h"
#include "game_sa\CTimer.h"

using namespace plugin;

class CreateCarExample {
public:
    static unsigned int m_nLastSpawnedTime; // ����� ���������� ������ ; last spawned time

    // ������� �������� ���� � ��������� ������� CreateCar ; Put everything related to car creation into CreateCar function
    static CVehicle *CreateCar(int modelId, CVector posn) {
        CVehicle *result = nullptr;
        // ��������� ������ ; Load model
        unsigned int previousFlags = CStreaming::ms_aInfoForModel[modelId].m_flags;
        CStreaming::RequestModel(modelId, 2);
        CStreaming::LoadAllRequestedModels(false);
        if (CStreaming::ms_aInfoForModel[modelId].m_loadState == LOADSTATE_LOADED) { // ���� ������ ����� ��������� ; ensure model is loaded
            if (!(previousFlags & GAME_REQUIRED)) { // ���� ������ �� ���� ����� ; if model was not required previously
                CStreaming::SetModelIsDeletable(modelId); // ��� ������ ����, ��� ������ ����� ����� ������� ; tell game it could delete this model
                CStreaming::SetModelTxdIsDeletable(modelId); // �� �� ����� - ��� txd ; same for txd
            }
            result = CCarCtrl::CreateCarForScript(modelId, posn, false); // ������ ���� � ����������� ; create a car at coords
            if (result)
                CTheScripts::CleanUpThisVehicle(result);
        }
        return result;
    }

    // ���� ������� ������ ; Our spawning function
    static void ProcessSpawn() {
        if (KeyPressed(VK_TAB) && CTimer::m_snTimeInMilliseconds > (m_nLastSpawnedTime + 1000)) { // ���� ������ ������� � ������ ������ ������� � ������� ���������� ������
            CPed *player = FindPlayerPed(0); // ������� ������ ; find player
            if (player) { // ���� ������ ; if found
                CVector position = player->TransformFromObjectSpace(CVector(0.0f, 3.0f, 0.0f)); // �������� ���������� �� ������ �� ������ ; get coords on offset from player
                CAutomobile *automobile = reinterpret_cast<CAutomobile *>(CreateCar(MODEL_INFERNUS, position)); // ������ � ���� ����������� ���� ; Create a car at these coords
                if (automobile) { // ���� ���� ������� ; if car was created
                    m_nLastSpawnedTime = CTimer::m_snTimeInMilliseconds; // ���������� ����� ����� ������ ; store new spawning time
                    if (automobile->m_pRwClump) { // ���� ������ ����������� ������ (RpClump) ; if graphical object was created (RpClump)
                        RwFrame *component = CClumpModelInfo::GetFrameFromName(automobile->m_pRwClump, "bonnet_dummy"); // ������� ��������� � �������� ; find component in hierarchy
                        if (component) // ���� ��������� ���� � �������� ; if component found
                            component->modelling.pos.z += 1.0f; // ������� ��������� ����� ; move component up

                        // ��� ���� ������� ���������� ����������� - ���������� � �����������, ������� �������� � ��������� ���� ; Another way to control components - work with components that are stored in car structure
                        if (automobile->m_aCarNodes[CAR_BUMP_FRONT]) // ���� ��������� ������������ ; if component present
                            automobile->m_aCarNodes[CAR_BUMP_FRONT]->modelling.pos.z += 1.0f; // ������� ��������� ����� ; move component up
                    }
                }
            }
        }
    }

    CreateCarExample() {
        // ��������� ���� ������� � gameProcessEvent; Add our function to gameProcessEvent
        Events::gameProcessEvent.Add(ProcessSpawn); // ��� �� ��, ��� � Events::gameProcessEvent += ProcessSpawn ; Same as Events::gameProcessEvent += ProcessSpawn
    }
} example;

unsigned int CreateCarExample::m_nLastSpawnedTime = 0;