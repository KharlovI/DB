#include "Order.h"
#include <iostream>
#pragma warning(disable : 4996)

void PrintData(Order& temp)
{
	std::cout << "Order data:" << std::endl;
	std::cout << "Company ID: " << temp.companyID << std::endl;
	std::cout << "Order ID: " << temp.orderID << std::endl;
	std::cout << "Departure adress: ";
	for (int i = 0; temp.departureAdress[i] != '\0'; i++)
	{
		std::cout << temp.departureAdress[i];
	}
	std::cout << std::endl;
	std::cout << "Arrival adress: ";
	for (int i = 0; temp.arrivalAdress[i] != '\0'; i++)
	{
		std::cout << temp.arrivalAdress[i];
	}
	std::cout << std::endl;
	std::cout << "Cost: " << temp.cost << std::endl;
	std::cout << "--------------------";
	std::cout << std::endl;
}

int GetFreeIndexForOrder(FILE* order_fl_out)
{
	fseek(order_fl_out, 0, SEEK_SET);

	int pointer = getc(order_fl_out);
	if (pointer == EOF)
		return 0;

	int freeInd;
	Order temp;

	while (pointer != EOF)
	{
		fseek(order_fl_out, -1, SEEK_CUR);
		fread(&temp, sizeof(Order), 1, order_fl_out);

		if (temp.flag)
			return temp.orderID;

		pointer = getc(order_fl_out);
	}

	return temp.orderID + 1;
}

void GetNextOrderPosition(FILE* order_fl_out, FILE* company_fl_out, Order& newOrder, PositionIndex*& index, int& size)
{
	int position = -1;

	for (int i = 0; i < size; i++)   // find master position by ID
	{
		if (newOrder.companyID == index[i].index)
		{
			position = index[i].position;
			break;
		}
	}

	if (position == -1)
	{
		std::cout << "Invalid company ID" << std::endl;
		return;
	}

	Company temp;

	fseek(company_fl_out, sizeof(Company) * position, SEEK_SET);
	fread(&temp, sizeof(Company), 1, company_fl_out);

	if (temp.firstOrderID == -1)				// if master doesn't has any orders 
	{
		newOrder.nextOrderPosition = newOrder.orderID;
		temp.firstOrderID = newOrder.orderID;

		fseek(company_fl_out, sizeof(Company) * position, SEEK_SET);
		fwrite((char*)&temp, sizeof(Company), 1, company_fl_out);

		return;
	}

	if (temp.firstOrderID > newOrder.orderID)
	{
		newOrder.nextOrderPosition = temp.firstOrderID;
		temp.firstOrderID = newOrder.orderID;

		fseek(company_fl_out, sizeof(Company) * position, SEEK_SET);
		fwrite((char*)&temp, sizeof(Company), 1, company_fl_out);

		return;
		
	}
	Order orderTemp;

	fseek(order_fl_out, sizeof(Order) * temp.firstOrderID, SEEK_SET);
	fread(&orderTemp, sizeof(Order), 1, order_fl_out);

	while (orderTemp.nextOrderPosition != orderTemp.orderID)
	{
		fseek(order_fl_out, sizeof(Order) * orderTemp.nextOrderPosition, SEEK_SET);
		fread(&orderTemp, sizeof(Order), 1, order_fl_out);
	}

	orderTemp.nextOrderPosition = newOrder.orderID;
	newOrder.nextOrderPosition = newOrder.orderID;

	fseek(order_fl_out, sizeof(Order) * orderTemp.orderID, SEEK_SET);
	fwrite((char*)&orderTemp, sizeof(Order), 1, order_fl_out);
}

void GetAllOrders(FILE* order_fl_out)
{
	fseek(order_fl_out, 0, SEEK_SET);

	int positionInFile = getc(order_fl_out);
	Order temp;
	std::cout << "--------------------";
	while (positionInFile != EOF)
	{
		fseek(order_fl_out, -1, SEEK_CUR);
		fread(&temp, sizeof(Order), 1, order_fl_out);
		if (temp.flag != 1)
		{
			PrintData(temp);
		}
		positionInFile = getc(order_fl_out);
	}
}
void GetAllOrdersByCompanyID(PositionIndex*& temp, int& tempSize,  FILE* company_fl_out, FILE* order_fl_out, int companyID)
{
	Company company;

	int position = -1;

	for (int i = 0; i < tempSize; i++)
	{	
		if (companyID == temp[i].index)
		{
			position = temp[i].position;
			break;
		}
	}

	if (position == -1)
	{
		std::cout << " ID not found" << std::endl;
		return;
	}

	fseek(company_fl_out, sizeof(Company) * position, SEEK_SET);
	fread(&company, sizeof(Company), 1, company_fl_out);

	if (company.firstOrderID == -1)
	{
		std::cout << "This company does not have any orders " << std::endl;
		return;
	}

	fseek(order_fl_out, sizeof(Order) * company.firstOrderID, SEEK_SET);

	Order order;

	fread(&order, sizeof(Order), 1, order_fl_out);
	std::cout << "--------------------";
	PrintData(order);

	while (order.nextOrderPosition != order.orderID)
	{
		fseek(order_fl_out, sizeof(Order) * order.nextOrderPosition, SEEK_SET);
		fread(&order, sizeof(Order), 1, order_fl_out);
		PrintData(order);
	}
}

void RemoveOrder(FILE* order_fl_out, FILE* company_fl_out, PositionIndex*& index, int& size, int orderID)
{
	fseek(order_fl_out, 0, SEEK_SET);

	int pointer = getc(order_fl_out);
	Order orderTemp;

	if (pointer == EOF)
		return;

	while (pointer != EOF)
	{
		fseek(order_fl_out, -1, SEEK_CUR);
		fread(&orderTemp, sizeof(Order), 1, order_fl_out);

		if (orderTemp.orderID == orderID)
			break;

		pointer = getc(order_fl_out);
	}

	orderTemp.flag = 1;
	
	int positionCompany = -1;

	for (int i = 0; i < size; i++)
	{
		if (orderTemp.companyID == index[i].index)
		{
			positionCompany = index[i].position;
			break;
		}
	}

	if (positionCompany == -1)
		return;
	Company companyTemp;

	fseek(company_fl_out, sizeof(Company) * positionCompany, SEEK_SET);
	fread(&companyTemp, sizeof(Company), 1, company_fl_out);

	if (companyTemp.firstOrderID == orderTemp.orderID)
	{
		if (orderTemp.nextOrderPosition == orderTemp.orderID)				/// Слейв первый и единственный
		{
			companyTemp.firstOrderID = -1;
			fseek(company_fl_out, sizeof(Company) * positionCompany, SEEK_SET);
			fwrite((char*)&companyTemp, sizeof(Company), 1, company_fl_out);

			fseek(order_fl_out, sizeof(Order) * orderTemp.orderID, SEEK_SET);
			fwrite((char*)&orderTemp, sizeof(Order), 1, order_fl_out);

			return;
		}

		companyTemp.firstOrderID = orderTemp.nextOrderPosition;				/// Слей первый, но неединственный 
		fseek(company_fl_out, sizeof(Company) * positionCompany, SEEK_SET);
		fwrite((char*)&companyTemp, sizeof(Company), 1, company_fl_out);

		fseek(order_fl_out, sizeof(Order) * orderTemp.orderID, SEEK_SET);
		fwrite((char*)&orderTemp, sizeof(Order), 1, order_fl_out);


		return;
	}

	if (orderTemp.nextOrderPosition == orderTemp.orderID)					/// Cлейв последний 
	{
		Order seccondOrder;

		fseek(order_fl_out, sizeof(Order) * companyTemp.firstOrderID, SEEK_SET);
		fread(&seccondOrder, sizeof(Order), 1, order_fl_out);

		while (seccondOrder.nextOrderPosition != orderTemp.orderID)
		{
			fseek(order_fl_out, sizeof(Order) * seccondOrder.nextOrderPosition, SEEK_SET);
			fread(&seccondOrder, sizeof(Order), 1, order_fl_out);
		}

		seccondOrder.nextOrderPosition = seccondOrder.orderID;
		fseek(order_fl_out, sizeof(Order) * seccondOrder.orderID, SEEK_SET);
		fwrite((char*)&seccondOrder, sizeof(Order), 1, order_fl_out);

		fseek(order_fl_out, sizeof(Order) * orderTemp.orderID, SEEK_SET);
		fwrite((char*)&orderTemp, sizeof(Order), 1, order_fl_out);

		return;
	}

	Order seccondOrder;														/// Слейв не последний 

	fseek(order_fl_out, sizeof(Order) * companyTemp.firstOrderID, SEEK_SET);
	fread(&seccondOrder, sizeof(Order), 1, order_fl_out);

	while (seccondOrder.nextOrderPosition != orderTemp.orderID)
	{
		fseek(order_fl_out, sizeof(Order) * seccondOrder.nextOrderPosition, SEEK_SET);
		fread(&seccondOrder, sizeof(Order), 1, order_fl_out);
	}

	seccondOrder.nextOrderPosition = orderTemp.nextOrderPosition;

	fseek(order_fl_out, sizeof(Order) * seccondOrder.orderID, SEEK_SET);
	fwrite((char*)&seccondOrder, sizeof(Order), 1, order_fl_out);

	fseek(order_fl_out, sizeof(Order) * orderTemp.orderID, SEEK_SET);
	fwrite((char*)&orderTemp, sizeof(Order), 1, order_fl_out);
}
void UpdateOrder(FILE* order_fl_out, int orderID)
{
	fseek(order_fl_out, 0, SEEK_SET);

	int pointer = getc(order_fl_out);
	Order temp;

	while (pointer != EOF)
	{
		fseek(order_fl_out, -1, SEEK_CUR);
		fread(&temp, sizeof(Order), 1, order_fl_out);
		if (temp.orderID == orderID)
			break;

		pointer = getc(order_fl_out);
	}

	if (temp.orderID != orderID)
		return;

	std::cout << "Departure adress: ";
	std::cin >> temp.departureAdress;
	std::cout << "Arrival adress: ";
	std::cin >> temp.arrivalAdress;
	std::cout << "Cost: ";
	std::cin >> temp.cost;

	fseek(order_fl_out, sizeof(Order) * temp.orderID, SEEK_SET);
	fwrite((char*)&temp, sizeof(Order), 1, order_fl_out);
}
void InsertOrder(FILE* order_fl_out, FILE* company_fl_out,PositionIndex*& index, int& size)
{
	Order temp1;
	std::cout << "Enter order data:" << std::endl;
	std::cout << "Chouse company ID: ";
	for (int i = 0; i < size; i++)
	{
		std::cout << index[i].index << " ";
	}

	std::cout << std::endl;
	std::cin >> temp1.companyID;

	bool flag = 1;

	while (flag)
	{
		for (int i = 0; i < size; i++)
		{
			if (temp1.companyID == index[i].index)
			{
				flag = 0;
				break;
			}
		}

		if (flag)
		{
			std::cout << "ID not found" << std::endl;
			std::cout << "Chouse one of this ID:" << std::endl;
			for (int i = 0; i < size; i++)
			{
				std::cout << index[i].index << " ";
			}

			std::cout << std::endl;
			std::cin >> temp1.companyID;
		}
	}

	temp1.orderID = GetFreeIndexForOrder(order_fl_out);

	std::cout << "Departure adress: ";
	std::cin >> temp1.departureAdress;
	std::cout << "Arrival adress: ";
	std::cin >> temp1.arrivalAdress;
	std::cout << "Cost: ";
	std::cin >> temp1.cost;
	temp1.flag = 0;

	GetNextOrderPosition(order_fl_out, company_fl_out, temp1, index, size);
	
	fseek(order_fl_out, sizeof(Order) * temp1.orderID, SEEK_SET);
	fwrite((char*)&temp1, sizeof(Order), 1, order_fl_out);
	
}