#pragma once

#include "pch.h"

#include "EventInterface.h"
#include "TargetType.h"

struct CreateBeacon
{
	sf::Vector2f Position;
	CW::TargetType Type;
	uint8_t BitDirection;

};

class CreateBeaconObs
	: virtual public CW::OnEvent
{
public:
	virtual ~CreateBeaconObs() = default;
	virtual void OnCreateBeacon(const CreateBeacon*) = 0;
};

// ��� ����������� ���������������� ����� ������� ����� �������� � ���� ����� ������ CW_USER_EVENTS_LIST
// � �������� ����� ������� ���� ������������ �������
#define CW_USER_EVENTS_LIST CreateBeacon

// ���� ���������� ���������������� ���� �������, �� ���������� �������� ������ CW_USER_EVENTS_PAIRS,
// � ������� ����� ������� ����������� ��� ���� ��� ����������� ������ � ��� ������ ������
// <���_����>, <���_������>, <���_����>, <���_������>, ...
#define CW_USER_EVENTS_PAIRS CreateBeaconObs, CreateBeacon

// ������
// 
//struct EventData
//{
//	int a;
//};
//
//class MyEventReciever
//	: virtual public CW::OnEvent
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
//	:virtual public CW::OnEvent
//{
//public:
//	virtual void onOtherEvent(const OtherEventData*) = 0;
//};
//
//
//struct Thing {};
//
//class OnThing
//	: virtual public CW::OnEvent
//{
//public:
//	virtual void gimme(void) = 0;
//};

// #define CW_USER_EVENTS_LIST EventData, OtherEventData, Thing
// #define CW_USER_EVENTS_PAIRS MyEventReciever, EventData, OtherReciever, OtherEventData, OnThing, Thing