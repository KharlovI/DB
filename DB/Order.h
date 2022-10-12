#pragma once
#include "Company.h"

struct Order
{
	int companyID;
	int orderID;
	int nextOrderPosition;
	char departureAdress[20];
	char arrivalAdress [20];
	float cost;
	bool flag;
};
int  GetFreeIndexForOrder(FILE* order_fl_out);
void GetNextOrderPosition(FILE* order_fl_out, FILE* company_fl_out, Order& newOrder, PositionIndex*& index, int& size);

void PrintData(Order& temp);

void GetAllOrders(FILE* order_fl_out);
void GetAllOrdersByCompanyID(PositionIndex*& temp, int& tempSize, FILE* company_fl_out, FILE* order_fl_out, int companyID);

void RemoveOrder(FILE* order_fl_out, FILE* company_fl_out, PositionIndex*& index, int& size, int orderID);
void UpdateOrder(FILE* order_fl_out, int orderID);
void InsertOrder(FILE* order_fl_out, FILE* company_fl_out, PositionIndex*& index, int& size);