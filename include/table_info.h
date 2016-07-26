#ifndef TABLE_STRUCTURE_H
#define TABLE_STRUCTURE_H

#include <stdint.h>
#include "general.h"

#define FIELD_TYPE_CHAR			1
#define FIELD_TYPE_TIMESTAMP	2
#define FIELD_TYPE_NUMBERIC		3

typedef struct field_info_s {
	char name[BYTE64];
	uint16_t type;
	uint16_t precision;
	uint16_t scale;
}field_info_t;

typedef struct table_info_s {
	uint16_t box_id;
	uint16_t table_id;
	uint32_t time_window;
	char prefix[BYTE16];
	uint8_t field_num;
	field_info_t cols[BYTE256];
}table_info_t;

#endif

