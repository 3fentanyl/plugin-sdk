#include <plugin.h>
#include "game_sa\common.h"
#include "game_sa\CAutomobile.h"
#include "game_sa\CTimer.h"
#include "game_sa\CMessages.h"

using namespace plugin;

class OpenDoorExample {
public:
	enum eOpenDoorState {
		STATE_OPENING, // ������ �� ���-�� ���������
		STATE_CLOSING, // ���������
		STATE_WAITING  // ��� ��������
	};

	static eOpenDoorState m_currentState; // ��� ������
	static CAutomobile *m_pCurrentCar; // ����, � ������� ������ ���-�� ������
	static float m_openingState; // ������ ��������/��������
	static int m_currentComponentId;
	static eDoors m_currentDoorId;
	static unsigned int m_nLastTimeWhenAnyActionWasEnabled;

	static void OpenComponent(CAutomobile *automobile, int componentId, eDoors doorId) {
		if (automobile->IsComponentPresent(componentId)) {
			if (automobile->m_damageManager.GetDoorStatus(doorId) != 4) { // 3 - ��� ���� ���� ������-�� ������������, ����. STATUS_DAMAGED
				m_pCurrentCar = automobile; // ��������� ��� ������ � ����������, ����� ����� ������� ��� ��������� STATE_OPENING/STATE_CLOSING (��. ������� Process)
				m_currentComponentId = componentId;
				m_currentDoorId = doorId;
				if (!automobile->IsDoorFullyOpen(doorId)) {
					m_currentState = STATE_OPENING; // ����������� ������
					m_openingState = 0.0f; // � ������� ������ ��������
				}
				else {
					m_currentState = STATE_CLOSING; // ����������� ������
					m_openingState = 1.0f; // � ������� ������ ��������
				}
				m_nLastTimeWhenAnyActionWasEnabled = CTimer::m_snTimeInMilliseconds; // ���������� �����
			}
		}
	}

	static void Process() { // ������������ �������� ��������� - ��� ������� ������� ��� ��������� ��� ��������� ���������
		switch (m_currentState) {
		case STATE_OPENING: // ���-�� ���������. ������ ��������, ��� ����� �� �������� �������� ���� � �� ����������� � ����������� ���������� ������ (�� ����, ��� ���������� ����������)
			m_pCurrentCar->OpenDoor(0, m_currentComponentId, m_currentDoorId, m_openingState, true);
			m_openingState += 0.05f;
			if (m_openingState > 1.0f) { // ���� ��������� �������
				m_currentState = STATE_WAITING; // ����������� ������ �� "��������"
				m_pCurrentCar->OpenDoor(0, m_currentComponentId, m_currentDoorId, 1.0f, true); // ��������� ���������
			}
			break;
		case STATE_CLOSING: // ���-�� ���������
			m_pCurrentCar->OpenDoor(0, m_currentComponentId, m_currentDoorId, m_openingState, true);
			m_openingState -= 0.05f;
			if (m_openingState < 0.0f) { // ���� ��������� �������
				m_currentState = STATE_WAITING; // ����������� ������ �� "��������"
				m_pCurrentCar->OpenDoor(0, m_currentComponentId, m_currentDoorId, 0.0f, true); // ��������� ���������
			}
			break;
		case STATE_WAITING:
			if (CTimer::m_snTimeInMilliseconds >(m_nLastTimeWhenAnyActionWasEnabled + 500)) { // ���� ������ 500 �� � ���� �������, ��� �� ������ ���������/��������� ���-��
				CVehicle *vehicle = FindPlayerVehicle(0, false);
				if (vehicle && vehicle->m_dwVehicleClass == VEHICLE_AUTOMOBILE) {
					CAutomobile *automobile = reinterpret_cast<CAutomobile *>(vehicle); // ����� ��, ���������� �����. �.�. �� ����� ����� damageManager, ��� ����� ���������, ��� ��������� - ��� ���������� (CAutomobile)
					if (KeyPressed(219)) // [
						OpenComponent(automobile, CAR_BONNET, static_cast<eDoors>(0)); // �����
						//OpenComponent(automobile, CAR_BONNET, BONNET);
					else if (KeyPressed(221)) // ]
						OpenComponent(automobile, CAR_BOOT, static_cast<eDoors>(1)); // ��������
						//OpenComponent(automobile, CAR_BOOT, BOOT);
					else if (KeyPressed(186) && KeyPressed(187)) // ; =
						OpenComponent(automobile, CAR_DOOR_LF, static_cast<eDoors>(2)); // ����� �������� �����
						//OpenComponent(automobile, CAR_DOOR_LF, DOOR_FRONT_LEFT);
					else if (KeyPressed(222) && KeyPressed(187)) // ' =
						OpenComponent(automobile, CAR_DOOR_RF, static_cast<eDoors>(3)); // ������ �������� �����
						//OpenComponent(automobile, CAR_DOOR_RF, DOOR_FRONT_RIGHT);
					else if (KeyPressed(186) && KeyPressed(189)) // ; -
						OpenComponent(automobile, CAR_DOOR_LR, static_cast<eDoors>(4)); // ����� ������ �����
						//OpenComponent(automobile, CAR_DOOR_LR, DOOR_REAR_LEFT);
					else if (KeyPressed(222) && KeyPressed(189)) // ' -
						OpenComponent(automobile, CAR_DOOR_RR, static_cast<eDoors>(5)); // ������ ������ �����
						//OpenComponent(automobile, CAR_DOOR_RR, DOOR_REAR_RIGHT);
				}
			}
		}

		switch (m_currentState) { // ��� ����������� ������� �������� ������ �� �����
		case STATE_OPENING:
			CMessages::AddMessageJumpQ("state: opening", 100, 0, false);
			break;
		case STATE_CLOSING:
			CMessages::AddMessageJumpQ("state: closing", 100, 0, false);
			break;
		case STATE_WAITING:
			CMessages::AddMessageJumpQ("state: waiting", 100, 0, false);
			break;
		}
	}

	OpenDoorExample() {
		Events::gameProcessEvent.Add(Process);
	}
} example;

OpenDoorExample::eOpenDoorState OpenDoorExample::m_currentState = STATE_WAITING;
CAutomobile *OpenDoorExample::m_pCurrentCar;
float OpenDoorExample::m_openingState;
int OpenDoorExample::m_currentComponentId;
eDoors OpenDoorExample::m_currentDoorId;
unsigned int OpenDoorExample::m_nLastTimeWhenAnyActionWasEnabled = 0;