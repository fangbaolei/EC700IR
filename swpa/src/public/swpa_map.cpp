#include <map>
#include "swpa.h"
#include "swpa_map.h"
using namespace std;

class NODE
{
public:	
	NODE()
	{
		value = 0;
		compare = 0;
	}
	
	NODE(void* v, int (*cmp)(void*,void*) = 0)
	{
		value = v;
		compare = cmp;
	}
	
	NODE(const NODE &node)
	{
		value = node.value;
		compare = node.compare;
	}
	
	virtual ~NODE()
	{
	}
	
	bool operator<(const NODE &node) const
	{
		return 0 != compare ? 0 > compare(value, node.value) : (int)value < (int)node.value;
	}
	
	void* value;
	int (*compare)(void*,void*);
};

typedef struct tagMAP
{
	int       mutex;              //互斥量
	int       lock;               //锁的标志
	map<NODE,NODE>mymap;	        //映射表
	int       fdel;               //是否删除标志
	map<NODE,NODE>::iterator itr; //指针位置
	int (*compare)(void*,void*);  //key比较的函数指针,-1表明小于,0表明相等，1表明大于
}MAP;

/**
 *@brief 创建映射表
 *@return 成功返回映射表句柄，失败返回-1
 */
int swpa_map_create(int (*compare)(void*, void*))
{
	MAP *mymap = new MAP;
	if(0 != mymap)
	{
		mymap->lock = 0;
		mymap->fdel = 0;
		swpa_mutex_create(&mymap->mutex, 0);
		mymap->compare = compare;
		return (int)mymap;
	}
	return -1;
}

/**
 *@brief 删除映射表
 *@param [in] handle 映射表句柄
 *return 成功返回0，失败返回-1
 */
int swpa_map_destroy(int handle)
{
	MAP *mymap = (MAP *)handle;
	if(0 != mymap)
	{
		swpa_mutex_delete(&mymap->mutex);
		delete mymap;
		return 0;
	}
	return -1;
}

/**
 *@brief 添加一个节点
 *@param [in] handle 映射表句柄
 *@param [in] key 映射表键值
 *@param [in] value映射表的值
 *@return 成功返回0，失败返回-1
 */
int swpa_map_add(int handle, void* key, void* value)
{
	int ret = -1;
	MAP *mymap = (MAP *)handle;
	if(mymap)
	{
		swpa_mutex_lock(&mymap->mutex, -1);
		mymap->lock++;
		NODE k(key, mymap->compare), v(value, mymap->compare);
		map<NODE,NODE>::iterator itr = mymap->mymap.find(k);
		if(itr == mymap->mymap.end())
		{
			mymap->mymap.insert(pair<NODE,NODE>(k,v));
			ret = 0;
		}
		mymap->lock--;
		swpa_mutex_unlock(&mymap->mutex);
	}
	return ret;
}

/**
 *@brief 删除一个节点
 *@param [in] handle 映射表句柄
 *@param [in] key 映射表键值
 *@return 成功返回0，失败返回-1
 */
int swpa_map_delete(int handle, void* key)
{
	int ret = -1;
	MAP *mymap = (MAP *)handle;
	if(mymap)
	{
		swpa_mutex_lock(&mymap->mutex, -1);
		mymap->lock++;
		NODE k(key, mymap->compare);
		map<NODE,NODE>::iterator itr = mymap->mymap.find(k);
		if(itr != mymap->mymap.end())
		{
			mymap->fdel = (mymap->itr == itr);
			mymap->mymap.erase(itr++);
			if(mymap->fdel)
			{
				mymap->itr= itr;
			}
			ret = 0;
		}
		mymap->lock--;
		swpa_mutex_unlock(&mymap->mutex);
	}
	return ret;
}

/**
 *@brief 映射表的大小
 *@param [in] handle 映射表句柄
 *@return 返回映射表大小
 */
int swpa_map_size(int handle)
{
	int size = 0;
	MAP *mymap = (MAP *)handle;
	if(mymap)
	{
		swpa_mutex_lock(&mymap->mutex, -1);
		mymap->lock++;
		size = mymap->mymap.size();
		mymap->lock--;
		swpa_mutex_unlock(&mymap->mutex);
	}
	return size;
}


/**
 *@brief 查找满足key的节点
 *@param [in] handle 映射表句柄
 *@param [in] key 映射表键值
 *@return 成功返回节点的值，失败返回0
 */
void* swpa_map_find(int handle, void* key)
{
	void* value = 0;
	MAP *mymap = (MAP *)handle;
	if(mymap)
	{
		swpa_mutex_lock(&mymap->mutex, -1);
		mymap->lock++;
		NODE k(key, mymap->compare);
		map<NODE,NODE>::iterator itr = mymap->mymap.find(k);
		if(itr != mymap->mymap.end())
		{
			value = itr->second.value;
		}
		mymap->lock--;
		swpa_mutex_unlock(&mymap->mutex);
	}
	return value;
}

/**
 *@brief 打开映射表，准备遍历表
 *@param [in] handle 映射表句柄
 *@return 成功返回映射表头结点位置，失败返回-1
 */
int swpa_map_open(int handle)
{
	int ret = -1;
	MAP *mymap = (MAP *)handle;
	if(mymap && 0 < mymap->mymap.size())
	{
		swpa_mutex_lock(&mymap->mutex, -1);
		mymap->lock++;
		mymap->fdel = 0;
		mymap->itr = mymap->mymap.begin();
		ret = (int)&mymap->itr;
	}
	return ret;
}

/**
 *@brief 取得下一个节点
 *@param [in] handle 映射表句柄
 *@return 返回下一个节点位置
 */
int swpa_map_next(int handle)
{
	int ret = -1;
	MAP *mymap = (MAP *)handle;
	if(mymap && 0 < mymap->mymap.size())
	{
		if(!mymap->fdel)
		{
			mymap->itr++;
		}
		else
		{
			mymap->fdel = 0;
		}
		if(mymap->itr != mymap->mymap.end())
		{
			ret = (int)&mymap->itr;
		}
	}
	return ret;
}

/**
 *@brief 关闭映射表，停止遍历
 *@param [in] handle 映射表句柄
 *@return 成功返回0，失败返回-1
 */
int swpa_map_close(int handle)
{
	int ret = -1;
	MAP *mymap = (MAP *)handle;
	if(mymap && mymap->lock > 0)
	{
		mymap->lock--;
		swpa_mutex_unlock(&mymap->mutex);
		ret = 0;
	}
	return ret;
}

/**
 *@brief 得到当前节点的key
 *@param [in] handle 映射表句柄
 *@param [in] pos 位置
 *@return 成功返回节点的key,失败返回0
 */
void* swpa_map_key(int handle, int pos)
{
	void* key = 0;
	MAP *mymap = (MAP *)handle;
	if(mymap && 0 < pos)
	{
		swpa_mutex_lock(&mymap->mutex, -1);
		mymap->lock++;
		map<NODE,NODE>::iterator itr = *(map<NODE,NODE>::iterator *)pos;
		key = itr->first.value;
		mymap->lock--;
		swpa_mutex_unlock(&mymap->mutex);
	}
	return key;
}

/**
 *@brief 得到当前节点的value
 *@param [in] handle 映射表句柄
 *@param [in] pos 位置
 *@return 成功返回节点的value,失败返回0
 */
void* swpa_map_value(int handle, int pos)
{
	void* value = 0;
	MAP *mymap = (MAP *)handle;
	if(mymap && 0 < pos)
	{
		swpa_mutex_lock(&mymap->mutex, -1);
		mymap->lock++;
		map<NODE,NODE>::iterator itr = *(map<NODE,NODE>::iterator *)pos;
		value = itr->second.value;
		mymap->lock--;
		swpa_mutex_unlock(&mymap->mutex);
	}
	return value;
}

