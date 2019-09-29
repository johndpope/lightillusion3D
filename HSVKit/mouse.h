#pragma once
#include"common.h"

struct mouseParam {
	int x, y, event, flags;
};

void mouseCallBack(int event, int x, int y, int flags, void* data) {
	mouseParam* ptr = (mouseParam*)data;
	ptr->x = x; ptr->y = y; ptr->event = event; ptr->flags = flags;
}