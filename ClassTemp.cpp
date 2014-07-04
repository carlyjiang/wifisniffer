#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <map>
#include <vector>


using namespace std;

class CExample
{
public:
	CExample()
	{ 
		pBuffer = NULL;
		nSize = 0;
	}

	~CExample()
	{ 
		if (pBuffer != NULL)
		{
			delete[]pBuffer;
			pBuffer = NULL;
		}
	}

	CExample(const CExample&);//拷贝构造函数
	CExample &operator=(const CExample&);//赋值符重载
	void Init(int n){ pBuffer = new char[n]; nSize = n; }
private:
	char *pBuffer;//类的对象中包含指针,指向动态分配的内存资源
	int nSize;
};

//赋值操作符重载
CExample & CExample::operator=(const CExample &RightSides)
{
	if (this == &RightSides)//如果自己给自己赋值则直接返回
	{
		return*this;
	}
	nSize = RightSides.nSize;//复制常规成员
	char *temp = new char[nSize];//复制指针指向的内容
	memcpy(temp, RightSides.pBuffer, nSize * sizeof(char));
	delete []pBuffer;//删除原指针指向内容(将删除操作放在后面，避免X=X特殊情况下，内容的丢失)
	pBuffer = temp;//建立新指向
	temp = NULL;
	return *this;
}

int v2_main()
{
	vector<int> vector1;
	vector<int> vector2;

	for (int i = 0; i < 10; i++)
		vector2.push_back(i);

	
	return 0;
}
