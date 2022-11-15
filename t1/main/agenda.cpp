#include "agenda.h"
#include "memoria.h"

void Agenda::logErr(char const *msg) { printf("[Error] %s", msg); }

void Agenda::Database::headerClear()
{
	header_t clearHeader = {0, 1023};
	Agenda::Database::headerWrite(clearHeader);
}

void Agenda::Database::headerWrite(header_t header)
{
	this->mem.escreve(AGENDA_HEADER_OFFSET, header.as_uint8ptr(), sizeof(header));
}

Agenda::header_t Agenda::Database::headerRead()
{
	header_t buff{0, 0};
	this->mem.le(AGENDA_HEADER_OFFSET, buff.as_uint8ptr(), sizeof(buff));
	return buff;
}

void Agenda::Database::registerWrite(index_t index, register_t reg)
{
	Agenda::header_t header = this->headerRead();
	if (index > header.capacity) {
		Agenda::logErr("Capacity overload.");
		return;
	}
	this->mem.escreve(AGENDA_REG_OFFSET + index * sizeof(reg), reg.as_uint8ptr(), sizeof(reg));

	// If we are inserting on the end we need to increase length.
	if (index == header.length) {
		header.length += 1;
		this->headerWrite(header);
	}
}

Agenda::register_t Agenda::Database::registerRead(index_t index)
{
	Agenda::header_t header = this->headerRead();

	if (index > header.length) {
		Agenda::logErr("Index out of bounds.");
		return {};
	}

	Agenda::register_t buff{};
	this->mem.le(AGENDA_REG_OFFSET + index * sizeof(buff), buff.as_uint8ptr(), sizeof(buff));
	return buff;
}

void Agenda::Database::registerRemove(index_t index)
{
	Agenda::header_t header = this->headerRead();

	if (index > header.length) {
		Agenda::logErr("Index out of bounds.");
		return;
	}
	if (header.length <= 0) {
		Agenda::logErr("Database is already empty.");
		return;
	}

	Agenda::register_t last = this->registerRead(header.length - 1);
	this->registerWrite(index, last);

	header.length -= 1;
	this->headerWrite(header);
}
