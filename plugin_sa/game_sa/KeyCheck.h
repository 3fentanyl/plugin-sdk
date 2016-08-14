#pragma once
     
class KeyCheck {
    static unsigned char currStates[256];
    static unsigned char prevStates[256];
    static unsigned int timeDelayPressed[256];
public:
    static void Update(); // ������ ������. ����� �������� ��� ������� ���� ��� �� �����, ����� ���, ��� ��������� �������
     
    static bool Check(unsigned int key); // ���������, ������ �� ������ �������
    static bool CheckJustDown(unsigned int key); // ���������, ���� �� ������ ������� ����� ������
    static bool CheckJustUp(unsigned int key); // ���������, ���� �� �������� ������� ����� ������
    static bool CheckWithDelay(unsigned int key, unsigned int time); // ��������� ������ �� �������, � ���������� ��������
};