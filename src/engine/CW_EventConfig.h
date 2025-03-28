#pragma once

#include "EventInterface.h"

//#define CW_DEFINE_USER_DISPATCHER(Reciever_t, Event_t, code, ...) template<> void UserDispatcher<MyEventReciever, EventData>::operator()() { code; } CW_DEFINE_USER_DISPATCHER(__VA_ARGS__)

struct EventData
{
	int a;
};

class MyEventReciever
	: virtual public CW::OnEvent
{
public:
	virtual void onE(const EventData*) = 0;
};

// ��� ����������� ���������������� ����� ������� ����� �������� � ���� ����� ������ CW_USER_EVENTS_LIST
// � �������� ����� ������� ���� ������������ �������
#define CW_USER_EVENTS_LIST EventData

// ���� ���������� ���������������� ���� �������, �� ���������� �������� ������ CW_USER_EVENTS_PAIRS,
// � ������� ����� ������� ����������� ��� ���� ��� ����������� ������ � ��� ������ ������
// <���_����>, <���_������>, <���_����>, <���_������>, ...
#define CW_USER_EVENTS_PAIRS MyEventReciever, EventData