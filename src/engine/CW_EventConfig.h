#pragma once

#include "EventInterface.h"

#include "SFML/Graphics.hpp"

#include "BeaconType.h"

struct CreateBeacon
{
	sf::Vector2f position;
	CW::BeaconType type;
};

class OnCreateBeacon
	: virtual public CW_E::OnEvent
{
public:
	virtual ~OnCreateBeacon() = default;
	virtual void onCreateBeacon(const CreateBeacon*) = 0;
};

// ��� ����������� ���������������� ����� ������� ����� �������� � ���� ����� ������ CW_USER_EVENTS_LIST
// � �������� ����� ������� ���� ������������ �������
#define CW_USER_EVENTS_LIST CreateBeacon

// ���� ���������� ���������������� ���� �������, �� ���������� �������� ������ CW_USER_EVENTS_PAIRS,
// � ������� ����� ������� ����������� ��� ���� ��� ����������� ������ � ��� ������ ������
// <���_����>, <���_������>, <���_����>, <���_������>, ...
#define CW_USER_EVENTS_PAIRS OnCreateBeacon, CreateBeacon

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