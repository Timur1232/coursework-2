#pragma once

// Для определения пользовательских типов ивентов нужно объявить в этом файле макрос CW_USER_EVENTS_LIST
// и записать через запятую типы отправляемых ивентов.
// 
// Пользовательские ивенты обязаны наследоваться от CW::Event
//
#define CW_USER_EVENTS_LIST CreateBeacon, CloseApp, StartSimulation