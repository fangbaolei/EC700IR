#include <math.h>
#include <iostream>

#include <list>
using namespace std;

typedef struct _TagPoint3D
{
	double m_x, m_y, m_z;
}Point3D;

class Vector3D{
public:
	Point3D m_point;
public:
	Vector3D(Vector3D&vector);
	Vector3D(double x, double y, double z);
	Vector3D operator=(Vector3D&vector);
	void Set(double x, double y, double z);
	void Get(double &x, double &y, double &z);
	Vector3D Normalize();//单位化
	Vector3D* CrossProduct(Vector3D &point,Vector3D *retPoint);//叉积
	double DotProduct(Vector3D &vector);//点积
	bool point_is_on_the_vector(Point3D point_0,Point3D point_1);
	     //判断点point_1是否在以point_0为起点的向量上
	int Left_or_Right(Point3D point_0, Vector3D vector_N, Point3D point_1);
	    //判断以point_0为起点，从vector的方向看，点point在向量的左侧或右侧

};

class Polygon3D{
public:
	list<Point3D> m_pointArray;
	Vector3D N;//平面法向量
	int Num;//顶点个数
public:
//	Polygon3D(int Point_Num, Point3D point_array[]);
	Polygon3D();
	Polygon3D(int Point_Num, list<Point3D>&point_array);
	void Removeall( );
	void add_element(Point3D& point);
	bool HasBorderPoint(Point3D&point);
	int HasInnerPoint(Point3D&point);
};

Vector3D::Vector3D(Vector3D&vector)
{
	m_point.m_x=vector.m_point.m_x;
	m_point.m_y=vector.m_point.m_y;
	m_point.m_z=vector.m_point.m_z;
};

Vector3D::Vector3D(double x, double y, double z)
{
	m_point.m_x=x;
	m_point.m_y=y;
	m_point.m_z=z;
};

Vector3D Vector3D::operator =(Vector3D&vector)
{
    m_point = vector.m_point;
	m_point.m_x=vector.m_point.m_x;
	m_point.m_y=vector.m_point.m_y;
	m_point.m_z=vector.m_point.m_z;
	return *this;
};

void Vector3D::Set(double x, double y, double z)
{
	m_point.m_x=x;
	m_point.m_y=y;
	m_point.m_z=z;
};

void Vector3D::Get(double &x, double &y, double &z)
{
	x=this->m_point.m_x;
	y=this->m_point.m_y;
	z=this->m_point.m_z;
};

Vector3D Vector3D::Normalize()
{
	double denominator;
	denominator = sqrt(m_point.m_x*m_point.m_x+m_point.m_y*m_point.m_y+m_point.m_z*m_point.m_z);
	if (denominator == 0)
		return *this;
	else
	{
		m_point.m_x=m_point.m_x/denominator;
	    m_point.m_y=m_point.m_y/denominator;
		m_point.m_z=m_point.m_z/denominator;
	}
	return *this;
};

Vector3D *Vector3D::CrossProduct(Vector3D &point,Vector3D *retPoint)//*this 与 point 的叉积
{
	double x=0, y=0, z=0;
	x=m_point.m_y*point.m_point.m_z-point.m_point.m_y*m_point.m_z;
	y=point.m_point.m_x*m_point.m_z-m_point.m_x*point.m_point.m_z;
	z=m_point.m_x*point.m_point.m_y-point.m_point.m_x*m_point.m_y;

    retPoint->m_point.m_x = x;
    retPoint->m_point.m_y = y;
    retPoint->m_point.m_z = z;
	return retPoint;
};


double Vector3D::DotProduct(Vector3D &vector)//点积
{
	return m_point.m_x*vector.m_point.m_x+m_point.m_y*vector.m_point.m_y+m_point.m_z*vector.m_point.m_z;
};

bool Vector3D::point_is_on_the_vector(Point3D point_0,Point3D point_1)//point_0 is the start point
{
	(*this).m_point = this->Normalize().m_point;
	Vector3D *vector = new Vector3D(0,0,0);
	vector->m_point.m_x = point_1.m_x - point_0.m_x;
	vector->m_point.m_y = point_1.m_y - point_0.m_y;
	vector->m_point.m_z = point_1.m_z - point_0.m_z;
	vector->m_point=vector->Normalize().m_point;
	double Dot=DotProduct(*vector);
	delete vector;
	if((Dot-1<1E-5) && (1-Dot<1E-5))//点在射线上
		return true;
	else
		return false;
};

int Vector3D::Left_or_Right(Point3D point_0, Vector3D vector_N, Point3D point_1)
{
	Vector3D *vector = new Vector3D(0,0,0);
	vector->m_point.m_x = point_1.m_x - point_0.m_x;
	vector->m_point.m_y = point_1.m_y - point_0.m_y;
	vector->m_point.m_z = point_1.m_z - point_0.m_z;

	vector= this->CrossProduct(*vector,vector);//vector先与原向量作叉积，
	                                  //并将叉积存入vector
	double temp= vector_N.DotProduct(*vector);//然后将vector与vector_N作点积
	delete vector;
	if( temp>0 )
		return 1;
	else
		return -1;
	//判断完毕,如果point_1在左侧或右侧，分别给F1赋值1或－1
};

Polygon3D::Polygon3D():N(0,0,1){};
//Polygon3D(int Point_Num, CArray<Point3D,Point3D>point_array);
Polygon3D::Polygon3D(int Point_Num, list<Point3D>& point_array):N(0,0,1)
//CArray<Point3D,Point3D>&point_array 此处必须为引用!!!!!!!!!!!!!!!!!!!
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
{
	Num = Point_Num;
	//m_pointArray.Copy(point_array);
	for( int i=0; i<Point_Num; i++ )
	{
		//m_pointArray.Add(point_array[i]);
		list<Point3D>::iterator it=point_array.begin();
        advance(it,i);
		Point3D node;
		node.m_x = (*it).m_x;
		node.m_y = (*it).m_y;
		node.m_z = (*it).m_z;
		m_pointArray.push_back(node);
	}
}

void Polygon3D::add_element(Point3D& point)
{
	Num++;
	//m_pointArray.Add(point);
	Point3D node;
	node.m_x = point.m_x;
	node.m_y = point.m_y;
	node.m_z = point.m_z;
	m_pointArray.push_back(node);
}

void Polygon3D::Removeall()
{
	Num = 0;
	//m_pointArray.RemoveAll();
	m_pointArray.clear();
}

bool Polygon3D::HasBorderPoint(Point3D&point)
//先检查point是否是多边形的顶点
//然后检查point是否在多边形的边界上
//算法如下：
//对每条边P1、P2生成向量 P1P 和向量PP2, 然后单位化P1P 和PP2
//然后对单位向量P1P 和PP2作点积,将结果与1进行比较
{
	bool check=0;//check作为返回值
	//首先判断point 是否是多边形的顶点
	for( int j=0; j<Num; j++ )
	{
		list<Point3D>::iterator it=m_pointArray.begin();
        advance(it,j);
		if((point.m_x==(*it).m_x) &&
			(point.m_y==(*it).m_y) &&
			(point.m_z==(*it).m_z))
			check=true;//是多边形的顶点，则返回true
		return check;
	};

	//如果不是多边形的顶点，则进一步判断point是否在各边上
	for( int i=0; i<Num; i++ )
	{
		list<Point3D>::iterator it=m_pointArray.begin();
        advance(it,i);

		Vector3D  *P1= new Vector3D(point.m_x-(*it).m_x,
		               point.m_y-(*it).m_y,
			           point.m_z-(*it).m_z);
		int m;//m用来定义i+1这点的标号
		if (i==Num-1)
			m=0;
		else
			m=i+1;

		list<Point3D>::iterator it0=m_pointArray.begin();
        advance(it0,m);
		Vector3D *P2= new Vector3D((*it0).m_x-point.m_x,
			           (*it0).m_y-point.m_y,
			           (*it0).m_z-point.m_z);
		P1->m_point= P1->Normalize().m_point;
		P2->m_point= P2->Normalize().m_point;
		double Dot=P1->DotProduct(*P2);

		delete P1;
		delete P2;
	//cout<<i<<"   "<<Dot<<endl;
		if((Dot-1<1E-5) && (1-Dot<1E-5))
		{
			std::cout<<i<<"   "<<Dot<<std::endl;
			check=true;
        return check;
		};
	};
	//检查check的值,并返回check
	return check;
};

int Polygon3D::HasInnerPoint(Point3D &point)
{
	int Count = 0;//计数器Count
	int Index = 0, Index1 = 0, Index2, F1, F2;
	Point3D P1, P2;
	//Vector3D per_L = Vector3D(1,0,0);//射线L的单位方向向量
	Vector3D *per_L = new Vector3D(1,0,0);

	if (HasBorderPoint(point))
	{//判断点是否在多边形上
		//cout<<"点在多边形上"<<endl;
		//return false;
		delete per_L;
		return 0;
	};
	for ( int i=0; i<Num; i++ )
	{//搜索第一个不在射线上的点
		list<Point3D>::iterator it=m_pointArray.begin();
        advance(it,i);
		if( !per_L->point_is_on_the_vector(point, (*it)))
		{
			Index = i;
			Index1 = i;
			P1 = (*it);
			break;
		}
		else
			continue;
	};
	//判断P1在Per_L射线的左侧或右侧
	//////////////////////////////////////////////////////////////

	F1=per_L->Left_or_Right(point, N, P1);//直接调用函数
	//cout<<F1<<endl;

	for( int j=Index+1; j<Num+Index+1; j++ )//??????
	{//进入主循环，对每条边分别判断
		int m=(j % Num);
		list<Point3D>::iterator it=m_pointArray.begin();
        advance(it,m);
		P2=(*it);
		//判断P2是否在射线per_L上，如果是，则转下次循环
		if( per_L->point_is_on_the_vector(point, P2))
			continue;
		else//如果不在射线上，则判断P2在射线哪侧,并记下P2的下标
		{
			F2=per_L->Left_or_Right(point, N, P2);
			Index2= j;
		};
		if( F1*F2==-1 )
		{
			if (Index2-Index1>1) // || (Index2-Index1<-1))
				Count++;
			else
			{
				double y=point.m_y;
				double x=(y-P1.m_y)*(P2.m_x-P1.m_x)/(P2.m_y-P1.m_y)+P1.m_x;
				//Vector3D vector_temp=Vector3D(x-point.m_x, y-point.m_y, 0);
				//if( per_L.DotProduct(vector_temp)>0 )
				if( x > point.m_x )
					Count++;
			};
		};
		Index1=Index2;
		P1=P2;
		F1=F2;
	}
	delete per_L;
	if(Count%2)
	{
		//cout<<"点在多边形内"<<endl;
		return 1;
	}
	else
	{
		//cout<<"点在多边形外"<<endl;
		return 2;
	}
	/////////////////////////////////////////////////////////////////
};

Polygon3D Polygon_result;//全局变量
