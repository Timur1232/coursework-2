#pragma once

#include "EventInterface.h"

struct BeaconDischarge
{
	size_t index;
};

class OnBeaconDischarge
	: virtual public CW_E::OnEvent
{
public:
	virtual ~OnBeaconDischarge() = default;
	virtual void onBeaconDischarge(const BeaconDischarge*) = 0;
};

// ��� ����������� ���������������� ����� ������� ����� �������� � ���� ����� ������ CW_USER_EVENTS_LIST
// � �������� ����� ������� ���� ������������ �������
#define CW_USER_EVENTS_LIST BeaconDischarge

// ���� ���������� ���������������� ���� �������, �� ���������� �������� ������ CW_USER_EVENTS_PAIRS,
// � ������� ����� ������� ����������� ��� ���� ��� ����������� ������ � ��� ������ ������
// <���_����>, <���_������>, <���_����>, <���_������>, ...
#define CW_USER_EVENTS_PAIRS OnBeaconDischarge, BeaconDischarge

// ������
// 
//struct EventData
//{
//	int a;
//};
//
//class MyEventReciever
//	: virtual public CW_E::OnEvent
//{
//public:
//	virtual void onE(const EventData*) = 0;
//};
//
//
//struct OtherEventData
//{
//	float f;
//	int i;
//};
//
//class OtherReciever
//	:virtual public CW_E::OnEvent
//{
//public:
//	virtual void onOtherEvent(const OtherEventData*) = 0;
//};
//
//
//struct Thing {};
//
//class OnThing
//	: virtual public CW_E::OnEvent
//{
//public:
//	virtual void gimme(void) = 0;
//};

// #define CW_USER_EVENTS_LIST EventData, OtherEventData, Thing
// #define CW_USER_EVENTS_PAIRS MyEventReciever, EventData, OtherReciever, OtherEventData, OnThing, Thing