#include "Company.h"
#include <iostream>
#include "Order.h"

#pragma warning(disable : 4996)


PositionIndex* GetPositionIndex(int& tempSize)
{
	FILE* indexFile = fopen("Company.ind", "rb");

	int maxPosition = 0;
	int iter = getc(indexFile);
	if (iter == EOF)
		return nullptr;

	while (iter != EOF)
	{
		fseek(indexFile, sizeof(int) * 2 - 1, SEEK_CUR);
		iter = getc(indexFile);
		maxPosition++;
	}

	tempSize = maxPosition;

	PositionIndex* temp = new PositionIndex[tempSize];

	fseek(indexFile, 0, SEEK_SET);
	iter = getc(indexFile);

	int position;
	int index;
	int i = 0;

	while (iter != EOF)
	{
		fseek(indexFile, -1, SEEK_CUR);
		fread(&position, sizeof(int), 1, indexFile);
		fread(&index, sizeof(int), 1, indexFile);

		temp[i].index = index;
		temp[i].position = position;

		i++;
		iter = getc(indexFile);
	}
	fclose(indexFile);

	return temp;
}
void SetPositionIndex(PositionIndex *& temp, int& tempSize)
{
	FILE* indexFile = fopen("Company.ind", "wb");

	fwrite((char*)temp, tempSize * sizeof(PositionIndex), 1, indexFile);

	delete[] temp; 
	fclose(indexFile);
}

void PrintCompany(Company& data)
{
	if (data.flag == 0)
	{
		std::cout << "Company ID: " << '\t' << '\t' << data.companyID << std::endl <<
			"Company name: " << '\t' << '\t' << data.name << std::endl <<
			"Company website: " << '\t' << data.website;
		std::cout << std::endl << "------------------------------------------------------";
		std::cout << std::endl;
	}
}

int GetEmptyIndexForCompany(FILE* company_fl_out, PositionIndex*& company_ind, int& sizeCompany_ind)
{	
	int freeIndex = -1;
	int position = sizeCompany_ind;

	for (int i = 0; i < sizeCompany_ind; i++)
	{
		if (company_ind[i].index > i)
		{
			freeIndex = i;
			break;
		}
	}
	if (freeIndex == -1)
		freeIndex = sizeCompany_ind;

	sizeCompany_ind++;
	PositionIndex* index = new PositionIndex[sizeCompany_ind];
	
	index[freeIndex].index = freeIndex;
	index[freeIndex].position = position;

	fseek(company_fl_out, sizeof(Company) * position, SEEK_SET);

	for (int i = 0 , j = 0; i < sizeCompany_ind; i++, j++)
	{
		if (i == freeIndex)
		{
			j--;
			continue;
		}
		index[i].index = company_ind[j].index;
		index[i].position = company_ind[j].position;
	}
	company_ind = index;
	return freeIndex;
}

void InsertCompany(FILE* company_fl_out, PositionIndex* &company_ind, int& sizeCompany_ind)
{
	Company temp;
	temp.companyID = GetEmptyIndexForCompany(company_fl_out, company_ind, sizeCompany_ind);

	std::cout << "Your company ID is: " << temp.companyID << std::endl;

	std::cout << "Enter Company data" << std::endl;
	std::cout << "Company name: ";
	std::cin >> temp.name;
	std::cout << "Website: ";
	std::cin >> temp.website;

	fwrite((char*)&temp, sizeof(Company), 1, company_fl_out);
}

void GarbageCollectCompany()
{
	FILE* garbage = fopen("Garbage.txt", "wb");
	FILE* company_fl_out = fopen("Company.fl", "r+b");

	fseek(company_fl_out, 0, SEEK_SET);

	int pointerOnCompanyFL = getc(company_fl_out);

	Company temp;

	while (pointerOnCompanyFL != EOF)
	{
		fseek(company_fl_out, -1, SEEK_CUR);
		fread(&temp, sizeof(Company), 1, company_fl_out);

		if (temp.flag != 1)
		{
			fwrite((char*)&temp, sizeof(Company), 1, garbage);
		}
		pointerOnCompanyFL = getc(company_fl_out);
	}

	fclose(company_fl_out);
	fclose(garbage);
}
void RewriteFile()
{
	FILE* garbage = fopen("Garbage.txt", "rb");
	FILE* companyFile = fopen("Company.fl", "wb");
	
	Company temp;

	int iterator = getc(garbage);

	while (iterator != EOF)
	{
		fseek(garbage, -1, SEEK_CUR);
		fread(&temp, sizeof(Company), 1, garbage);
		fwrite((char*)&temp, sizeof(Company), 1, companyFile);

		iterator = getc(garbage);
	}

	fclose(companyFile);
	fclose(garbage);
}

void GetAllCompanys(FILE* company_fl_out)
{
	Company temp;

	fseek(company_fl_out, 0, SEEK_SET);
	int position = getc(company_fl_out);
	
	std::cout << "------------------------------------------------------" << std::endl;
	while (position != EOF)
	{
		fseek(company_fl_out, -1, SEEK_CUR);
		fread(&temp, sizeof(Company), 1, company_fl_out);
		PrintCompany(temp);
		position = getc(company_fl_out);
	}

	fseek(company_fl_out, 0, SEEK_END);
	std::cout << std::endl;
}
void GetCompanyByID(FILE* company_fl_out, PositionIndex*& company_ind, int& sizeCompany_ind, int ID)
{
	Company temp;
	int position;

	for(int i = 0; i < sizeCompany_ind; i++)
	{
		if (company_ind[i].index == ID)
		{
			position = company_ind[i].position;
			break;
		}
	}

	fseek(company_fl_out, sizeof(Company) * position, SEEK_SET);
	fread(&temp, sizeof(Company), 1, company_fl_out);
	std::cout << std::endl << "------------------------------------------------------";
	std::cout << std::endl;
	PrintCompany(temp);
	std::cout << std::endl;
}

void RemoveCompany(FILE* company_fl_out, PositionIndex*& company_ind, int& sizeCompany_ind, FILE* order_fl_in, int ID)
{
	int position = -1;

	for (int i = 0; i < sizeCompany_ind; i++)
	{
		if (company_ind[i].index == ID)
		{
			position = company_ind[i].position;
			break;
		}
	}

	if (position == -1)
	{
		std::cout << "ID not found" << std::endl;
		return;
	}

	fseek(company_fl_out, sizeof(Company) * position, SEEK_SET);

	Company companyTemp;
	fread(&companyTemp, sizeof(Company), 1, company_fl_out);

	companyTemp.flag = 1;
	sizeCompany_ind--;
	PositionIndex* index = new PositionIndex[sizeCompany_ind];

	if (companyTemp.firstOrderID == -1)
	{	
		fseek(company_fl_out, sizeof(Company) * position, SEEK_SET);
		fwrite((char*)&companyTemp, sizeof(Company), 1, company_fl_out);

		fclose(company_fl_out);
		GarbageCollectCompany();
		RewriteFile();
		company_fl_out = fopen("Company.fl", "r+b");

		for (int i = 0, j = 0; i < sizeCompany_ind; i++, j++)
		{
			if (company_ind[j].index == ID)
			{
				j++;
			}

			if (position < company_ind[j].position)
			{
				index[i].position = company_ind[j].position - 1;
			}
			else
			{
				index[i].position = company_ind[j].position;
			}
			index[i].index = company_ind[j].index;
		}

		company_ind = index;

		return;
	}

	fseek(company_fl_out, sizeof(Company) * position, SEEK_SET);
	fwrite((char*)&companyTemp, sizeof(Company), 1, company_fl_out);

	for (int i = 0, j = 0; i < sizeCompany_ind; i++, j++)
	{
		if (company_ind[j].index == ID)
		{
			j++;
		}

		if (position < company_ind[j].position)
		{
			index[i].position = company_ind[j].position - 1;
		}
		else
		{
			index[i].position = company_ind[j].position;
		}
		index[i].index = company_ind[j].index;
	}

	company_ind = index;
	Order orderTemp;

	fseek(order_fl_in, sizeof(Order) * companyTemp.firstOrderID, SEEK_SET);
	fread(&orderTemp, sizeof(Order), 1, order_fl_in);

	while (orderTemp.nextOrderPosition != orderTemp.orderID)
	{
		orderTemp.flag = -1;

		int nextOrderPosition = orderTemp.nextOrderPosition;
		orderTemp.nextOrderPosition = orderTemp.orderID;

		fseek(order_fl_in, sizeof(Order) * orderTemp.orderID, SEEK_SET);
		fwrite((char*)&orderTemp, sizeof(Order), 1, order_fl_in);
		fseek(order_fl_in, sizeof(Order) * nextOrderPosition, SEEK_SET);
		
		fread(&orderTemp, sizeof(Order), 1, order_fl_in);
	}

	orderTemp.flag = -1;
	fseek(order_fl_in, sizeof(Order) * orderTemp.orderID, SEEK_SET);
	fwrite((char*)&orderTemp, sizeof(Order), 1, order_fl_in);

	fclose(company_fl_out);
	GarbageCollectCompany();
	RewriteFile();
	company_fl_out = fopen("Company.fl", "r+b");
}

void UpdateCompany(FILE* company_fl_out, PositionIndex*& company_ind, int& sizeCompany_ind, int ID)
{
	Company temp;
	int position = -1;
	
	for (int i = 0; i < sizeCompany_ind; i++)
	{
		if (company_ind[i].index == ID)
		{
			position = company_ind[i].position;
			break;
		}
	}

	if (position == -1)
	{
		std::cout << "Id not found" << std::endl;
		return;
	}

	fseek(company_fl_out, sizeof(Company) * position, SEEK_SET);
	fread(&temp, sizeof(Company), 1, company_fl_out);

	std::cout << "Enter company data: " << std::endl;
	std::cout << "Name: ";
	std::cin >> temp.name;
	std::cout << "Website: ";
	std::cin >> temp.website;
	
	fseek(company_fl_out, sizeof(Company) * position, SEEK_SET);
	fwrite((char*)&temp, sizeof(Company), 1, company_fl_out);
}
