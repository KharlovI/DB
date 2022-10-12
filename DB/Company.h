#pragma once
#include <stdio.h>

struct PositionIndex
{
	int position;
	int index;
};

struct Company
{
	int companyID;
	int firstOrderID = -1;
	char name[15];
	char website[20];
	bool flag = 0;
};

PositionIndex* GetPositionIndex(int& tempSize);
void SetPositionIndex(PositionIndex*& temp, int& tempSize);

void PrintCompany(Company& data);

int GetEmptyIndexForCompany(FILE* company_fl_out, PositionIndex* &company_ind, int& sizeCompany_ind);
void GetAllCompanys(FILE* company_fl_out);
void GetCompanyByID(FILE* company_fl_out, PositionIndex*& company_ind, int& sizeCompany_ind, int ID);
void RemoveCompany(FILE* company_fl_out, PositionIndex*& company_ind, int& sizeCompany_ind, FILE* order_fl_in, int ID);
void UpdateCompany(FILE* company_fl_out, PositionIndex*& company_ind, int& sizeCompany_ind, int ID);
void InsertCompany(FILE* company_fl_out, PositionIndex* &company_ind, int& sizeCompany_ind);

void GarbageCollectCompany();
void RewriteFile();

//void PrintIndexFile(PositionIndex*& company_ind, int& sizeCompany_ind);