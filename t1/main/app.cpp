/* Hello World Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include <string.h>

#include "esp_spi_flash.h"
#include "esp_system.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "memoria.h"
#include "serial.h"
#include <inttypes.h>

#include "agenda.h"

void print_menu()
{
	printf("--- MENU\n\n"
	       "[0] - Formatar registros\n"
	       "[1] - Listar todos registros\n"
	       "[2] - Contar registros\n"
	       "[3] - Inserir registro\n"
	       "[4] - Remover registro\n"
	       "[5] - Pesquisar registros por nome\n"
	       "[6] - Pesquisar registros por telefone\n");
}

extern "C" void app_main();
int v = 0;

void app_main()
{

	serial.begin(9600);

	Memoria memoria_i2c = Memoria(); // pinos de dados eh o 16 e pino de clock eh o 5
	memoria_i2c.init(0);

	Agenda::Database agenda = Agenda::Database(memoria_i2c);

	char readBuff[100];
	printf("--- AGENDA EEPROM ---\n");
	while (true) {
		print_menu();
		printf("\nInput: \n");

		char option = serial.readChar();

		printf("---\n");
		Agenda::register_t reg{};
		Agenda::register_t regSearch{};
		Agenda::header_t header{};
		switch (option) {
			case '0': { // Format
				printf("Formatando...\n");
				agenda.headerClear();
				printf("Formatado com sucesso!\n");
				break;
			}

			case '1': { // List
				printf("Listando registros...\n");
				header = agenda.headerRead();
				if (header.length <= 0) {
					printf("Nao ha nenhum registro para ser listado!\n");
					break;
				}
				for (size_t i = 0; i < header.length; i++) {
					printf("Registro %d:\n", i);
					reg = agenda.registerRead(i);
					printf(
					    "\tNome: %s\n"
					    "\tTelefone: %s\n"
					    "\tEndereco: %s\n",
					    reg.name,
					    reg.phone,
					    reg.addr
					);
				}
				printf("Registros listados com sucesso!\n");
				break;
			}

			case '2': { // count
				printf("Cotando registros...\n");
				header = agenda.headerRead();
				printf("Existem %d registros.\n", header.length);
				printf("Ha espaco livre para %d registros!\n", header.capacity - header.length);
				break;
			}

			case '3': { // insert
				printf("Inserindo registro...\n");
				Agenda::header_t header = agenda.headerRead();
				if (header.capacity <= header.length) {
					printf("Erro: Nao ha capacidade de armazenamento!\n");
					break;
				}
				reg = {};
				printf("Insira os dados do registro:\n");

				printf("\tNome: ");
				serial.readString(reinterpret_cast<uint8_t *>(&readBuff), sizeof(Agenda::register_t::name));
				strcpy(reg.name, readBuff);

				printf("\n\tTelefone:");
				serial.readString(reinterpret_cast<uint8_t *>(&readBuff), sizeof(Agenda::register_t::phone));
				strcpy(reg.phone, readBuff);

				printf("\n\tEndereco: ");
				serial.readString(reinterpret_cast<uint8_t *>(&readBuff), sizeof(Agenda::register_t::addr));
				strcpy(reg.addr, readBuff);

				agenda.registerWrite(header.length, reg);

				printf("\nRegistro inserido com sucesso!\n");
				break;
			}

			case '4': { // remove
				printf("Removendo registro...\n");
				printf("Procurando registro para ser removido...\n");
				header = agenda.headerRead();
				if (header.length <= 0) {
					printf("Nao ha nenhum registro para ser procurado!\n");
					break;
				}

				printf("\tInsira o telefone a ser procurado: ");
				serial.readString(reinterpret_cast<uint8_t *>(&readBuff), sizeof(Agenda::register_t::phone));
				strcpy(regSearch.phone, readBuff);

				printf("Procurando.");
				bool found = false;
				int foundIndex = -1;
				for (size_t i = 0; i < header.length; i++) {
					printf(".");
					reg = agenda.registerRead(i);

					if (strcmp(reg.phone, regSearch.phone) == 0) {
						printf("\nRegistro encontrado!\n");
						printf(
						    "\tIndice: %d\n"
						    "\tNome: %s\n"
						    "\tTelefone: %s\n"
						    "\tEndereco: %s\n",
						    i,
						    reg.name,
						    reg.phone,
						    reg.addr
						);
						foundIndex = static_cast<int>(i);
						found = true;
						break;
					}
				}
				if (found) {
					agenda.registerRemove(foundIndex);
					printf("\nRegistro removido com sucesso!\n");
				} else {
					printf("\nErro: Registro nao encontrado!\n");
				}
				break;
			}

			case '5': { // search name
				printf("Procurando registro...\n");
				header = agenda.headerRead();
				if (header.length <= 0) {
					printf("Nao ha nenhum registro para ser procurado!\n");
					break;
				}

				printf("\tInsira o nome a ser procurado: ");
				serial.readString(reinterpret_cast<uint8_t *>(&readBuff), sizeof(Agenda::register_t::name));
				strcpy(regSearch.name, readBuff);

				printf("Procurando.");
				bool found = false;
				for (size_t i = 0; i < header.length; i++) {
					printf(".");
					reg = agenda.registerRead(i);

					if (strcmp(reg.name, regSearch.name) == 0) {
						printf("\nRegistro encontrado!\n");
						printf(
						    "\tIndice: %d\n"
						    "\tNome: %s\n"
						    "\tTelefone: %s\n"
						    "\tEndereco: %s\n",
						    i,
						    reg.name,
						    reg.phone,
						    reg.addr
						);
						found = true;
						break;
					}
				}
				if (!found) {
					printf("\nErro: Registro nao encontrado!\n");
				}
				break;
			}

			case '6': { // search phone
				printf("Procurando registro...\n");
				header = agenda.headerRead();
				if (header.length <= 0) {
					printf("Nao ha nenhum registro para ser procurado!\n");
					break;
				}

				printf("\tInsira o telefone a ser procurado: ");
				serial.readString(reinterpret_cast<uint8_t *>(&readBuff), sizeof(Agenda::register_t::phone));
				strcpy(regSearch.phone, readBuff);

				printf("Procurando.");
				bool found = false;
				for (size_t i = 0; i < header.length; i++) {
					printf(".");
					reg = agenda.registerRead(i);

					if (strcmp(reg.phone, regSearch.phone) == 0) {
						printf("\nRegistro encontrado!\n");
						printf(
						    "\tIndice: %d\n"
						    "\tNome: %s\n"
						    "\tTelefone: %s\n"
						    "\tEndereco: %s\n",
						    i,
						    reg.name,
						    reg.phone,
						    reg.addr
						);
						found = true;
						break;
					}
				}
				if (!found) {
					printf("\nErro: Registro nao encontrado!\n");
				}
				break;
			}

			default: {
				printf("Entrada nao reconhecida.\n");
				break;
			}
		}
		printf("---\n\n");
	}
}
