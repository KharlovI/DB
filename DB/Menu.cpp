#include "Menu.h"
#include "Order.h"
#pragma warning(disable : 4996)

void Start()
{
	RewriteFile();

	int action;
	int size = 0;
	int companyID;
	int orderID;

	PrintActions();
	std::cout << std::endl;

	action = Action();

	PositionIndex* index = GetPositionIndex(size);

	FILE* companyFL = fopen("Company.fl", "r+b");
	FILE* orderFL   = fopen("Orders.fl", "r+b");

	while (action != 0)
	{
		switch (action)
		{
		case 0:
			break;

		case 1:
			GetAllCompanys(companyFL);
			break;
		
		case 2:
			std::cout << "Enter company ID: ";
			std::cin >> companyID;

			GetCompanyByID(companyFL, index, size, companyID);
			break;
		
		case 3:
			std::cout << "Enter company ID: ";
			std::cin >> companyID;

			RemoveCompany(companyFL, index, size, orderFL, companyID);
			break;
		
		case 4:
			std::cout << "Enter company ID: ";
			std::cin >> companyID;

			UpdateCompany(companyFL, index, size, companyID);
			break;

		case 5:
			InsertCompany(companyFL, index, size);
			break;

		case 6:
			std::cout << std::endl;
			std::cout << "Orders: " << std::endl;
			GetAllOrders(orderFL);
			break;

		case 7:

			std::cout << "Enter compay ID: ";
			std::cin >> companyID;

			GetAllOrdersByCompanyID(index, size,companyFL,orderFL, companyID);
			break;

		case 8:
			std::cout << std::endl;
			std::cout << "Enter order ID: ";
			std::cin >> orderID;

			RemoveOrder(orderFL, companyFL, index, size, orderID);
			break;

		case 9:
			std::cout << "Enter order ID: ";
			std::cin >> orderID;

			UpdateOrder(orderFL, orderID);
			break;

		case 10:

			InsertOrder(orderFL, companyFL, index, size);
			break;

		default:
			std::cout << "Choose another action" << std::endl;
			break;
		}

		std::cout << std::endl;
		std::cout << std::endl;

		PrintActions();
		std::cout << std::endl;
		std::cout << std::endl;
		if (action != 0);
		action = Action();
	}

	fclose(companyFL);
	fclose(orderFL);

	GarbageCollectCompany();
	SetPositionIndex(index, size);
}

void PrintActions()
{
	std::cout << "Actions: "						<< std::endl;
	std::cout << "1)  Print all companys"			<< std::endl;
	std::cout << "2)  Print company by ID "			<< std::endl;
	std::cout << "3)  Remove company "				<< std::endl;
	std::cout << "4)  Update company"				<< std::endl;
	std::cout << "5)  Insert company"				<< std::endl;
	std::cout << "6)  Print all orders"				<< std::endl;
	std::cout << "7)  Print orders by company ID"	<< std::endl;
	std::cout << "8)  Remove order by ID "			<< std::endl;
	std::cout << "9)  Update order"					<< std::endl; 
	std::cout << "10) Insert order"					<< std::endl;
	std::cout << "0)  Exit"						    << std::endl;
}

int Action()
{
	int answer;
	
	std::cout << "Chose action:" << std::endl;
	std::cin >> answer;

	while (answer < 0 || answer > 10)
	{
		std::cout << "Incorrect answer" << std::endl;
		std::cin >> answer;
	}
	return answer;
}
