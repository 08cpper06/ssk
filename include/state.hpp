#pragma once


struct state {
	;
};

struct invalid_state {
	operator state() { return state(); }
};
