#ifndef __AGENDA_H__
#define __AGENDA_H__

#include "memoria.h"
#include <stdint.h>

#define AGENDA_HEADER_OFFSET 0
#define AGENDA_HEADER_SIZE 64 //(sizeof(Agenda::header_t))

#define AGENDA_REG_OFFSET (AGENDA_HEADER_OFFSET + AGENDA_HEADER_SIZE)
#define AGENDA_REG_SIZE 64 // sizeof(register_t)

namespace Agenda
{
	void logErr(char const *msg);

	typedef uint16_t index_t;

	typedef struct {
		int length;
		int capacity;
		uint8_t *as_uint8ptr() { return reinterpret_cast<uint8_t *>(this); }
		void pprint() const { printf("header_t { length=%d, capacity=%d }\n", this->length, this->capacity); }
	} header_t;

	typedef struct {
		char name[20];
		char phone[14];
		char addr[30];
		uint8_t *as_uint8ptr() { return reinterpret_cast<uint8_t *>(this); }
		void pprint() const
		{
			printf("register_t { name=%s, phone=%s, addr=%s }\n", this->name, this->phone, this->addr);
		}
	} register_t;

	class Database
	{
	public:
		Database(Memoria mem) : mem(mem){};

		void headerWrite(header_t header);
		header_t headerRead();
		void headerClear();

		void registerWrite(index_t index, register_t reg);
		register_t registerRead(index_t index);
		void registerRemove(index_t index);

	private:
		header_t header;
		Memoria mem;
	};
}

#endif